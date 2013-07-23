/* Brigadier */
#define JSNativeSource

#include "JSNative.inc"
#include "JSNativeClass.inc"

#include "JSNative.h"

JSObjectRef RtJSNativeAPI;
JSObjectRef RtJSNativeClassRegistry;

JSClassRef JSNativeGhost;

static JSStringRef RtCSPrototype, RtCSSetProperty, RtCSGetProperty, RtCSConstruct, RtCSClassRegister,
RtCSConvert;

static JSValueRef jsConvertProperty JSTNativeConvertor() {
	JSValueRef conversion;
	JSObjectRef convert = JSTCoreGetPropertyObject(object, RtCSConvert);
	if (convert && JSTReference(convert)) {
		if (kJSTypeString == type) {
			conversion = JSTCall(convert, object, RtJS(String));
		} else
		if (kJSTypeNumber == type) {
			conversion = JSTCall(convert, object, RtJS(Number));
		} else
		if (kJSTypeBoolean == type) {
			conversion = JSTCall(convert, object, RtJS(Boolean));
		} else
		if (kJSTypeObject == type) {
			conversion = JSTCall(convert, object, RtJS(Object));
		} else
		if (kJSTypeNull == type) {
			conversion = JSTCall(convert, object, JSTMakeNull());
		} else
		if (kJSTypeUndefined == type) {
			conversion = JSTCall(convert, object, RtJS(undefined));
		}
		if (JSTBoolean(JSTEval("this === JSNative.api.conversionFailure", conversion))) return false;
		return conversion;
	}
	return false;
}

static bool jsSetProperty JSTNativePropertyWriter() {
	if (JSTCoreHasProperty(object, RtCSSetProperty))
	return JSTBoolean(JSTCoreCallProperty(object, RtCSSetProperty, JSTMakeString(property, NULL, false), value));
	return false;
}

static JSValueRef jsGetProperty JSTNativePropertyReader() {
	if (JSTCoreEqualsNative(property, "setProperty")) return NULL;
	if (JSTCoreEqualsNative(property, "getProperty")) return NULL;
	if (JSTCoreEqualsNative(property, "callAsConstructor")) return NULL;
	if (JSTCoreEqualsNative(property, "callAsFunction")) return NULL;
	if (JSTCoreEqualsNative(property, "prototype")) return NULL;
	JSObjectRef get = JSTGetPropertyObject(object, "getProperty");
	if (get && ! JSTUndefined(get)) {
		JSValueRef result = JSTCoreCallProperty(object, RtCSGetProperty, JSTMakeString(property, NULL, false));
		if (JSTNull(result)) return NULL;
		return result;
	}
	return NULL;
}

JSValueRef jsGhostCallAsFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef this, size_t argc, const JSValueRef argv[], JSValueRef* exception);
JSObjectRef jsGhostCallAsConstructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

bool jsHasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName) {
	JSValueRef * exception = NULL;
	if (JSTCoreEqualsNative(propertyName, "hasProperty")) return false;
	return JSTCallProperty(object, "hasProperty", JSTMakeString(propertyName, NULL, false));
}


static JSValueRef jsRegisterClass JSToolsFunction () {

	char * buffer = NULL;
	JSValueRef className = JSTGetProperty(this, "name");

	JSClassDefinition jsClass = kJSClassDefinitionEmpty;
	jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsClass.getProperty = &jsGetProperty;
	jsClass.setProperty = &jsSetProperty;
	jsClass.convertToType = &jsConvertProperty;
//	jsClass.hasProperty = &jsHasProperty;
	jsClass.callAsFunction = &jsGhostCallAsFunction;
//	jsClass.callAsConstructor = &jsGhostCallAsConstructor;
	jsClass.className = JSTGetValueBuffer(className, &buffer);

	JSClassRef jsClassRef = JSClassRetain(JSClassCreate(&jsClass));

	JSValueRef valueOf = (JSValueRef) JSTCreateClassObject(JSNativeGhost, jsClassRef);
	JSTSetPrototype((JSObjectRef)valueOf, JSTGetPrototype(this));
	JSTSetProperty((JSObjectRef)valueOf,"name", className, 0);
	JSTFreeBuffer(buffer);

	return valueOf;

}

JSValueRef jsGhostCallAsFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef this, size_t argc, const JSValueRef argv[], JSValueRef* exception) {

	JSObjectRef classFunction = JSTGetPropertyObject(function, "callAsFunction");
	if (! JSTReference(classFunction) ) {
		JSTReferenceError("call as function not defined");
		JSTReturnValueException;
	}
	
	return JSTRelayFunctionCall(classFunction);

}

JSObjectRef jsGhostCallAsConstructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	JSObjectRef classConstructor = JSTGetPropertyObject(constructor, "callAsConstructor");
	JSClassRef jsClassRef = JSTGetPrivate(constructor);
	JSObjectRef this = JSTCreateClassObject(jsClassRef, NULL);
	JSTSetPrototype(this, JSTGetProperty(constructor, "prototype"));
	if (! JSTReference(classConstructor) ) {
		//JSTReferenceError("call as constructor not defined");
		//JSTReturnObjectException;
		return this;
	}
	JSValueRef deviation = JSObjectCallAsFunction(ctx, classConstructor, this, argumentCount, arguments, exception);
	if (!JSTUndefined(deviation)) return (JSObjectRef)deviation;
	return this;
}

void js_native_init JSToolsProcedure (int argc, char *argv[], char *envp[]) {

	JSObjectRef global = JSTGetGlobalObject();

	JSTLoadRuntime(global, argc, argv, envp);

	RtCSPrototype = JSTCreateStaticString("prototype", NULL);
	RtCSSetProperty = JSTCreateStaticString("setProperty", NULL);
	RtCSGetProperty = JSTCreateStaticString("getProperty", NULL);
	RtCSConstruct = JSTCreateStaticString("constructor", NULL);
	RtCSClassRegister = JSTCreateStaticString("$registry", NULL);
	RtCSConvert = JSTCreateStaticString("convert", NULL);

	JSClassDefinition jsClass = kJSClassDefinitionEmpty;
	jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsClass.callAsConstructor = &jsGhostCallAsConstructor;
	jsClass.callAsFunction = &jsGhostCallAsFunction;
	jsClass.getProperty = &jsGetProperty;
	jsClass.setProperty = &jsSetProperty;
	jsClass.convertToType = &jsConvertProperty;

	jsClass.className = "JSNative.Class";

	JSNativeGhost = JSClassRetain(JSClassCreate(&jsClass));
	
	RtJSNativeAPI = JSTCreateClassObject(NULL,NULL);
	JSTSetProperty(global, "api", RtJSNativeAPI, 0);

	JSTSetPropertyFunction(RtJSNativeAPI, "registerClass", &jsRegisterClass);

	JSTEvalScript(JSNativeClass, global, "JSNativeClass.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);


	JSTEvalScript(JSNativeSupport, global, "JSNative.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);

//	JSTEval("Object.defineProperty(this, 'api', {value:undefined,writeable:true,configurable:true}); delete JSNative.Class.$registry, api", global);
	
}

