/* Brigadier */
#define JSNativeSource

#include "JSNative.inc"
#include "JSNativeClass.inc"
#include "JSNativeType.inc"

#include "JSNative.h"

#include "dyncall.inc"

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

EnumTypeConst = 2,
EnumTypeUnsigned = 4,
EnumTypeVoid = 8,
EnumTypeBoolean = 16,
EnumTypeChar = 32 | 4,
EnumTypeShort = 64,
EnumTypeInt = 128,
EnumTypeLong = 256,
EnumTypeLongLong = 512,
EnumTypeFloat = 1024,
EnumTypeDouble = 2048,
EnumTypeFunction = 4096,
EnumTypeStruct = 8192,
EnumTypeUnion = 16384,
EnumTypeEnum = 32768;


static JSValueRef jsNativeCallVM JSToolsFunction(DCsize size) {
	if (argc != 1) {
		JSTSyntaxError("JSNative.api.callVM: expected 1 integer argument: size");
		return RtJS(undefined);
	}
	return JSTMakeNumber((long)dcNewCallVM(JSTLong(JSTParam(1))));
}

static JSValueRef jsNativeFreeCallVM JSToolsFunction(DCCallVM * vm) {
	if (argc != 1) {
		JSTSyntaxError("JSNative.api.freeCallVM: expected 1 integer argument: (JSNative.CallVM) pointer");
		return RtJS(undefined);
	} 
	dcFree(JSTPointer(JSTParam(1))); return RtJS(undefined);
}

static JSValueRef jsNativeCallVMError JSToolsFunction(DCCallVM * vm) {
	if (argc != 1) {
		JSTSyntaxError("JSNative.api.callVMError: expected 1 integer argument: (JSNative.CallVM) pointer");
		return RtJS(undefined);
	}
	return JSTMakeNumber(dcGetError(JSTPointer(JSTParam(1))));
}

static JSValueRef jsNativeCallVMSetMode JSToolsFunction(DCCallVM * vm, DCint mode) {
	if (argc != 2) {
		JSTSyntaxError("JSNative.api.callVMSetMode: expected 2 integer arguments: (JSNative.CallVM) pointer, JSNativeCallVMPrototype) mode");
		return RtJS(undefined);
	}
	dcMode(JSTPointer(JSTParam(1)), JSTInteger(JSTParam(2))); return RtJS(undefined);
}

static JSValueRef jsNativeCallVMReset JSToolsFunction(DCCallVM * vm) {
	if (argc != 1) {
		JSTSyntaxError("JSNative.api.callVMReset: expected 1 integer argument: (JSNative.CallVM) pointer");
		return RtJS(undefined);
	}
	dcReset(JSTPointer(JSTParam(1))); return RtJS(undefined);
}

static JSValueRef jsNativeCallVMPush JSToolsFunction(DCCallVM * vm, type, value) {

	if (argc != 3) {
		JSTSyntaxError("JSNative.api.callVMPush: expected 3 arguments: (JSNative.CallVM) pointer, (Number) type, (Number) value");
		return RtJS(undefined);
	}

	void * vm = JSTPointer(argv[0]);
	int type = JSTInteger(argv[1]);
	JSValueRef value = argv[2];

	if (type & EnumTypeBoolean) dcArgBool(vm, JSTBoolean(value));
	else if (type & EnumTypeChar) dcArgChar(vm, JSTChar(value));
	else if (type & EnumTypeShort) dcArgShort(vm, JSTShort(value));
	else if (type & EnumTypeInt) dcArgInt(vm, JSTInteger(value));
	else if (type & EnumTypeLong) dcArgLong(vm, JSTLong(value));
	else if (type & EnumTypeLongLong) dcArgLongLong(vm, JSTLongLong(value));
	else if (type & EnumTypeFloat) dcArgFloat(vm, JSTFloat(value));
	else if (type & EnumTypeDouble) dcArgDouble(vm, JSTDouble(value));
	else JSTTypeError("JSNative.api.callVMPush: invalid argument type");
	return RtJS(undefined);

}

static JSValueRef jsNativeCallVMCall JSToolsFunction(DCCallVM * vm, type, symbol) {

	if (argc != 3) {
		JSTSyntaxError("JSNative.api.callVMCall: expected 3 arguments: (JSNative.CallVM) pointer, (Number) type, (void pointer) function");
		return RtJS(undefined);
	}

	void * vm = JSTPointer(argv[0]);
	int type = JSTInteger(argv[1]);
	void * symbol = JSTPointer(argv[2]);

	if (type & EnumTypeBoolean) return JSTMakeBoolean(dcCallBool(vm, symbol));
	else if (type & EnumTypeChar) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?dcCallChar(vm, symbol):(signed char)dcCallChar(vm, symbol));
	else if (type & EnumTypeShort) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?(unsigned short)dcCallShort(vm, symbol):dcCallShort(vm, symbol));
	else if (type & EnumTypeInt) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?(unsigned int)dcCallInt(vm, symbol):dcCallInt(vm, symbol));
	else if (type & EnumTypeLong) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?(unsigned long)dcCallLong(vm, symbol):dcCallLong(vm, symbol));
	else if (type & EnumTypeLongLong) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?(unsigned long long)dcCallLongLong(vm, symbol):dcCallLongLong(vm, symbol));
	else if (type & EnumTypeFloat) return JSTMakeNumber(dcCallFloat(vm, symbol));
	else if (type & EnumTypeDouble) return JSTMakeNumber(dcCallDouble(vm, symbol));
	else if (type & EnumTypeVoid) dcCallVoid(vm, symbol);
	else JSTTypeError("JSNative.api.callVMCall: invalid call type");
	return RtJS(undefined);

}

