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

void jsNativeClassEnumerate(JSContextRef ctx, JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames) {
	void * exception = NULL;
	JSObjectRef constructor = JSTGetPropertyObject(object, "constructor");
	JSObjectRef enumerate = (JSObjectRef) JSTGetPropertyObject(constructor, "enumerate");
	JSObjectRef names = JSTCallObject(enumerate, object); // JSValArray
	long length = JSTInteger(JSTGetProperty(names, "length"));
	JSStringRef name = NULL;
	long i; for (i = 0; i < length; i++) { JSPropertyNameAccumulatorAddName(
		propertyNames, JSTGetValueString(JSTGetIndex(names, i), &name)
	);
		JSTFreeString(name);
	};
}

static bool jsNativeClassSet JSTNativePropertyWriter() {

	static bool requestInProgress;
	if (requestInProgress) return false;
 
	requestInProgress = true;
		JSObjectRef constructor = JSTGetPropertyObject(object, "constructor");

		JSObjectRef set = (JSObjectRef) JSTGetPropertyObject(constructor, "set");
	requestInProgress = false;

	if (! JSTReference(set) ) return false;

	requestInProgress = true;
		bool result = JSTBoolean(JSTCall(set, object, JSTMakeString(property, NULL, false), value));
	requestInProgress = false;

	return result;

}

static JSValueRef jsNativeClassGet JSTNativePropertyReader() {

	static bool requestInProgress;
	if (requestInProgress) return NULL;

	if (JSTCoreEqualsNative(property, "constructor")) return NULL;

	requestInProgress = true;
		JSObjectRef constructor = JSTGetPropertyObject(object, "constructor");	

		JSObjectRef get = (JSObjectRef) JSTGetPropertyObject(constructor, "get");
	requestInProgress = false;

	if (! JSTReference(get) ) return NULL;

	requestInProgress = true;
		JSValueRef result = JSTCall(get, object, JSTMakeString(property, NULL, false));
	requestInProgress = false;

	if (JSTNull(result)) return NULL;

	return result;

}

bool jsNativeClassDelete(JSContextRef ctx, JSObjectRef this, JSStringRef property, JSValueRef* exception) {

	JSObjectRef constructor = JSTGetPropertyObject(this, "constructor");	
	JSObjectRef delete = (JSObjectRef) JSTGetPropertyObject(constructor, "delete");
	if (! JSTReference(delete) ) return false;
	return JSTBoolean(JSTCall(delete, this, JSTMakeString(property, NULL, false)));

}

bool jsNativeClassInstanceOf(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception) {
	JSObjectRef instanceof = (JSObjectRef) JSTGetPropertyObject(constructor, "instanceof");
	if ( ! JSTReference(instanceof) ) return false;
	return JSTBoolean(JSTCall(instanceof, constructor, possibleInstance));
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
	JSObjectRef constructor = JSTGetPropertyObject(function, "constructor");
	JSObjectRef classFunction = JSTGetPropertyObject(constructor, "invoke");
	if (! JSTReference(classFunction) ) return NULL;
	return JSTRelayFunctionCall(classFunction);
}

JSObjectRef jsNativeClassConstruct(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

	JSClassRef jsClassRef = JSTGetPrivate(constructor);
	JSObjectRef this = JSTCreateClassObject(jsClassRef, jsClassRef);
	JSObjectRef jsConstructor = JSTGetPropertyObject(constructor, "construct");
	int flags = JSTInteger(JSTGetProperty(constructor, "flags"));
	if (flags & EnumClassInitialize) {
		JSObjectRef initialize = JSTGetPropertyObject(constructor, "initialize");
		JSTCall(initialize, this);
	} else {
		JSTSetPrototype(this, JSTGetProperty(constructor, "prototype"));
		JSTSetProperty(this, "constructor", constructor, JSTPropertyProtected);
	}
	if (! JSTReference(jsConstructor) ) return this;
	JSValueRef deviation = JSObjectCallAsFunction(ctx, jsConstructor, this, argumentCount, arguments, exception);
	if (!JSTUndefined(deviation)) return (JSObjectRef)deviation;
	return this;

}

static JSValueRef jsNativeClassCreate JSToolsFunction () {

	JSClassDefinition jsClass = kJSClassDefinitionEmpty;

	JSObjectRef classModel = (JSObjectRef)argv[0];
	JSValueRef className = JSTGetProperty(classModel, "name");
	int flags = JSTInteger(JSTGetProperty(classModel, "flags"));
	char * nativeClassName = JSTGetValueBuffer(className, NULL);
	JSObjectRef parent = JSTGetPropertyObject(classModel, "parent");
	JSClassRef parentClass = NULL;

	if (JSTReference(parent)) {
		if (JSTConstructor(parent)) parentClass = JSTGetPrivate(parent);
		else { JSTTypeError("class model 'parent' is not a constructor");
			JSTReturnValueException;
		}
		if (! parentClass ) {
			JSTTypeError("class model 'parent' is not a class constructor");
			JSTReturnValueException;
		}
	}

	jsClass.className = nativeClassName;

	if (flags & EnumClassConstruct) jsClass.callAsConstructor = &jsNativeClassConstruct;
	if (flags & EnumClassInvoke) jsClass.callAsFunction = &jsNativeClassInvoke;
	if (flags & EnumClassGet) jsClass.getProperty = &jsNativeClassGet;
	if (flags & EnumClassSet) jsClass.setProperty = &jsNativeClassSet;
	if (flags & EnumClassDelete) jsClass.deleteProperty = &jsNativeClassDelete;
	if (flags & EnumClassConvert) jsClass.convertToType = &jsNativeClassConvert;
	if (flags & EnumClassInstanceOf) jsClass.hasInstance = &jsNativeClassInstanceOf;
	if (flags & EnumClassEnumerate) jsClass.getPropertyNames = &jsNativeClassEnumerate;

	jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsClass.parentClass = (parentClass)?parentClass:JSNativeClass;

	JSClassRef jsClassRef = JSClassRetain(JSClassCreate(&jsClass));
	JSObjectRef thisClass = JSTCreateClassObject(JSNativeClass, jsClassRef);
	JSTSetPrototype(thisClass, JSTEval("JSNative.Class.prototype", NULL));

	JSTFreeBuffer(nativeClassName);

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

	jsClass.callAsConstructor = &jsNativeClassConstruct;
	jsClass.callAsFunction = &jsNativeClassInvoke;
	jsClass.hasInstance = &jsNativeClassInstanceOf;
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

