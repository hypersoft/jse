/* Brigadier */
#define JSNativeSource

#include "JSNative.inc"
#include "JSNativeAPI.inc"
#include "JSNative.h"

JSObjectRef RtJSNativeApi;
JSClassRef JSNativeGhost;

static JSStringRef RtJSPrototype, RtJSSetProperty, RtJSGetProperty, RtJSConstruct, RtJSClassRegister;

static JSValueRef jsConvertProperty JSTNativeConvertor() {
	JSObjectRef interface = JSTGetPrivate(object);
	JSValueRef conversion;
	JSObjectRef convert = JSTGetPropertyObject(interface, "convert");
	if (convert && JSTReference(convert)) {
		if (kJSTypeString == type) {
			conversion = JSTCall(convert, interface, object, RtJS(String));
		} else
		if (kJSTypeNumber == type) {
			conversion = JSTCall(convert, interface, object, RtJS(Number));
		} else
		if (kJSTypeBoolean == type) {
			conversion = JSTCall(convert, interface, object, RtJS(Boolean));
		} else
		if (kJSTypeObject == type) {
			conversion = JSTCall(convert, interface, object, RtJS(Object));
		} else
		if (kJSTypeNull == type) {
			conversion = JSTCall(convert, interface, object, JSTMakeNull());
		} else
		if (kJSTypeUndefined == type) {
			conversion = JSTCall(convert, interface, object, RtJS(undefined));
		}
		if (JSTBoolean(JSTEval("this === JSNative.api.conversionFailure", conversion))) return false;
		return conversion;
	}
	return false;
}

static bool jsSetProperty JSTNativePropertyWriter() {
	JSObjectRef interface = JSTGetPrivate(object);
	return JSTBoolean(JSTCallProperty(interface, "setProperty", object, JSTMakeString(property, NULL, false), value));
}

static JSValueRef jsGetProperty JSTNativePropertyReader() {
	if (JSTCoreEqualsNative(property, "constructor")) return (JSValueRef) JSTGetPrivate(object);
	JSObjectRef interface = JSTGetPrivate(object);
	JSValueRef result = JSTCallProperty(interface, "getProperty", object, JSTMakeString(property, NULL, false));
	if (JSTNull(result)) {
		result = NULL;
	}
	return result;
}

static JSObjectRef jsCreateClass JSTNativeConstructor() {
	JSObjectRef classRegister = JSTCoreGetPropertyObject(RtJSNativeApi, RtJSClassRegister);
	JSObjectRef classObject = JSTCallObject(classRegister, RtJSNativeApi, argv[0]);
	JSObjectRef constructor = JSTCoreGetPropertyObject(classObject, RtJSConstruct);
	JSClassRef class = JSTGetPrivate(classObject);
	JSObjectRef this = JSTCreateClassObject(class, (void*) constructor);
	JSTSetPrototype(this, (argc > 1)? argv[1]:JSTCoreGetProperty(constructor, RtJSPrototype));
	return this;
}

static JSValueRef jsRegisterClass JSToolsFunction () {

	JSObjectRef classRegister = JSTCoreGetPropertyObject(this, RtJSClassRegister);
	char * buffer = NULL;
	JSClassDefinition jsClass = kJSClassDefinitionEmpty;
	jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsClass.getProperty = &jsGetProperty;
	jsClass.setProperty = &jsSetProperty;
	jsClass.convertToType = &jsConvertProperty;
	JSValueRef className = JSTCall(classRegister, this, argv[0]);
	if (JSTCaughtException) return JSTMakeNull();
	jsClass.className = JSTGetValueBuffer(className, &buffer);
	JSClassRef JSNativeClass = JSClassRetain(JSClassCreate(&jsClass));
	JSTSetProperty(classRegister, buffer, JSTCreateClassObject(JSNativeGhost, JSNativeClass), JSTPropertyConst);
	JSObjectRef result = JSTGetPropertyObject(classRegister, buffer);
	JSTCoreSetProperty(result, RtJSConstruct, argv[1], JSTPropertyRequired);
	JSTFreeBuffer(buffer);
	return result;

}

void js_native_init JSToolsProcedure (int argc, char *argv[], char *envp[]) {

	JSObjectRef global = JSTGetGlobalObject();

	JSTLoadRuntime(global, argc, argv, envp);

	RtJSPrototype = JSTCreateStaticString("prototype", NULL);
	RtJSSetProperty = JSTCreateStaticString("setProperty", NULL);
	RtJSGetProperty = JSTCreateStaticString("getProperty", NULL);
	RtJSConstruct = JSTCreateStaticString("constructor", NULL);
	RtJSClassRegister = JSTCreateStaticString("classRegister", NULL);

	JSClassDefinition jsClass = kJSClassDefinitionEmpty;
	jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsClass.className = "JSNative.Class";

	JSNativeGhost = JSClassRetain(JSClassCreate(&jsClass));

	JSTEvalScript(JSNativeAPISupport, global, "JSNativeAPI.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);

	JSObjectRef RtJSNativeClass = JSTGetPropertyObject(global, "NativeClass");

	RtJSNativeApi = JSTGetPropertyObject(global, "api");

	JSTSetPropertyFunction(RtJSNativeApi, "registerClass", &jsRegisterClass);
	JSTSetProperty(RtJSNativeApi, "createClass", JSTMakeConstructor(JSNativeGhost, &jsCreateClass), 0);

	JSTEvalScript(JSNativeSupport, global, "JSNative.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);

}

