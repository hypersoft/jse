#define JSNativeValueSource

#include "JSNative.h"

size_t strlen(const char *s);

JSClassRef JSNativeValue = NULL;
JSClassRef JSNativeArray = NULL;

JSObjectRef RtJSNativeValue = NULL;
JSObjectRef RtJSNativeArray = NULL;

static JSValueRef js_native_value_read_address(JSContextRef ctx, void * address, JSNativeTypeCode type, JSValueRef * exception) {

	if (JSNativeTypeCodeIsUnsigned(type)) { type--;
		if (type == JSNativeTypeChar) return JSTMakeNumber((double) (unsigned char) *(unsigned char*)(address));
		else if (type == JSNativeTypeShort) return JSTMakeNumber((double) *(unsigned short*)(address));
		else if (type == JSNativeTypeInt) return JSTMakeNumber((double) *(unsigned int*)(address));
		else if (type == JSNativeTypeLong) return JSTMakeNumber((double) *(unsigned long*)(address));
		else if (type == JSNativeTypeLongLong) return JSTMakeNumber((double) *(unsigned long long*)(address));
		else { JSTTypeError("JSNative.Type: invalid type"); return RtJS(undefined); }
	} else {
		if (type == JSNativeTypeBool) return JSTMakeNumber((double) *(bool*)(address));
		else if (type == JSNativeTypeChar) return JSTMakeNumber((double) *(char*)(address));
		else if (type == JSNativeTypeShort) return JSTMakeNumber((double) *(short*)(address));
		else if (type == JSNativeTypeInt) return JSTMakeNumber((double) *(int*)(address));
		else if (type == JSNativeTypeLong) return JSTMakeNumber((double) *(long*)(address));
		else if (type == JSNativeTypeLongLong) return JSTMakeNumber((double) *(long long*)(address));
		else if (type == JSNativeTypeFloat) return JSTMakeNumber((double) *(float*)(address));
		else if (type == JSNativeTypeDouble) return JSTMakeNumber((double) *(double*)(address));
		else if (type == JSNativeTypePointer) return JSTMakeNumber((double) *(unsigned long*)(address));
		else { JSTTypeError("JSNative.Type: invalid type"); return RtJS(undefined); }
	}
	return NULL;
}

static bool js_native_value_write_address JSToolsProcedure (void * address, JSNativeTypeCode type, JSValueRef value) {

	if (JSNativeTypeCodeIsUnsigned(type)) { type--;
		if (type == JSNativeTypeChar) { *(unsigned char*)(address) = (unsigned char) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeShort) { *(unsigned short*)(address) = (unsigned short) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeInt) { *(unsigned int*)(address) = (unsigned int) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeLong) { *(unsigned long*)(address) = (unsigned long) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeLongLong) { *(unsigned long long*)(address) = (unsigned long long) JSValueToNumber(ctx, value, exception); return true; }
		else { JSTTypeError("JSNative.Type: invalid type"); return false; }
	} else {
		if (type == JSNativeTypeBool) { *(bool*)(address) = JSValueToBoolean(ctx, value); return true; }
		else if (type == JSNativeTypeChar) { *(char*)(address) = (char) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeShort) { *(short*)(address) = (short) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeInt) { *(int*)(address) = (int) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeLong) { *(long*)(address) = (long) JSValueToNumber(ctx, value, exception); return true; } 
		else if (type == JSNativeTypeLongLong) { *(long long*)(address) = (long long) JSValueToNumber(ctx, value, exception); return true; } 
		else if (type == JSNativeTypeFloat) { *(float*)(address) = (float) JSValueToNumber(ctx, value, exception); return true; } 
		else if (type == JSNativeTypeDouble) { *(double*)(address) = JSValueToNumber(ctx, value, exception); return true; } 
		else if (type == JSNativeTypePointer) { *(unsigned long*)(address) = (unsigned long) JSValueToNumber(ctx, value, exception); return true; }
		else { JSTTypeError("JSNative.Type: invalid type"); return false; }
	}
	return false;
}

