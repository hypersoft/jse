/* Brigadier */
#define JSNativeSource

#include "JSNative.inc"
#include "JSNativeClass.inc"

#include "JSNative.h"

JSObjectRef RtJSNativeAPI;
JSObjectRef RtJSNativeClassRegistry;

JSClassRef JSNativeClass;

static int EnumClassInitialize = 2,
EnumClassConstruct = 4,
EnumClassInvoke = 8,
EnumClassGet = 16,
EnumClassSet = 32,
EnumClassDelete = 64,
EnumClassConvert = 128,
EnumClassInstanceOf = 256,
EnumClassEnumerate = 512;

// The array of names returned by 'enumerate', must be handled by 'get' in order to show up 
// in the resulting list of enumerated property names.
void jsNativeClassEnumerate(JSContextRef ctx, JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames) {
	void * exception = NULL;
	JSObjectRef classEnumerate = (JSObjectRef) JSTGetPropertyObject(object, "classEnumerate");
	JSObjectRef names = JSTCallObject(classEnumerate, object); // JSValArray
	register long length = JSTInteger(JSTGetProperty(names, "length"));
	JSStringRef name = NULL;
	register long i; for (i = 0; i < length; i++) { JSPropertyNameAccumulatorAddName(
		propertyNames, JSTGetValueString(JSTGetIndex(names, i), &name)
	);
		JSTFreeString(name);
	};
}

static bool jsNativeClassSet JSTNativePropertyWriter() {

	static bool requestInProgress;
	if (requestInProgress) return false;
 
	requestInProgress = true;
		JSObjectRef classSet = (JSObjectRef) JSTGetPropertyObject(object, "classSet");
	requestInProgress = false;

	if (! JSTReference(classSet) ) return false;

	requestInProgress = true;
		bool result = JSTBoolean(JSTCall(classSet, object, JSTMakeString(property, NULL, false), value));
	requestInProgress = false;

	return result;

}

static JSValueRef jsNativeClassGet JSTNativePropertyReader() {

	static bool requestInProgress;
	if (requestInProgress) return NULL;

	requestInProgress = true;
		JSObjectRef get = (JSObjectRef) JSTGetPropertyObject(object, "classGet");
	requestInProgress = false;

	if (! JSTReference(get) ) return NULL;

	requestInProgress = true;
		JSValueRef result = JSTCall(get, object, JSTMakeString(property, NULL, false));
	requestInProgress = false;

	if (JSTNull(result)) return NULL;

	return result;

}

bool jsNativeClassDelete(JSContextRef ctx, JSObjectRef this, JSStringRef property, JSValueRef* exception) {
	JSObjectRef classDelete = JSTGetPropertyObject(this, "classDelete");	
	if (! JSTReference(classDelete) ) return false;
	return JSTBoolean(JSTCall(classDelete, this, JSTMakeString(property, NULL, false)));
}

bool jsNativeClassInstanceOf(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception) {
	JSObjectRef classInstanceOf = (JSObjectRef) JSTGetPropertyObject(constructor, "classInstanceOf");
	if ( ! JSTReference(classInstanceOf) ) return false;
	return JSTBoolean(JSTCall(classInstanceOf, constructor, possibleInstance));
}

static JSValueRef jsNativeClassConvert JSTNativeConvertor() {
	JSValueRef conversion;
	JSObjectRef constructor = JSTGetPropertyObject(object, "constructor");	
	JSObjectRef convert = (JSObjectRef) JSTGetPropertyObject(constructor, "convert");
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
		if (JSTBoolean(JSTEval("this === JSNative.api.failedToConvert", conversion))) return false;
		return conversion;
	}
	return false;
}

JSValueRef jsNativeClassInvoke(JSContextRef ctx, JSObjectRef function, JSObjectRef this, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
	JSObjectRef classInvoke = JSTGetPropertyObject(function, "classInvoke");
	if (! JSTReference(classInvoke) ) return NULL;
	return JSTRelayFunctionCall(classInvoke);
}

