#include <sys/wait.h>
#include <signal.h>


#include <jse.h>

// this definition just makes it easy to change the class name
#define CLASSNAME "Fork"

JSClass JSForkClass;

static JSValue ForkObjectConvertToType(JSContext ctx, JSObject object, JSType type, JSValue * exception)
{
	if (type == kJSTypeNumber) {
		return JSObjectGetUtf8Property(ctx, object, "pid");
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
	(void*) NULL,						/* ForkObject Initializer */
	(void*) NULL,						/* ForkObject Finalizer */
	(void*) NULL,					/* ForkObject Has Property */
	(void*) NULL,					/* ForkObject Get Property */
	(void*) NULL,					/* ForkObject Set Property */
	(void*) NULL,					/* ForkObject Delete Property */
	(void*) NULL,				/* ForkObject Get Property Names */
	(void*) NULL,					/* new ForkObject Call As Function */
	(void*) NULL,				/* new ForkObject Call As Constructor */
	(void*) NULL,					/* Has Instance */
	(void*) ForkObjectConvertToType					/* ForkObject Convert To Type */
};

JSObject ForkObjectConstructor (JSContext ctx, JSObject constructor, size_t argc, const JSValue argv[], JSValue* exception)
{
	
	JSObject self = JSObjectMake(ctx, JSForkClass, NULL);
	JSObject scope = JSValueToObject(ctx, argv[0], NULL);
	JSObject func = JSValueToObject(ctx, argv[1], NULL);

	JSValue args[argc - 2];
	for (int i = 2; i < argc; i++) args[i - 2] = argv[i];
	
	int c = fork();

	if (c == 0) {
		JSValue exn;
		JSObjectCallAsFunction(ctx, func, scope, argc - 2, args, &exn);
		JSReportException(ctx, "fork", exn);
		exit((exn != 0)?1:0);
	} else {
		JSObjectSetUtf8Property(ctx, self, "pid", JSValueFromNumber(ctx, c), kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly);
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

