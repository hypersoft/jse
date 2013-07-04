#include "JSTools.h"
#include "JSNativeAddress.h"

JSClassRef JSNativeAllocator = NULL;

JSObjectRef RtJSNativeAllocator = NULL;
JSObjectRef RtJSNativeAllocatorLease = NULL;
JSObjectRef RtJSNativeAllocatorStack = NULL;
JSObjectRef RtJSNativeAllocatorStackClaim = NULL;

#define JSNativeAllocatorClaim(VAL)	(JSValueRef) (JSTCallFunction(RtJSNativeAllocatorStack, RtJSNativeAllocatorStackClaim, VAL, NULL))

static JSObjectRef js_native_allocator_construct(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	return JSTCreateObject(JSNativeAllocator, JSTPointer(JSTGetProperty(RtJSNativeAllocatorStack, "length")));
}

static JSValueRef js_native_allocator_lease JSTNativeFunction(size, count, zero) {

	JSValueRef size = JSTArgument(0), count = JSTArgument(1), zero = JSTArgument(2);

	JSObjectRef this = NULL;
	if (JSTObject(size)) size = JSTGetProperty(JSTKnownObject(size), "size");
	unsigned long bytes = ( JSTNumber(size) * JSTNumber(count) );

	if (bytes) {
		if (JSTBoolean(zero)) this = JSNativeMakeAddress(g_malloc0(bytes));
		else this = JSNativeMakeAddress(g_malloc(bytes)); JSNativeAddressSetAllocated(this, true);
		JSNativeAllocatorClaim(this); return (JSValueRef) this;
	}

	js_throw_exception(ctx, "no data", exception);
	JSTMaybeReturnValueException("JSNative.Allocator.lease: %s\n", "unable to allocate empty request");
}

static JSValueRef js_native_allocator_release(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	JSValueRef truncate = JSTMakeNumber(JSTGetPrivateInteger(thisObject));
	register JSObjectRef next = JSTKnownObject ( JSTCallFunction ( RtJSNativeAllocatorStack,
		JSTGetProperty(RtJSNativeAllocatorStack, "slice"), truncate)
	);
	register JSObjectRef address = JSTKnownObject (JSTGetProperty(next, "pop"));
	JSTSetProperty(RtJSNativeAllocatorStack, "length", truncate, JSTPropertyRequired);
	while (true) { /* register JSObjectRef globalBase used as temp here ... */
		register JSObjectRef result = JSTKnownObject(JSTCallFunction(next, address, NULL));
		if ( JSTReference(result) ) { g_free(JSTGetPrivate(result));
			JSTSetPrivate(result, NULL); JSNativeAddressSetAllocated(result, false);
		} else break;
	}
	return JSTMakeUndefined();
}

void js_native_allocator_class_init JSTProcedure (JSObjectRef object) {

	JSStaticFunction StaticFunctionArray[] = {
		{ "release", &js_native_allocator_release, JSTPropertyConst },
		{ NULL, 0, 0 }
	};

	JSClassDefinition jsNative = kJSClassDefinitionEmpty;
	jsNative.className = "JSNative.Allocator";
	jsNative.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsNative.staticFunctions = StaticFunctionArray; 
	JSNativeAllocator = JSClassRetain(JSClassCreate(&jsNative));

	RtJSNativeAllocator = JSTMakeConstructorProperty(object, "Allocator", JSNativeAllocator, &js_native_allocator_construct);
	JSTSetProperty(RtJSNativeAllocator, "globalBase", (RtJSNativeAllocatorStack = JSObjectMakeArray(ctx, 0, NULL, exception)), JSTPropertyProtected);
	RtJSNativeAllocatorStackClaim = (JSObjectRef) JSTGetProperty(RtJSNativeAllocatorStack, "push");
	RtJSNativeAllocatorLease = JSTMakePropertyFunction(RtJSNativeAllocator, "lease", &js_native_allocator_lease);

}

