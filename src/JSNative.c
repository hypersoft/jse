/* Brigadier */
#define JSNativeSource

#include "JSNative.inc"
#include "JSNativeClass.inc"
#include "JSNativeType.inc"

#include "JSNative.h"

JSObjectRef RtJSNativeAPI;

static int EnumClassInitialize = 2,
EnumClassConstruct = 4,
EnumClassInvoke = 8,
EnumClassGet = 16,
EnumClassSet = 32,
EnumClassDelete = 64,
EnumClassConvert = 128,
EnumClassInstanceOf = 256,
EnumClassEnumerate = 512,
EnumClassHasProperty = 1024,

EnumTypeVoid = 2,
EnumTypeBoolean = 4,
EnumTypeChar = 8,
EnumTypeShort = 16,
EnumTypeInt = 32,
EnumTypeLong = 64,
EnumTypeLongLong = 128,
EnumTypeFloat = 256,
EnumTypeDouble = 512,
EnumTypePointer = 1024,
EnumTypeFunction = 2048;

static JSValueRef jsNativeGetTypeMax JSToolsFunction(enumType) {
	int enumType = JSTInteger(argv[0]), result = 0;

	if (enumType & EnumTypePointer) {
		result = sizeof(void*); goto makeResult;
	} 

	if (enumType & EnumTypeVoid) goto makeResult;
	else if (enumType & EnumTypeBoolean) {
		result = sizeof(bool); goto makeResult;
	} else if (enumType & EnumTypeChar) {
		result = sizeof(char); goto makeResult;
	} else if (enumType & EnumTypeShort) {
		result = sizeof(short); goto makeResult;
	} else if (enumType & EnumTypeInt) {
		result = sizeof(int); goto makeResult;
	} else if (enumType & EnumTypeLong) {
		result = sizeof(long); goto makeResult;
	} else if (enumType & EnumTypeLongLong) {
		result = sizeof(long long); goto makeResult;
	} else if (enumType & EnumTypeFloat) {
		result = sizeof(float); goto makeResult;
	} else if (enumType & EnumTypeDouble) {
		result = sizeof(double); goto makeResult;
	} else /* that's an error */ JSTTypeError("JSNative.api.getTypeSize: invalid type code");

	return RtJS(undefined);

makeResult:
	return JSTMakeNumber(result);
}

static JSValueRef jsNativeGetTypeSize JSToolsFunction(enumType) {
	int enumType = JSTInteger(argv[0]), result = 0;

	if (enumType & EnumTypePointer) {
		result = sizeof(void*); goto makeResult;
	} 

	if (enumType & EnumTypeVoid) goto makeResult;
	else if (enumType & EnumTypeBoolean) {
		result = sizeof(bool); goto makeResult;
	} else if (enumType & EnumTypeChar) {
		result = sizeof(char); goto makeResult;
	} else if (enumType & EnumTypeShort) {
		result = sizeof(short); goto makeResult;
	} else if (enumType & EnumTypeInt) {
		result = sizeof(int); goto makeResult;
	} else if (enumType & EnumTypeLong) {
		result = sizeof(long); goto makeResult;
	} else if (enumType & EnumTypeLongLong) {
		result = sizeof(long long); goto makeResult;
	} else if (enumType & EnumTypeFloat) {
		result = sizeof(float); goto makeResult;
	} else if (enumType & EnumTypeDouble) {
		result = sizeof(double); goto makeResult;
	} else /* that's an error */ JSTTypeError("JSNative.api.getTypeSize: invalid type code");

	return RtJS(undefined);

makeResult:
	return JSTMakeNumber(result);
}

static JSObjectRef jsNativeGetClassMethod JSToolsProcedure(JSObjectRef object, char * name) {

	// we are going to allow all monkey business ...
	// prototype will not be queried for methods directly

	JSObjectRef method = JSTGetPropertyObject(object, name);

	return method;

}

bool jsNativeClassHasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef property) {

	void * exception = NULL;

	static bool requestInProgress;
	if (requestInProgress) return false;

	bool result = false; requestInProgress = true;

	JSObjectRef classHasProperty = JSToolsCall(jsNativeGetClassMethod, object, "classHasProperty");

	if (JSTFunction(classHasProperty))
		result = JSTBoolean(JSTCall(classHasProperty, object, JSTMakeString(property, NULL, false)));

	requestInProgress = false;

	return result;

}

// The array of names returned by 'enumerate', must be handled by 'get' in order to show up 
// in the resulting list of enumerated property names.
void jsNativeClassEnumerate(JSContextRef ctx, JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames) {
	void * exception = NULL;
	JSObjectRef classEnumerate = JSToolsCall(jsNativeGetClassMethod, object, "classEnumerate");
	if (!JSTFunction(classEnumerate)) return;
	JSObjectRef names = JSTCallObject(classEnumerate, object); // numeric indexed pseudo array with length
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
 	bool result = false;
	requestInProgress = true;

	JSObjectRef classSet = JSToolsCall(jsNativeGetClassMethod, object, "classSet");

	if (JSTFunction(classSet) )
		result = JSTBoolean(JSTCall(classSet, object, JSTMakeString(property, NULL, false), value));

	requestInProgress = false;

	return result;

}

