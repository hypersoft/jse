/*

 This is a template file for a single class plug-in. Plug-ins can contain
 multiple classes.

 The provided loadCount variable is not necessary, but demonstrates reference
 counting by a plug-in.

 The associated function comments are exported from javascriptcoregtk-4.0.

 You should not expect this class to work 'out of the box,' it merely
 demonstrates how your class plug-in should be defined.

 */

#include <jse.h>

#define CLASSNAME "Environment"

#define DESCRIPTION R/W Thread safe environment access

static GRWLock lock;

JSClass Environment;

static JSValue EnvObjectGetProperty(JSContext ctx, JSObject object, JSString name, JSValue * exception)
{
	/*
	If this function returns NULL, the get request forwards to object's
	statically declared properties, then its parent class chain (which
	includes the default object class), then its prototype chain.
    */

	g_rw_lock_reader_lock(&lock);

	char buffer[JSStringUtf8Size(name)];

	JSStringGetUTF8CString (name, buffer, sizeof(buffer));

	char * data = (void*)g_getenv(buffer);

	if (! data ) return NULL;

	JSValue value = JSValueFromUtf8(ctx, data);

	g_rw_lock_reader_unlock(&lock);

	return value;

}

static bool setProperty (JSContext ctx, JSObject object, JSString name, JSValue value, JSValue * exception)
{
   /*
	If this function returns false, the set request forwards to object's
	statically declared properties, then its parent class chain (which
	includes the default object class).
    */

	g_rw_lock_writer_lock(&lock);

	JSString vdata = JSValueToStringCopy(ctx, value, exception);
	if (exception && *exception) return NULL;

	char buffer[JSStringUtf8Size(name)], vbuffer[JSStringUtf8Size(vdata)];

	JSStringGetUTF8CString (name, buffer, sizeof(buffer));
	JSStringGetUTF8CString (vdata, vbuffer, sizeof(vbuffer));

	JSStringRelease(vdata);

	JSValue status =  JSValueMakeBoolean(ctx, g_setenv(buffer, vbuffer, true));

	g_rw_lock_writer_unlock(&lock);

	return status;

}

static bool deleteProperty (JSContext ctx, JSObject object, JSString name, JSValue * exception)
{
   /*
	If this function returns false, the delete request forwards to object's
	statically declared properties, then its parent class chain (which
	includes the default object class).
    */

	g_rw_lock_writer_lock(&lock);

	char buffer[JSStringUtf8Size(name)];
	JSStringGetUTF8CString (name, buffer, sizeof(buffer));
	g_unsetenv(buffer);

	g_rw_lock_writer_unlock(&lock);

	return true;
}

static void LibraryObjectGetPropertyNames (JSContext ctx, JSObject object, JSPropertyNameAccumulator props)
{
   /*
	Use JSPropertyNameAccumulatorAddName to add property names to accumulator.
	A class's getPropertyNames callback only needs to provide the names of
	properties that the class offers through a custom getProperty or
	setProperty callback. Other properties, including statically declared
	properties, properties offered by other classes, and properties belonging
	to object's prototype, are added independently.
    */

	g_rw_lock_reader_lock(&lock);
	char ** names = g_listenv();
	g_rw_lock_reader_unlock(&lock);

	char * name;
	int i = 0;

	while(name = names[i++]) {
		JSString data = JSStringFromUtf8(name);
		JSPropertyNameAccumulatorAddName(props, data);
		JSStringRelease(data);
	}

}

static JSClassDefinition definition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	CLASSNAME,								/* Class Name */
	NULL,									/* Parent Class */
	NULL,									/* Static Values */
	NULL,								/* Static Functions */
	NULL,						/* Object Initializer */
	NULL,						/* Object Finalizer */
	NULL,					/* Object Has Property */
	(void*) EnvObjectGetProperty,					/* Object Get Property */
	(void*) setProperty,					/* Object Set Property */
	(void*) deleteProperty,					/* Object Delete Property */
	(void*) LibraryObjectGetPropertyNames,				/* Object Get Property Names */
	NULL,					/* new Object Call As Function */
	NULL,				/* new Object Call As Constructor */
	NULL,					/* Has Instance */
	NULL					/* Object Convert To Type */
};

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{
	if (!Environment) {
		Environment = JSClassCreate(&definition);
		g_rw_lock_init(&lock);
	}
	JSObject env = JSObjectMake(ctx, Environment, NULL);

	JSObjectSetUtf8Property(ctx, object, CLASSNAME,
		(JSValue) env, kJSPropertyAttributeDontDelete
	);

	return (JSValue) object;

}

void unload(JSContext ctx)
{
	g_rw_lock_clear(&lock);
}