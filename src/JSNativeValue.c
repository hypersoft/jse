#include "JSTools.h"

#include "JSNative.h"
#include "JSNativeType.h"
#include "JSNativeAddress.h"
#include "JSNativeAllocator.h"

JSClassRef JSNativeValue = NULL;
JSObjectRef RtJSNativeValue = NULL;

JSValueRef js_native_value_read_address(JSContextRef ctx, void * address, JSNativeTypeCode type, JSValueRef * exception) {

	char * e[] = { // error strings
		"js_address_read: cannot read null base pointer",
		"js_address_read: cannot read void type",
		"js_address_read: cannot read unknown native type code",
		NULL
	};
	if ( ! address ) { js_throw_exception(ctx, e[0], exception); return NULL; }
	if (type == JSNativeTypeVoid) { js_throw_exception(ctx, e[1], exception); return NULL; }
	if (JSNativeTypeCodeIsUnsigned(type)) { type--;
		if (type == JSNativeTypeChar) return JSTMakeNumber((double) (unsigned char) *(unsigned char*)(address));
		else if (type == JSNativeTypeShort) return JSTMakeNumber((double) *(unsigned short*)(address));
		else if (type == JSNativeTypeInt) return JSTMakeNumber((double) *(unsigned int*)(address));
		else if (type == JSNativeTypeLong) return JSTMakeNumber((double) *(unsigned long*)(address));
		else if (type == JSNativeTypeLongLong) return JSTMakeNumber((double) *(unsigned long long*)(address));
		else { js_throw_exception(ctx, e[2], exception); return NULL; }
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
		else { js_throw_exception(ctx, e[2], exception); return NULL; }
	}
	return NULL;
}

bool js_native_value_write_address JSTProcedure (void * address, JSNativeTypeCode type, JSValueRef value) {

	char * e[] = { // error strings
		"js_address_write: cannot write null base pointer",
		"js_address_write: cannot write void type",
		"js_address_write: cannot write unknown native type code",
		NULL
	};
	if ( ! address ) { js_throw_exception(ctx, e[0], exception); return false; }
	if (type == JSNativeTypeVoid) { js_throw_exception(ctx, e[1], exception); return false; }
	if (JSNativeTypeCodeIsUnsigned(type)) { type--;
		if (type == JSNativeTypeChar) { *(unsigned char*)(address) = (unsigned char) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeShort) { *(unsigned short*)(address) = (unsigned short) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeInt) { *(unsigned int*)(address) = (unsigned int) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeLong) { *(unsigned long*)(address) = (unsigned long) JSValueToNumber(ctx, value, exception); return true; }
		else if (type == JSNativeTypeLongLong) { *(unsigned long long*)(address) = (unsigned long long) JSValueToNumber(ctx, value, exception); return true; }
		else { js_throw_exception(ctx, e[2], exception); return false; }
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
		else { js_throw_exception(ctx, e[2], exception); return false; }
	}
	return false;
}


/*
void Initialize(JSContextRef ctx, JSObjectRef object);
void Finalize(JSObjectRef object);
bool HasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName);
bool DeleteProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception);
bool HasInstance(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);


static JSValueRef GetProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) {

}

static bool SetProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {

}

static JSValueRef CallAsFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

}

static JSValueRef ConvertToType(JSContextRef ctx, JSObjectRef object, JSType type, JSValueRef* exception) {

}

*/

static JSObjectRef JSNativeMakeValue JSTNativeConstructor(type, count, [value]) {

	/*	ARGUMENTS:

		1. Type Object, Type String, or Type Number
		2. Positive Integer
		3. Address Object, Number

		Parameter 3 is optional
	*/

	JSObjectRef this = NULL;
	JSValueRef type = JSTArgument(0), count = JSTArgument(1), value = JSTArgument(2);

	if ( ! JSTReference(type) || ! JSTReference(count) ) goto bugger;
	if ( ! JSTReference((type = JSNativeTypeLookup(type))) ) goto bugger;

	if ( ! JSTReference(value) ) {
		this = (JSObjectRef) JSNativeAllocatorLease(type, count, true);
		JSTSetProperty(this, "type", type, JSTPropertyConst); goto bugger;
	} else {

	}

bugger:
	if (this) return this;
	return JSTKnownObject(JSTMakeUndefined());
}

void js_native_value_class_init(JSContextRef ctx, JSObjectRef object, JSValueRef * exception) {

	JSClassDefinition ClassDef = kJSClassDefinitionEmpty;

	ClassDef.className = "JSNative.Value";
	ClassDef.attributes = kJSClassAttributeNoAutomaticPrototype;

/*
	JSStaticValue StaticValueArray[] = {
		{ "", NULL, NULL, kJSPropertyAttributeDontDelete },
		{ 0, 0, 0, 0 }
	}; */
/*	JSStaticFunction StaticFunctionArray[] = {
		{ "", &js_native_address_is_access_aligned, kJSPropertyAttributeDontDelete },
		{ "seekAlignForward", &js_native_address_seek_align_forward, kJSPropertyAttributeDontDelete },
		{ "seekAlignReverse", &js_native_address_seek_align_reverse, kJSPropertyAttributeDontDelete },
		{ NULL, 0, 0 }
	}; */

/*	ClassDef.getProperty = &js_native_address_get_index;
	ClassDef.convertToType = &js_native_address_convert;
	ClassDef.staticValues = StaticValueArray;
	ClassDef.staticFunctions = StaticFunctionArray; */

	JSNativeValue = JSClassRetain(JSClassCreate(&ClassDef));

	JSObjectRef constructor = JSTMakeConstructorProperty(object, "Value", JSNativeValue, &JSNativeMakeValue);
	JSTSetProperty(constructor, "byteOrder", JSTMakeNumber(G_BYTE_ORDER), JSTPropertyConst);
	JSTSetProperty(constructor, "wordSize", JSTMakeNumber(__WORDSIZE), JSTPropertyConst);

}

