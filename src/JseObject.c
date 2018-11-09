#include "jse.h"

bool JSObjectIsFrozen(JSContext ctx, JSObject object)
{
	JSValue isFrozen = JSInlineEval(ctx, "Object.isFrozen(this)", object, NULL);
	return JSValueToBoolean(ctx, isFrozen);
}

JSObject JSObjectCreateFunction(JSContext ctx, JSObject this, char * name, void * callback)
{
	JSString functionName = JSStringFromUtf8(name);
	JSObject function = JSObjectMakeFunctionWithCallback(ctx, functionName, (JSObjectCallAsFunctionCallback)callback);
	if (this) {
		JSObjectSetProperty(ctx, this, functionName, (JSValue)function, 0, NULL);
	}
	JSStringRelease(functionName);
	return function;
}

JSObject JSObjectCreateConstructor(JSContext ctx, JSClass class, JSObject (*constructor)(JSContext ctx, JSObject constructor, size_t argc, const JSValue argv[], JSValue* exception))
{
	return JSObjectMakeConstructor(ctx, class, (JSObjectCallAsConstructorCallback) constructor);
}

bool JSObjectHasUtf8Property(JSContext ctx, JSObject object, char * property)
{
	JSString propertyName = JSStringFromUtf8(property);
	bool hasProperty = JSObjectHasProperty(ctx, object, propertyName);
	JSStringRelease(propertyName);
	return hasProperty;
}

JSValue JSObjectGetUtf8Property(JSContext ctx, JSObject object, char * property)
{
	JSString propertyName = JSStringFromUtf8(property);
	JSValue propertyValue = JSObjectGetProperty(ctx, object, propertyName, NULL);
	JSStringRelease(propertyName);
	return propertyValue;
}

bool JSObjectSetUtf8Property(JSContext ctx, JSObject object, char * property, JSValue value, JSPropertyAttributes attributes)
{
	JSValue error = NULL;
	JSString propertyName = JSStringFromUtf8(property);
	JSObjectSetProperty(ctx, object, propertyName, value, attributes, &error);
	JSStringRelease(propertyName);
	if (error) return false;
	return true;
}

bool JSObjectDeleteUtf8Property(JSContext ctx, JSObject object, char * property)
{
	JSString propertyName = JSStringFromUtf8(property);
	bool deleted = JSObjectDeleteProperty(ctx, object, propertyName, NULL);
	JSStringRelease(propertyName);
	return deleted;
}

bool JSObjectModifyUtf8Property(JSContext ctx, JSObject object, char * property, JSPropertyAttributes attribs, JSValue * exception) {
	JSString propertyName = JSStringFromUtf8(property);
	JSValue member = JSObjectGetProperty(ctx, object, propertyName, exception);
	if (exception && *exception) return false;
	if (! JSObjectDeleteProperty(ctx, object, propertyName, exception)) return false;
	JSObjectSetProperty(ctx, object, propertyName, member, attribs, exception);
	JSStringRelease(propertyName);
	return (exception)?*exception == NULL:true;
}