static JSValueRef jsNativeClassGet JSTNativePropertyReader() {

	static bool requestInProgress;
	if (requestInProgress) return NULL;

	JSValueRef result = NULL;

	requestInProgress = true;

	JSObjectRef get = JSToolsCall(jsNativeGetClassMethod, object, "classGet");

	if (JSTFunction(get) )
		result = JSTCall(get, object, JSTMakeString(property, NULL, false));

	requestInProgress = false;
	if (JSTNull(result)) return NULL;

	return result;

}

bool jsNativeClassDelete(JSContextRef ctx, JSObjectRef this, JSStringRef property, JSValueRef* exception) {

	static bool requestInProgress; if (requestInProgress) return false;
	bool result = false;

	requestInProgress = true;
		JSObjectRef classDelete = JSToolsCall(jsNativeGetClassMethod, this, "classDelete");
		if (JSTFunction(classDelete) )
			result = JSTBoolean(JSTCall(classDelete, this, JSTMakeString(property, NULL, false)));
	requestInProgress = false;

	return result;
}

bool jsNativeClassInstanceOf(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception) {
	JSObjectRef classInstanceOf = (JSObjectRef) JSTGetPropertyObject(constructor, "classInstanceOf");
	if ( ! JSTFunction(classInstanceOf) ) return false;
	return JSTBoolean(JSTCall(classInstanceOf, constructor, possibleInstance));
}

static JSValueRef jsNativeClassConvert JSTNativeConvertor() {
	JSValueRef conversion;

	JSObjectRef convert = JSToolsCall(jsNativeGetClassMethod, object, "classConvert");

	if (convert && JSTFunction(convert)) {
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
	JSObjectRef classInvoke = (JSObjectRef) JSTGetPropertyObject(function, "classInvoke");
	if (! JSTFunction(classInvoke) ) return classInvoke;
	return JSTRelayFunctionCall(classInvoke);
}

JSObjectRef jsNativeClassConstruct(JSContextRef ctx, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {

	JSObjectRef classConstruct = JSTGetPropertyObject(constructor, "classConstruct");
	if (! JSTFunction(classConstruct) ) return NULL;

	JSObjectRef prototype = JSTGetPropertyObject(constructor, "prototype");
	if (! JSTFunction(classConstruct) ) return NULL;

	JSClassRef instanceClass = JSTGetPrivate(prototype);
	JSObjectRef this = JSTCreateClassObject(instanceClass, constructor);

	if (JSTInteger(JSTGetProperty(prototype, "__classFlags__")) & EnumClassInitialize) {
		JSObjectRef classInitialize = JSTGetPropertyObject(prototype, "classInitialize");
		if (JSTReference(classInitialize) ) JSTCall(classInitialize, this);
	}

	if (JSTInteger(JSTGetProperty(constructor, "__classFlags__")) & EnumClassInitialize) {
		JSObjectRef classInitialize = JSTGetPropertyObject(constructor, "classInitialize");
		if (JSTReference(classInitialize) ) JSTCall(classInitialize, this);
		else JSTSetPrototype(this, prototype);
	} else JSTSetPrototype(this, prototype);

	JSObjectRef detour = (JSObjectRef)JSTRelayFunctionCall(classConstruct);
	if (JSTReference(detour)) return detour;

	return this;

}

static JSValueRef jsNativeClassCreate JSToolsFunction () {

	char *className = JSTGetValueBuffer(argv[0], NULL);
	int flags = JSTInteger(argv[1]);
	
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
	if (flags & EnumClassHasProperty) jsClass.hasProperty = &jsNativeClassHasProperty;
	if (flags & EnumClassInstanceOf) jsClass.hasInstance = &jsNativeClassInstanceOf;
	JSClassRef JSNativeClass = JSClassRetain(JSClassCreate(&jsClass));

	JSObjectRef thisClass = JSTCreateClassObject(JSNativeClass, JSNativeClass);

	JSTSetProperty(thisClass, "name", argv[0], JSTPropertyHidden);
	JSTSetProperty(thisClass, "__classFlags__", argv[1], JSTPropertyProtected);

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
	
	RtJSNativeAPI = (JSObjectRef) JSTCreateClassObject(NULL,NULL);

	JSTSetPropertyFunction(RtJSNativeAPI, "createClass", &jsNativeClassCreate);
	JSTSetPropertyFunction(RtJSNativeAPI, "setObjectPrototype", &jsNativeApiSetPrototype);
	JSTSetPropertyFunction(RtJSNativeAPI, "getTypeSize", &jsNativeGetTypeSize);

	/* Native Address Alignment */
	JSTSetProperty(RtJSNativeAPI, "addressAlignment", JSTMakeNumber(G_MEM_ALIGN), JSTPropertyRequired);

	JSTSetProperty(global, "api", RtJSNativeAPI, 0);

	JSTEvalScript(JSNativeInit, global, "JSNative.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);

	JSTEvalScript(JSNativeClassInit, global, "JSNativeClass.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);

	JSTEvalScript(JSNativeTypeInit, global, "JSNativeType.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);
	
}