static JSObjectRef ConstructArray JSTNativeConstructor () {

	JSObjectRef aborted = RtJSObject(undefined);

	if (argc < 2) {
		JSTReferenceError("new JSNative.Array: expected arguments: JSNative.Type, Array || ...");
		return aborted;
	}

	register JSObjectRef array = JSTCreateClassObject(JSNativeArray, NULL);
	JSValueRef type = JSTEval("JSNative.Type(this)", argv[0]);
	JSTSetProperty(array, "type", type,	JSTPropertyConst);
	int typeCode = JSTInteger(JSTEval("this.type.code", array));
	argc--; argv++;

	JSObjectRef sourceArray = NULL;
	JSStringRef sourceString = NULL;
	long length = 0;

	if (argc == 1) {
		if ((typeCode == 20 || typeCode == 21) && JSTBoolean(JSTEval("classOf(this) == \"String\"", JSTArgumentObject(0)))) {			
			sourceString = JSTGetValueString(argv[0], NULL);
			length = JSStringGetMaximumUTF8CStringSize(sourceString);						
		} else if (JSTHasProperty(JSTArgumentObject(0), "0") && JSTHasProperty(JSTArgumentObject(0), "length")) {
			sourceArray = JSTArgumentObject(0);
			length = JSTInteger(JSTGetProperty(sourceArray, "length"));
		}
		if(--argc) {
			JSTReferenceError("new JSNative.Array: did not expect more than two arguments");
			return aborted;
		}
	}

	if (argc) length = argc;

	JSTSetProperty(array, "count", JSTMakeNumber(length), JSTPropertyConst);
	// No turning back without an allocator...
	JSObjectRef address = JSTEvalObject("JSNative.Allocator.lease(this.type, this.count, true)", array);
	JSTSetProperty(array, "pointer", address, JSTPropertyConst);

	register long i;
	if (sourceString) {
		char * dest = JSTPointer(address);
		JSStringGetUTF8CString(sourceString, dest, length);
		JSTFreeString(sourceString);
		length = strlen(dest);
	} else
	if (sourceArray) for (i = 0; i < length; i++) JSTSetIndex(array, i, JSTGetIndex(sourceArray, i));
	else for (i = 0; i < length; i++) JSTSetIndex(array, i, argv[i]);

	JSTSetProperty(array, "length", JSTMakeNumber(length), JSTPropertyConst);

	return array;

}

static JSObjectRef ConstructValue JSTNativeConstructor () {
	JSObjectRef address, value = JSTCreateClassObject(JSNativeValue, NULL);
	JSTSetProperty(value, "type", JSTEval("JSNative.Type(this)", JSTParam(1)), JSTPropertyConst);
	JSTSetProperty(value, "count", JSTMakeNumber(1), JSTPropertyProtected);
	address = JSTEvalObject("JSNative.Allocator.lease(this.type, this.count, true)", value);
	JSTSetProperty(value, "pointer", address, JSTPropertyConst);
	JSValueRef data = JSTCall(RtJS(Number), NULL, JSTParam(2));
	if (JSTNumber(data)) JSTSetIndex(value, 0, data);
	return value;
}

static JSValueRef GetArrayProperty JSTNativePropertyReader() {

	JSValueRef propertyName = JSTMakeString(property, NULL, false);
	JSValueRef propertyIndex = JSTEval("parseInt(this)", propertyName);
	if (JSTBoolean(JSTCall(RtJSObject(isNaN), NULL, propertyIndex))) return NULL;

	JSObjectRef address = JSTGetPropertyObject(object, "pointer");
	if ( JSTHasProperty(address, "deallocated") ) {
		JSTReferenceError("JSNative.Array: cannot get value: storage pointer is deallocated");
		return NULL;
	}

	int count = JSTInteger(JSTGetProperty(object, "count"));
	int index = JSTInteger(propertyIndex);
	if (index >= count) {
		JSTRangeError("cannot get JSNative.Array value: index out of bounds");
		return RtJS(undefined);
	}
	JSObjectRef value = JSTCreateClassObject(JSNativeValue, NULL);
	JSTSetProperty(value, "type", JSTEval("JSNative.Type(this.type)", object), JSTPropertyConst);
	JSTSetProperty(value, "count", JSTMakeNumber(1), JSTPropertyConst);
	JSTSetProperty(value, "pointer", JSNativeMakeAddress(JSTGetPrivate(address) + index), JSTPropertyConst);
	JSTSetProperty(value, "container", address, JSTPropertyConst);
	return value;
}

static JSValueRef ConvertArray JSTNativeConvertor() {
	JSObjectRef jsAddress = JSTGetPropertyObject(object, "pointer");
	void * address = JSTGetPrivate(jsAddress);
	int typeCode = JSTInteger(JSTEval("this.type.code", object));
	int count = JSTInteger(JSTGetProperty(object, "count"));
	JSValueRef value;

	if (type == JSTNumberType) {
		return JSTCall(RtJS(Number), NULL, jsAddress);
	} else
	if (type == JSTStringType) {
		if (! address || JSTBoolean(JSTEval("this.pointer.deallocated", object))) {
			return JSTCall(RtJS(String), NULL, jsAddress);
		}
		if (typeCode == 20 || typeCode == 21) {
			char * sentinel = ((char*)(address + count));
			char c = *sentinel; *sentinel = 0;
			value = JSTMakeBufferValue(address);
			*sentinel = c;
			return value;
		}
		return JSTCall(RtJS(String), NULL, jsAddress);
	}
	return NULL;

}

