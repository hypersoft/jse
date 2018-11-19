/*

 */

#include "jse.h"

#define CLASSNAME "Constructor"

JSClass JSConstructorClass = NULL;

typedef struct ConstructorData {
	JSClass class;
	JSFunction function;
	JSString name;
} ConstructorData;

static void ConstructorFinalize(JSObject object)
{
	ConstructorData * p = JSObjectGetPrivate(object);
	if (!p) return;
	if (p->name) JSStringRelease(p->name);
	g_free(p);
}

static JSValue ConstructorGetName(JSContext ctx, JSObject object, JSString name, JSValue * exception)
{
	ConstructorData * p = JSObjectGetPrivate(object);
	if (!p) return NULL;
	return JSValueMakeString(ctx, p->name);
}

static JSValue ConstructorCallAsFunction (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	ConstructorData * p = JSObjectGetPrivate(function);
	if (! p ) return NULL;
	return p->function(ctx, function, this, argc, argv, exception);
}

static JSObject ConstructorCallAsConstructor (JSContext ctx, JSObject function, size_t argc, const JSValue argv[], JSValue * exception)
{
	ConstructorData * p = JSObjectGetPrivate(function);
	if (! p ) return NULL;
	JSObject this = JSObjectMake(ctx, p->class, NULL);
	JSObjectSetPrototype(ctx, this, JSObjectGetUtf8Property(ctx, function, "prototype"));
	return (JSObject) p->function(ctx, function, this, argc, argv, exception);
}

static bool ConstructorHasInstance(JSContext ctx, JSObject constructor, JSValue object, JSValue * exception)
{
	return JSObjectGetUtf8Property(ctx, (void *) object, "constructor") == constructor;
}

static JSValue ConstructorObjectConvertToType(JSContext ctx, JSObject object, JSType type, JSValue * exception)
{
	if (type != kJSTypeString) return NULL;
	ConstructorData * p = JSObjectGetPrivate(object);
	if (! p ) return NULL;
	char name[JSStringUtf8Size(p->name)];
	JSStringGetUTF8CString(p->name, name, sizeof(name));
	char buffer[1024];
	sprintf(buffer, "function %s()\n    [native code]\n}", name);
	return JSValueFromUtf8(ctx, buffer);
}

static JSStaticValue ConstructorValues[] = {
	{"name", (void*) ConstructorGetName, NULL, kJSPropertyAttributeDontDelete},	/* NAME, GET, SET, OBJECT ATTRIBUTES */
	{NULL, NULL, NULL, 0}	/* DO NOT MODIFY, LEAVE LAST */
};

static JSStaticFunction ConstructorFunctions[] = {
	{NULL, NULL, 0},		/* NAME, FUNCTION, OBJECT ATTRIBUTES*/
	{NULL, NULL, 0}			/* DO NOT MODIFY, LEAVE LAST */
};

static JSClassDefinition ConstructorDefinition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	CLASSNAME,								/* Class Name */
	NULL,									/* Parent Class */
	ConstructorValues,						/* Static Values */
	ConstructorFunctions,					/* Static Functions */
	NULL,									/* Object Initializer */
	(void*) ConstructorFinalize,			/* Object Finalizer */
	NULL,									/* Object Has Property */
	NULL,									/* Object Get Property */
	NULL,									/* Object Set Property */
	NULL,									/* Object Delete Property */
	NULL,									/* Object Get Property Names */
	(void*) ConstructorCallAsFunction,		/* new Object Call As Function */
	(void*) ConstructorCallAsConstructor,	/* new Object Call As Constructor */
	(void*) ConstructorHasInstance,			/* Has Instance */
	(void*) ConstructorObjectConvertToType	/* Object Convert To Type */
};

JSObject JSConstructorCreate(JSContext ctx, char * name, JSClass class, JSFunction function)
{
	if (!JSConstructorClass) JSConstructorClass = JSClassCreate(&ConstructorDefinition);
	ConstructorData * data = g_malloc0(sizeof(ConstructorData));
	JSObject constructor = JSObjectMake(ctx, JSConstructorClass, data);
	JSObjectSetUtf8Property(ctx, constructor, "prototype", (JSValue)JSObjectMake(ctx, NULL, NULL), kJSPropertyAttributeDontEnum);
	data->class = class, data->function = function,
	data->name = JSStringRetain(JSStringCreateWithUTF8CString(name));
	return constructor;
}
