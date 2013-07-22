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
	JSObjectRef interface = JSTGetPrivate(object);
	JSValueRef conversion;
	JSObjectRef convert = JSTCoreGetPropertyObject(interface, RtCSConvert);
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
	if (JSTCoreHasProperty(interface, RtCSSetProperty))
	return JSTBoolean(JSTCoreCallProperty(interface, RtCSSetProperty, object, JSTMakeString(property, NULL, false), value));
	return false;
}

static JSValueRef jsGetProperty JSTNativePropertyReader() {
	if (JSStringIsEqual(property, RtCSConstruct)) return (JSValueRef) JSTGetPrivate(object);
	JSObjectRef interface = JSTGetPrivate(object);
	JSValueRef result;
	if (JSTCoreHasProperty(interface, RtCSGetProperty))
	result = JSTCoreCallProperty(interface, RtCSGetProperty, object, JSTMakeString(property, NULL, false));
	else return NULL;
	if (JSTNull(result)) {
		result = NULL;
	}
	return result;
}

static JSObjectRef jsCreateClass JSTNativeConstructor() {
	char * str;
	JSObjectRef classObject = JSTGetPropertyObject(RtJSNativeClassRegistry, JSTGetValueBuffer(argv[0], &str));
	if (!JSTReference(classObject)) {
		char buffer[256];
		if (JSTHasProperty(RtJSNativeClassRegistry, str)) {
			sprintf(buffer, "createClass(): invalid class name parameter: '%s' is a reserved class name");
			JSTReferenceError(buffer); JSTFreeBuffer(str);
		}
		JSTReturnObjectException;
	}
	JSObjectRef constructor = JSTCoreGetPropertyObject(classObject, RtCSConstruct);
	JSClassRef class = JSTGetPrivate(classObject);
	JSObjectRef this = JSTCreateClassObject(class, (void*) constructor);
	JSTSetPrototype(this, (argc > 1)? argv[1]:JSTCoreGetProperty(constructor, RtCSPrototype));
	JSTFreeBuffer(str);
	return this;
}

static JSValueRef jsRegisterClass JSToolsFunction () {
	char * buffer = NULL;
	JSClassDefinition jsClass = kJSClassDefinitionEmpty;
	jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsClass.getProperty = &jsGetProperty;
	jsClass.setProperty = &jsSetProperty;
	jsClass.convertToType = &jsConvertProperty;
	JSValueRef className = argv[0];
	if (JSTCaughtException) return JSTMakeNull();
	jsClass.className = JSTGetValueBuffer(className, &buffer);
	JSClassRef JSNativeClass = JSClassRetain(JSClassCreate(&jsClass));
	JSTSetProperty(RtJSNativeClassRegistry, buffer, JSTCreateClassObject(JSNativeGhost, JSNativeClass), JSTPropertyConst);
	JSObjectRef result = JSTGetPropertyObject(RtJSNativeClassRegistry, buffer);
	JSTCoreSetProperty(result, RtCSConstruct, argv[1], JSTPropertyRequired);
	JSTFreeBuffer(buffer);
	return result;

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
	jsClass.className = "JSNative.Class";

	JSNativeGhost = JSClassRetain(JSClassCreate(&jsClass));

	
	RtJSNativeAPI = JSTCreateClassObject(NULL,NULL);
	JSTSetProperty(global, "api", RtJSNativeAPI, 0);

	JSTSetPropertyFunction(RtJSNativeAPI, "registerClass", &jsRegisterClass);
	JSTSetProperty(RtJSNativeAPI, "createClass", JSTMakeConstructor(JSNativeGhost, &jsCreateClass), 0);

	JSTEvalScript(JSNativeClass, global, "JSNativeClass.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);

	RtJSNativeClassRegistry = JSTEvalObject("JSNative.Class.$registry", global);
	JSTEval("Object.defineProperty(this, 'api', {value:undefined,writeable:true,configurable:true}); delete JSNative.Class.$registry, api", global);
	JSTEvalScript(JSNativeSupport, global, "JSNative.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);

	
}

