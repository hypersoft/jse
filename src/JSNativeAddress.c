#include "JSTools.h"

JSClassRef JSNativeAddress = NULL;

static JSObjectRef js_native_address_construct(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	return JSTCreateObject(JSNativeAddress, JSTPointer(JSTArgument(0)));
}

JSValueRef js_native_address_get_pointer(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) {
	return JSTMakeNumber((long) JSObjectGetPrivate(object));
}

bool js_native_address_set_pointer(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
	return JSTSetPrivate(object, JSTPointer(value));
}

JSValueRef js_native_address_get_index(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) {
	register JSValueRef jsPropertyName = JSTMakeCoreString(propertyName);
	register JSValueRef jsIndex = JSTParseInt(jsPropertyName);
	if ( ! JSTNaN(jsIndex) ) { return (JSValueRef)
		JSTCreateObject(JSNativeAddress, (long) JSTGetPrivate(object) + (long) JSTNumber(jsIndex));
	}
	return JSTMakeNull();
}

JSValueRef js_native_address_convert(JSContextRef ctx, JSObjectRef object, JSType type, JSValueRef* exception) {
	if (type == kJSTypeNumber)
	return JSTGetProperty(object, "pointer"); else if (type == kJSTypeString) 
	return JSTCall(JSTMakeObject(JSTGetProperty(object, "pointer")), "toString", JSTMakeNumber(16));
	else return NULL;
}

static JSValueRef CallAsFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	puts("called me");
	return JSValueMakeNull(ctx);
}

static JSValueRef js_native_address_is_access_aligned(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	unsigned long address = (unsigned long) JSTGetPrivate(thisObject), alignment = 4;
	if (argumentCount) alignment = (unsigned long) JSTNumber(arguments[0]);
	return JSTMakeBoolean(((address % alignment) == 0));
}

// [count, [alignment]]
static JSValueRef js_native_address_seek_align_forward(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	register unsigned long
	address = (unsigned long) JSTGetPrivate(thisObject), alignment = 4, count = 1, hit=0;
	if (argumentCount) count = (unsigned long) JSTNumber(arguments[0]);
	if (--argumentCount) alignment = (unsigned long) JSTNumber(arguments[1]);
	if ((hit = (address % alignment))) { address += hit; --count; }
	if (count) address += (alignment * count);
	return (JSValueRef) JSTCreateObject(JSNativeAddress, address);
}

// [count, [alignment]]
static JSValueRef js_native_address_seek_align_reverse(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	register unsigned long
	address = (unsigned long) JSTGetPrivate(thisObject), alignment = 4, count = 1, hit=0;
	if (argumentCount) count = (unsigned long) JSTNumber(arguments[0]);
	if (--argumentCount) alignment = (unsigned long) JSTNumber(arguments[1]);
	if ((hit = (address % alignment))) { address -= hit; --count; }
	if (count) address -= (alignment * count);
	return (JSValueRef) JSTCreateObject(JSNativeAddress, address);
}

void js_native_address_class_init(JSContextRef ctx, JSObjectRef object, JSValueRef * exception) {

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

	register JSObjectRef constructor = JSTMakeConstructorProperty
		(object, "Address", JSNativeAddress, &js_native_address_construct);

	JSTSetProperty(constructor, "alignment", JSTMakeNumber(G_MEM_ALIGN), JSTPropertyConst);

}