static JSValueRef GetValueProperty JSTNativePropertyReader() {

	if (! JSValueIsObjectOfClass(ctx, (JSValueRef) object, JSNativeValue) ) return NULL;

	JSValueRef propertyName = JSTMakeString(property, NULL, false);
	JSValueRef propertyIndex = JSTEval("parseInt(this)", propertyName);

	if (JSTBoolean(JSTCall(RtJSObject(isNaN), NULL, propertyIndex))) return NULL;

	// handle integer requests
	int index = JSTInteger(propertyIndex);
	if (index) {
		JSTReferenceError("JSNative.Value: object is not an array");
		return RtJSObject(undefined);
	}
	return object;

}

static bool SetProperty JSTNativePropertyWriter() {

	int index = 0;
	int count = JSTInteger(JSTGetProperty(object, "count"));
	void * address = JSTGetPrivate(JSTGetPropertyObject(object, "pointer"));
	int typeCode = JSTInteger(JSTEval("this.type.code", object));
	// handle static names...
	bool nativeValue = JSValueIsObjectOfClass(ctx, value, JSNativeValue);
	if (JSTCoreEqualsNative(property, "value") && nativeValue) {
		goto already;
	}

	JSValueRef propertyName = JSTMakeString(property, NULL, false);
	JSValueRef propertyIndex = JSTEval("parseInt(this)", propertyName);
	// handle integer requests
	index = JSTInteger(propertyIndex);

already:
	if (nativeValue && JSTHasProperty(JSTGetPropertyObject(object, "container"), "deallocated")) {
		JSTReferenceError("JSNative.Value: cannot set data: container has been deallocated");
		return false;
	}
	if (! address )  {
		JSTReferenceError("JSNative.Value: cannot set void data");
		return false;
	}

	if (JSTBoolean(JSTCall(RtJSObject(isNaN), NULL, propertyIndex))) return false;

	if ((typeCode == 20 || typeCode == 21) && JSTBoolean(JSTEval("classOf(this) == 'String'", value)) && (JSTInteger(JSTGetProperty((JSObjectRef)value, "length")) == 1)) {
		gunichar jsc = JSTInteger(JSTEval("this.charCodeAt(0)", value));
		char c[6]; g_unichar_to_utf8(jsc, c);
		value = JSTMakeNumber((char)c[0]);
	}
	if (index >= count) {
		JSTRangeError("JSNative.Value: cannot set data: index out of bounds");
		return false;
	}
	js_native_value_write_address(ctx, (address + index), typeCode, value, exception);
	return true;

}


static JSValueRef ConvertValue JSTNativeConvertor() {
	JSObjectRef jsAddressObject = JSTGetPropertyObject(object, "pointer");
	void * address = JSTGetPrivate(jsAddressObject);
	JSObjectRef container = JSTGetPropertyObject(object, "container");
	if (JSTHasProperty(container, "deallocated")) {
		JSTReferenceError("JSNative.Value: cannot get data: container has been deallocated");
		return NULL;
	}
	if (!address) {
		return RtJS(undefined);
	}
	int typeCode = JSTInteger(JSTEval("this.type.code", object));
	int count = JSTInteger(JSTGetProperty(object, "count"));
	JSValueRef value = js_native_value_read_address(ctx, address, typeCode, exception);

	if (type == JSTNumberType) {
		return value;
	} else
	if (type == JSTStringType) {
		if (typeCode == 20 || typeCode == 21) {
			return JSTCallProperty(RtJSObject(String), "fromCharCode", value);
		}
		return JSTCall(RtJSObject(String), NULL, value);
	}
	return NULL;

}

void js_native_value_init JSToolsProcedure(JSObjectRef object) {

	JSClassDefinition jsNative = kJSClassDefinitionEmpty;
	jsNative.className = "JSNative.Value";
	jsNative.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsNative.callAsConstructor = &ConstructValue;
	jsNative.setProperty = &SetProperty;
	jsNative.getProperty = &GetValueProperty;
	jsNative.convertToType = &ConvertValue;
	JSNativeValue = JSClassRetain(JSClassCreate(&jsNative));

	RtJSNativeValue = JSTMakeConstructor(JSNativeValue, &ConstructValue);
	JSTSetProperty(object, "Value", RtJSNativeValue, JSTPropertyConst);

	jsNative.className = "JSNative.Array";
	jsNative.callAsConstructor = &ConstructArray;
	jsNative.convertToType = &ConvertArray;
	jsNative.getProperty = &GetArrayProperty;
	JSStaticValue arrayProperties[] = {
		{ "length",		&GetArrayProperty, NULL, JSTPropertyConst },
		{ 0, 0, 0, 0 }
	};
	jsNative.staticValues = arrayProperties;
	JSNativeArray = JSClassRetain(JSClassCreate(&jsNative));

	RtJSNativeArray = JSTMakeConstructor(JSNativeValue, &ConstructArray);
	JSTSetProperty(object, "Array", RtJSNativeArray, JSTPropertyConst);

}
