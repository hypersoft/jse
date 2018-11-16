#include <sys/wait.h>
#include <signal.h>


#include <jse.h>

// this definition just makes it easy to change the class name
#define CLASSNAME "Fork"

JSClass JSForkClass;

typedef struct js_fork_data {
	JSContextGroupRef contextGroup;
	JSGlobalContextRef parentContext;
	JSGlobalContextRef ownContext;
	int pid;
	JSValue result, exception;
} ProcessFork;

static void ForkObjectInitialize(JSContext ctx, JSObject object)
{
	ProcessFork * pf = g_malloc0(sizeof(ProcessFork));
	pf->parentContext = ctx;
	pf->contextGroup = JSContextGetUniverse();
	pf->ownContext = JSGlobalContextCreateInGroup(pf->contextGroup, NULL);
	JSContextGroupRetain(pf->contextGroup);
	JSGlobalContextRetain(pf->parentContext);
	JSGlobalContextRetain(pf->ownContext);
	JSObjectSetPrivate(object, pf);
}

static void ForkObjectFinalize(JSObject object)
{
	ProcessFork * pf = JSObjectGetPrivate(object);
	JSGlobalContextRelease(pf->ownContext);
	JSGlobalContextRelease(pf->parentContext);
	JSContextGroupRelease(pf->contextGroup);
	g_free(pf);
	JSObjectSetPrivate(object, NULL);
}

static JSValue ForkObjectGetProperty(JSContext ctx, JSObject object, JSString id, JSValue * exception)
{	

	ProcessFork * pf = JSObjectGetPrivate(object);
	char name[JSStringUtf8Size(id)]; JSStringGetUTF8CString (id, name, sizeof(name));
	if (!g_strcmp0(name, "pid")) {
		return JSValueFromNumber(ctx, pf->pid);
	} else if (!g_strcmp0(name, "exception")) {
		if (pf->exception) return pf->exception;
		else return JSValueMakeUndefined(ctx);
	}

   /*
	If this function returns NULL, the get request forwards to object's
	statically declared properties, then its parent class chain (which
	includes the default object class), then its prototype chain.
    */
	return NULL;
}

static bool ForkObjectSetProperty (JSContext ctx, JSObject object, JSString id, JSValue value, JSValue * exception)
{
		ProcessFork * pf = JSObjectGetPrivate(object);
		char name[JSStringUtf8Size(id)]; JSStringGetUTF8CString (id, name, sizeof(name));
		if (!g_strcmp0(name, "pid") || !g_strcmp0(name, "exception")) {
			return true;
		}
	
	/*
	If this function returns false, the set request forwards to object's
	statically declared properties, then its parent class chain (which
	includes the default object class).
    */
	return false;
}

static bool ForkObjectDeleteProperty (JSContext ctx, JSObject object, JSString id, JSValue * exception)
{
	  /*
	If this function returns false, the delete request forwards to object's
	statically declared properties, then its parent class chain (which
	includes the default object class).
    */
	return false;
}

static void ForkObjectGetPropertyNames (JSContext ctx, JSObject object, JSPropertyNameAccumulator names)
{
	  /*
	Use JSPropertyNameAccumulatorAddName to add property names to accumulator.
	A class's getPropertyNames callback only needs to provide the names of
	properties that the class offers through a custom getProperty or
	setProperty callback. Other properties, including statically declared
	properties, properties offered by other classes, and properties belonging
	to object's prototype, are added independently.
    */
	 JSPropertyNameAccumulatorAddName(names, JSStringFromUtf8("pid"));
	 JSPropertyNameAccumulatorAddName(names, JSStringFromUtf8("exception"));
}

static JSValue ForkObjectConvertToType(JSContext ctx, JSObject object, JSType type, JSValue * exception)
{
	if (type == kJSTypeNumber) {
		ProcessFork * pf = JSObjectGetPrivate(object);
		return JSValueFromNumber(ctx, pf->pid);
	}
	/*
	If this function returns NULL, the conversion request forwards to
	object's parent class chain (which includes the default object class).

	This function is only invoked when converting object to a number or
	a string. An object converted to boolean is 'true.' An object converted
	to object is itself.
*/
	return NULL;
}

// JSValue ForkObjectWait(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
// {
// 	ProcessFork * pf = JSObjectGetPrivate(argv[0]);
// 	JSValue r;
// 	JSObjectSetUtf8Property(ctx, argv[0], "status", r = JSValueFromNumber(ctx, wait(&pf->pid)), 0);
// 	return r;
// }

static JSClassDefinition ForkObjectDefinition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	CLASSNAME,								/* Class Name */
	NULL,									/* Parent Class */
	NULL,									/* Static Values */
	NULL,								/* Static Functions */
	(void*) ForkObjectInitialize,						/* ForkObject Initializer */
	(void*) ForkObjectFinalize,						/* ForkObject Finalizer */
	(void*) NULL,					/* ForkObject Has Property */
	(void*) ForkObjectGetProperty,					/* ForkObject Get Property */
	(void*) ForkObjectSetProperty,					/* ForkObject Set Property */
	(void*) ForkObjectDeleteProperty,					/* ForkObject Delete Property */
	(void*) ForkObjectGetPropertyNames,				/* ForkObject Get Property Names */
	(void*) NULL,					/* new ForkObject Call As Function */
	(void*) NULL,				/* new ForkObject Call As Constructor */
	(void*) NULL,					/* Has Instance */
	(void*) ForkObjectConvertToType					/* ForkObject Convert To Type */
};

JSObject ForkObjectConstructor (JSContext ctx, JSObject constructor, size_t argc, const JSValue argv[], JSValue* exception)
{
	
	JSObject self = JSObjectMake(ctx, JSForkClass, NULL);

	ProcessFork * pf = JSObjectGetPrivate(self);

	JSObject scope = JSValueToObject(ctx, argv[0], NULL);
	JSObject func = JSValueToObject(pf->ownContext, argv[1], NULL);

	JSValue args[argc - 2];
	for (int i = 2; i < argc; i++) args[i - 2] = argv[i];

	//JSObjectSetUtf8Property(ctx, self, "scope", scope, 0);
	
	int c = fork();

	if (c == 0) {
		pf->result = JSObjectCallAsFunction(pf->ownContext, func, scope, sizeof(args), args, &pf->exception);
		exit((pf->exception != 0)?1:0);
	} else {
		pf->pid = c;
	}
	return self;

}

JSValue jsWait(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	int pid = JSValueToNumber(ctx, argv[0], exception);
	return JSValueFromNumber(ctx, wait(&pid));
}


JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{

	if (!JSForkClass)
		JSForkClass = JSClassCreate(&ForkObjectDefinition);

	JSObject constructor = JSObjectCreateConstructor(
		ctx, JSForkClass, ForkObjectConstructor
	);

	//JSObjectCreateFunction(ctx, constructor, "wait", ForkObjectWait);
	JSObjectCreateFunction(ctx, object, "wait", jsWait);

	JSObjectSetUtf8Property(ctx, object, CLASSNAME, (JSValue) constructor, 0);

	JSClassRetain(JSForkClass);

	return (JSValue) object;

}

void unload(JSContext ctx)
{
		JSClassRelease(JSForkClass);
}