JSObjectRef jsNativeClassConstruct(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

	JSObjectRef classConstruct = JSTGetPropertyObject(constructor, "classConstruct");
	if (! JSTReference(classConstruct) ) return NULL;

	JSObjectRef classInstance = JSTGetPropertyObject(constructor, "classInstance");
	JSClassRef thisInstance = JSTGetPrivate(classInstance);
	JSObjectRef this = JSTCreateClassObject(thisInstance, constructor);

	if (JSTInteger(JSTGetProperty(constructor, "classFlags")) & EnumClassInitialize) {
		JSObjectRef classInitialize = JSTGetPropertyObject(constructor, "classInitialize");
		if (! JSTReference(classInitialize) ) return NULL;
		JSTCall(classInitialize, this);
	} else JSTSetPrototype(this, JSTGetProperty(constructor, "prototype"));

	JSObjectRef detour = JSTCallObject(classConstruct, this);
	if (JSTReference(detour)) return detour;
	return this;

}

static JSValueRef jsNativeClassCreate JSToolsFunction () {

	JSObjectRef descriptor = (JSObjectRef) argv[0];
	int flags = JSTInteger(JSTGetProperty(descriptor, "classFlags"));
	char *className = JSTGetValueBuffer(JSTGetProperty(descriptor, "name"), NULL);
	
	JSClassDefinition jsClass = kJSClassDefinitionEmpty;
	jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsClass.className = className;

	if (flags & EnumClassConstruct) jsClass.callAsConstructor = &jsNativeClassConstruct;
	if (flags & EnumClassInvoke) jsClass.callAsFunction = &jsNativeClassInvoke;
	if (flags & EnumClassGet) jsClass.getProperty = &jsNativeClassGet;
	if (flags & EnumClassSet) jsClass.setProperty = &jsNativeClassSet;
	if (flags & EnumClassDelete) jsClass.deleteProperty = &jsNativeClassDelete;
	if (flags & EnumClassConvert) jsClass.convertToType = &jsNativeClassConvert;
	if (flags & EnumClassInstanceOf) jsClass.hasInstance = &jsNativeClassInstanceOf;
	if (flags & EnumClassEnumerate) jsClass.getPropertyNames = &jsNativeClassEnumerate;
	JSClassRef JSNativeClass = JSClassRetain(JSClassCreate(&jsClass));

	JSObjectRef thisClass = JSTCreateClassObject(JSNativeClass, JSNativeClass);
	JSTSetProperty(thisClass, "name", JSTGetProperty(descriptor, "name"), 0);
	JSTSetProperty(thisClass, "classFlags", JSTGetProperty(descriptor, "classFlags"), 0);
	JSTSetProperty(thisClass, "classInstance", JSTGetProperty(descriptor, "classInstance"), 0);
	JSTSetProperty(thisClass, "prototype", JSTGetProperty(descriptor, "prototype"), 0);

	JSTFreeBuffer(className);

	return (JSValueRef) thisClass;

}

static JSValueRef jsNativeApiSetPrototype JSToolsFunction (o, p) {
	JSTSetPrototype((JSObjectRef)argv[0], argv[1]);
	return RtJS(undefined);
}

void js_native_init JSToolsProcedure (int argc, char *argv[], char *envp[]) {

	JSObjectRef global = JSTGetGlobalObject();

	JSTLoadRuntime(global, argc, argv, envp);
	
	JSClassDefinition jsClass = kJSClassDefinitionEmpty;
	jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsClass.className = "JSNative.Class";

	JSNativeClass = JSClassRetain(JSClassCreate(&jsClass));

	RtJSNativeAPI = (JSObjectRef) JSTCreateClassObject(NULL,NULL);

	JSTSetPropertyFunction(RtJSNativeAPI, "createClass", &jsNativeClassCreate);
	JSTSetPropertyFunction(RtJSNativeAPI, "setObjectPrototype", &jsNativeApiSetPrototype);

	JSTSetProperty(global, "api", RtJSNativeAPI, 0);

	JSTEvalScript(JSNativeInit, global, "JSNative.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);

	JSTEvalScript(JSNativeClassInit, global, "JSNativeClass.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);
	
}

