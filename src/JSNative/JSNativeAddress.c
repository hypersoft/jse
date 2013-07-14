#define JSNativeAddressSource

#include "JSNative.h"

JSClassRef JSNativeAddress = NULL;
JSObjectRef RtJSNativeAddress = NULL;


static JSObjectRef js_native_address_construct JSTNativeConstructor (JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	return JSTCreateClassObject(JSNativeAddress, JSTPointer(JSTArgument(0)));
}

JSValueRef js_native_address_convert(JSContextRef ctx, JSObjectRef object, JSType type, JSValueRef* exception) {
	if (type == kJSTypeBoolean)return JSTMakeBoolean((long)JSTGetPrivate(object));
	if (type == kJSTypeNumber)return JSTMakeNumber((long)JSTGetPrivate(object));
	if (type == kJSTypeString) return JSTEval("'0x' + Number(this).toString(16)", object);
	else return NULL;
}

static JSValueRef CallAsFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	puts("called me");
	return JSValueMakeNull(ctx);
}


void js_native_address_init(JSContextRef ctx, JSObjectRef object, JSValueRef * exception) {

	JSClassDefinition jsNative = kJSClassDefinitionEmpty;
	jsNative.className = "JSNative.Address";
	jsNative.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsNative.convertToType = &js_native_address_convert;

	JSNativeAddress = JSClassRetain(JSClassCreate(&jsNative));

	register JSObjectRef RtJSNativeAddress = JSTMakeConstructor(JSNativeAddress, &js_native_address_construct);
	JSTSetProperty(object, "Address", RtJSNativeAddress, JSTPropertyConst);
	JSTSetProperty(RtJSNativeAddress, "alignment", JSTMakeNumber(G_MEM_ALIGN), JSTPropertyConst);

}


