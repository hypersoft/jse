#define JSNativeAddressSource

#include "JSNative.h"

JSClassRef JSNativeAddress = NULL;
JSObjectRef RtJSNativeAddress = NULL;

static JSObjectRef js_native_address_construct JSTNativeConstructor (JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	return JSTCreateClassObject(JSNativeAddress, JSTPointer(JSTArgument(0)));
}

JSValueRef js_native_address_get_pointer(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) {
	return JSTMakeNumber((long) JSObjectGetPrivate(object));
}

bool js_native_address_set_pointer(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
	return JSTSetPrivate(object, JSTPointer(value));
}

JSValueRef js_native_address_get_index(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) {
	register JSValueRef jsPropertyName = JSTMakeString(propertyName, NULL, false);
	register JSValueRef jsIndex = JSTCall(RtJS(parseInt), NULL, jsPropertyName);
	if ( ! JSTMakeBoolean(JSTCall(RtJS(isNaN), NULL, jsIndex)) ) { return (JSValueRef)
		JSTCreateClassObject(JSNativeAddress, (void*) (long) JSTGetPrivate(object) + (long) JSTNumber(jsIndex));
	}
	return RtJS(undefined);
}

JSValueRef js_native_address_convert(JSContextRef ctx, JSObjectRef object, JSType type, JSValueRef* exception) {
	if (type == kJSTypeNumber)
	return JSTGetProperty(object, "pointer"); else if (type == kJSTypeString) 
	return JSTCallProperty(JSTGetPropertyObject(object, "pointer"), "toString", JSTMakeNumber(16));
	else return NULL;
}

static JSValueRef CallAsFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	puts("called me");
	return JSValueMakeNull(ctx);
}

static JSValueRef js_native_address_is_access_aligned(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	unsigned long address = (unsigned long) JSTGetPrivate(thisObject), alignment = 4;
	if (argumentCount) alignment = (unsigned long) JSTInteger(arguments[0]);
	return JSTMakeBoolean(((address % alignment) == 0));
}

// [count, [alignment]]
static JSValueRef js_native_address_seek_align_forward(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	register unsigned long
	address = (unsigned long) JSTGetPrivate(thisObject), alignment = 4, count = 1, hit=0;
	if (argumentCount) count = (unsigned long) JSTInteger(arguments[0]);
	if (--argumentCount) alignment = (unsigned long) JSTInteger(arguments[1]);
	if ((hit = (address % alignment))) { address += hit; --count; }
	if (count) address += (alignment * count);
	return (JSValueRef) JSTCreateClassObject(JSNativeAddress, (void *) address);
}

// [count, [alignment]]
static JSValueRef js_native_address_seek_align_reverse(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	register unsigned long
	address = (unsigned long) JSTGetPrivate(thisObject), alignment = 4, count = 1, hit=0;
	if (argumentCount) count = (unsigned long) JSTInteger(arguments[0]);
	if (--argumentCount) alignment = (unsigned long) JSTInteger(arguments[1]);
	if ((hit = (address % alignment))) { address -= hit; --count; }
	if (count) address -= (alignment * count);
	return (JSValueRef) JSTCreateClassObject(JSNativeAddress, (void *) address);
}

void js_native_address_init(JSContextRef ctx, JSObjectRef object, JSValueRef * exception) {

	JSStaticValue StaticValueArray[] = {
		{ "pointer", &js_native_address_get_pointer, &js_native_address_set_pointer, JSTPropertyConst },
		{ 0, 0, 0, 0 }
	};
	JSStaticFunction StaticFunctionArray[] = {
		{ "isAccessAligned", &js_native_address_is_access_aligned, JSTPropertyConst },
		{ "seekAlignForward", &js_native_address_seek_align_forward, JSTPropertyConst },
		{ "seekAlignReverse", &js_native_address_seek_align_reverse, JSTPropertyConst },
		{ NULL, 0, 0 }
	};

	JSClassDefinition jsNative = kJSClassDefinitionEmpty;
	jsNative.className = "JSNative.Address";
	jsNative.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsNative.getProperty = &js_native_address_get_index;
	jsNative.convertToType = &js_native_address_convert;
	jsNative.staticValues = StaticValueArray;
	jsNative.staticFunctions = StaticFunctionArray;

	JSNativeAddress = JSClassRetain(JSClassCreate(&jsNative));

	register JSObjectRef RtJSNativeAddress = JSTMakeConstructor(JSNativeAddress, &js_native_address_construct);
	JSTSetProperty(object, "Address", RtJSNativeAddress, JSTPropertyConst);
	JSTSetProperty(RtJSNativeAddress, "alignment", JSTMakeNumber(G_MEM_ALIGN), JSTPropertyConst);

}