static JSValueRef jsNativeMalloc JSToolsFunction (size) {
	return JSTMakeNumber((long)malloc(JSTLong(argv[0])));
}

static JSValueRef jsNativeFree JSToolsFunction (address) {
	free(JSTPointer(argv[0]));
	return RtJS(undefined);
}

static JSValueRef jsNativeCalloc JSToolsFunction (units, unit_size) {
	return JSTMakeNumber((long)calloc(JSTLong(argv[0]), JSTLong(argv[1])));
}

static JSValueRef jsNativeRealloc JSToolsFunction (address, count) {
	return JSTMakeNumber((long)realloc(JSTPointer(argv[0]), JSTLong(argv[1])));
}

static JSValueRef jsNativeMemset JSToolsFunction (address, value, count) {
	memset(JSTPointer(argv[0]), JSTInteger(argv[1]), JSTLong(argv[2]));
	return RtJS(undefined);
}

static JSValueRef jsNativeReadAddress JSToolsFunction (address, type) {

	void * address = JSTPointer(argv[0]);
	int type = JSTInteger(argv[1]);

	if (type & EnumTypeBoolean) return JSTMakeNumber((double) *(bool*)(address));
	else if (type & EnumTypeChar) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?*(char*)(address):*(signed char*)(address));
	else if (type & EnumTypeShort) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?*(unsigned short*)(address):*(short*)(address));
	else if (type & EnumTypeInt) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?*(unsigned int*)(address):*(int*)(address));
	else if (type & EnumTypeLong) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?*(unsigned long*)(address):*(long*)(address));
	else if (type & EnumTypeLongLong) return JSTMakeNumber((double) (type & EnumTypeUnsigned)?*(unsigned long long*)(address):*(long long*)(address));
	else if (type & EnumTypeFloat) return JSTMakeNumber((double) *(float*)(address));
	else if (type & EnumTypeDouble) return JSTMakeNumber((double) *(double*)(address));
	else { JSTTypeError("JSNative.Type: invalid type"); return RtJS(undefined); }
	return NULL;

}

static JSValueRef jsNativeWriteAddress JSToolsFunction (address, type, value) {

	void * address = JSTPointer(argv[0]);
	int type = JSTInteger(argv[1]);
	JSValueRef value = argv[2];

	if (type & EnumTypeBoolean) { *(bool*)(address) = JSValueToBoolean(ctx, value); return JSTMakeBoolean(true); }
	else if (type & EnumTypeChar) { *(char*)(address) = (char) JSValueToNumber(ctx, value, exception); return JSTMakeBoolean(true); }
	else if (type & EnumTypeShort) { *(short*)(address) = (short) JSValueToNumber(ctx, value, exception); return JSTMakeBoolean(true); }
	else if (type & EnumTypeInt) { *(int*)(address) = (int) JSValueToNumber(ctx, value, exception); return JSTMakeBoolean(true); }
	else if (type & EnumTypeLong) { *(long*)(address) = (long) JSValueToNumber(ctx, value, exception); return JSTMakeBoolean(true); } 
	else if (type & EnumTypeLongLong) { *(long long*)(address) = (long long) JSValueToNumber(ctx, value, exception); return JSTMakeBoolean(true); } 
	else if (type & EnumTypeFloat) { *(float*)(address) = (float) JSValueToNumber(ctx, value, exception); return JSTMakeBoolean(true); } 
	else if (type & EnumTypeDouble) { *(double*)(address) = JSValueToNumber(ctx, value, exception); return JSTMakeBoolean(true); } 
	else { JSTTypeError("JSNative.Type: invalid type"); return JSTMakeBoolean(false); }
	return JSTMakeBoolean(false);

}

static JSValueRef jsNativeGetTypeSize JSToolsFunction(enumType) {
	int enumType = JSTInteger(argv[0]), result = 0;

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

	JSTSetPropertyFunction(RtJSNativeAPI, "getTypeSize", &jsNativeGetTypeSize);

	JSTSetPropertyFunction(RtJSNativeAPI, "callVM", &jsNativeCallVM);
	JSTSetPropertyFunction(RtJSNativeAPI, "freeCallVM", &jsNativeFreeCallVM);
	JSTSetPropertyFunction(RtJSNativeAPI, "callVMError", &jsNativeCallVMError);
	JSTSetPropertyFunction(RtJSNativeAPI, "callVMSetMode", &jsNativeCallVMSetMode);
	JSTSetPropertyFunction(RtJSNativeAPI, "callVMReset", &jsNativeCallVMReset);
	JSTSetPropertyFunction(RtJSNativeAPI, "callVMPush", &jsNativeCallVMPush);
	JSTSetPropertyFunction(RtJSNativeAPI, "callVMCall", &jsNativeCallVMCall);

	JSTSetPropertyFunction(RtJSNativeAPI, "malloc", &jsNativeMalloc);
	JSTSetPropertyFunction(RtJSNativeAPI, "free", &jsNativeFree);
	JSTSetPropertyFunction(RtJSNativeAPI, "calloc", &jsNativeCalloc);
	JSTSetPropertyFunction(RtJSNativeAPI, "realloc", &jsNativeRealloc);

	JSTSetPropertyFunction(RtJSNativeAPI, "memset", &jsNativeMemset);
	JSTSetPropertyFunction(RtJSNativeAPI, "writeAddress", &jsNativeWriteAddress);
	JSTSetPropertyFunction(RtJSNativeAPI, "readAddress", &jsNativeReadAddress);

	JSTSetPropertyFunction(RtJSNativeAPI, "createClass", &jsNativeClassCreate);
	JSTSetPropertyFunction(RtJSNativeAPI, "setObjectPrototype", &jsNativeApiSetPrototype);

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

