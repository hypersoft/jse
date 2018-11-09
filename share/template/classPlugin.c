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

// this definition just makes it easy to change the class name
#define CLASSNAME "Plugin"

JSClass PluginClass;

static void ObjectInitialize(JSContext ctx, JSObject object)
{
   /*
	Unlike the other object call backs, the initialize callback is called on
	the least derived class (the parent class) first, and the most derived
	class last.
    */
	g_print("Called Plugin Object Initialize\n");
}

static void ObjectFinalize(JSObject object)
{	g_print("Called Plugin Object Finalize\n");
   /*
	The finalize callback is called on the most derived class first, and the
	least derived class (the parent class) last.

	You must not call any function that may cause a garbage collection or an
	allocation of a garbage collected object from within a
	JSObjectFinalizeCallback. This includes all functions that have a
	JSContextRef parameter.
    */
}

static bool ObjectHasProperty (JSContext ctx, JSObject object, JSString id)
{	g_print("Called Plugin Object Has Property\n");
   /*
	If this function returns false, the hasProperty request forwards to
	object's statically declared properties, then its parent class chain
	(which includes the default object class), then its prototype chain.

	This callback enables optimization in cases where only a property's
	existence needs to be known, not its value, and computing its value
	would be expensive.

	If this callback is NULL, the getProperty callback will be used to
	service hasProperty requests.
    */
	char name[JSStringUtf8Size(id)];
	JSStringGetUTF8CString (id, name, sizeof(name));

	return false;
}

static JSValue ObjectGetProperty(JSContext ctx, JSObject object, JSString id, JSValue * exception)
{	g_print("Called Plugin Object Get Property\n");
   /*
	If this function returns NULL, the get request forwards to object's
	statically declared properties, then its parent class chain (which
	includes the default object class), then its prototype chain.
    */
	return NULL;
}

static bool ObjectSetProperty (JSContext ctx, JSObject object, JSString id, JSValue value, JSValue * exception)
{	g_print("Called Plugin Object Set Property\n");
   /*
	If this function returns false, the set request forwards to object's
	statically declared properties, then its parent class chain (which
	includes the default object class).
    */
	return false;
}

static bool ObjectDeleteProperty (JSContext ctx, JSObject object, JSString id, JSValue * exception)
{	g_print("Called Plugin Object Delete Property\n");
   /*
	If this function returns false, the delete request forwards to object's
	statically declared properties, then its parent class chain (which
	includes the default object class).
    */
	return false;
}

static void ObjectGetPropertyNames (JSContext ctx, JSObject object, JSPropertyNameAccumulator names)
{	g_print("Called Plugin Object Get Property Names\n");
   /*
	Use JSPropertyNameAccumulatorAddName to add property names to accumulator.
	A class's getPropertyNames callback only needs to provide the names of
	properties that the class offers through a custom getProperty or
	setProperty callback. Other properties, including statically declared
	properties, properties offered by other classes, and properties belonging
	to object's prototype, are added independently.
    */
}

static JSValue ObjectCallAsFunction (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{	g_print("Plugin Object Called as Function\n");
   /*
	If your callback were invoked by the JavaScript expression
	'myObject.myFunction()', `function' would be set to myFunction, and
	`this' would be set to myObject.

	If this callback is NULL, calling your object as a function will throw
	an exception.
    */
	return NULL;
}

static JSObject ObjectCallAsConstructor (JSContext ctx, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{	g_print("Plugin Object Called as Constructor\n");
   /*
	If your callback were invoked by the JavaScript expression
	'new myConstructor()', `this' would be set to myConstructor.

	If this callback is NULL, using your object as a constructor in a
	'new' expression will throw an exception.
    */
	return NULL;
}

static bool ObjectHasInstance(JSContext ctx, JSObject constructor, JSValue object, JSValue * exception)
{	g_print("Called Plugin Object Has Instance\n");
   /*
	If your callback were invoked by the JavaScript expression
	'someValue instanceof myObject', `constructor' would be set to myObject
	and `object' would be set to someValue.

	If this callback is NULL, 'instanceof' expressions that target your
	object will return false.

	Standard JavaScript practice calls for objects that implement the
	callAsConstructor callback to implement the hasInstance callback as well.
    */
	return false;
}

static JSValue ObjectConvertToType(JSContext ctx, JSObject object, JSType type, JSValue * exception)
{	g_print("Called Plugin Object Convert to Type\n");
/*
	If this function returns NULL, the conversion request forwards to
	object's parent class chain (which includes the default object class).

	This function is only invoked when converting object to a number or
	a string. An object converted to boolean is 'true.' An object converted
	to object is itself.
*/
	return NULL;
}

static JSStaticValue ObjectValues[] = {

	/* GET follows the same prototype as getProperty */
	/* SET follows the same prototype as setProperty */

	/*
	 We cast the types to void * because we use simplified type names
	 and we don't provide the full interface replacements.
	 We don't have to do this, but it prevents the compiler from spitting
	 out useless errors.
	 */

	{NULL, (void*) ObjectGetProperty, (void*) ObjectSetProperty, 0},	/* NAME, GET, SET, OBJECT ATTRIBUTES */
	{NULL, NULL, NULL, 0}	/* DO NOT MODIFY, LEAVE LAST */

};

static JSStaticFunction ObjectFunctions[] = {

	/* these functions follow the same prototype as callAsFunction */

	{NULL, NULL, 0},		/* NAME, FUNCTION, OBJECT ATTRIBUTES*/
	{NULL, NULL, 0}			/* DO NOT MODIFY, LEAVE LAST */

};

static JSClassDefinition ObjectDefinition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	CLASSNAME,								/* Class Name */
	NULL,									/* Parent Class */
	ObjectValues,									/* Static Values */
	ObjectFunctions,								/* Static Functions */
	(void*) ObjectInitialize,						/* Object Initializer */
	(void*) ObjectFinalize,						/* Object Finalizer */
	(void*) ObjectHasProperty,					/* Object Has Property */
	(void*) ObjectGetProperty,					/* Object Get Property */
	(void*) ObjectSetProperty,					/* Object Set Property */
	(void*) ObjectDeleteProperty,					/* Object Delete Property */
	(void*) ObjectGetPropertyNames,				/* Object Get Property Names */
	(void*) ObjectCallAsFunction,					/* new Object Call As Function */
	(void*) ObjectCallAsConstructor,				/* new Object Call As Constructor */
	(void*) ObjectHasInstance,					/* Has Instance */
	(void*) ObjectConvertToType					/* Object Convert To Type */
};

JSObject ClassObjectConstructor (JSContext ctx, JSObject constructor, size_t argc, const JSValue argv[], JSValue* exception)
{	g_print("Called Plugin Class Object Constructor\n");
	return JSObjectMake(ctx, PluginClass, NULL);
}

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{	g_print("Called Plugin Load\n");
	if (!PluginClass)
		PluginClass = JSClassCreate(&ObjectDefinition);

	JSObject constructor = JSObjectCreateConstructor(
		ctx, PluginClass, ClassObjectConstructor
	);

	JSObjectSetUtf8Property(ctx, object, CLASSNAME,
		(JSValue) constructor, 0
	);

	JSClassRetain(PluginClass);

	return (JSValue) object;

}

void unload(JSContext ctx)
{	g_print("Called Plugin Unload\n");
	JSClassRelease(PluginClass);
}

