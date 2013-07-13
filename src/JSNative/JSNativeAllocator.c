#define JSNativeAllocatorSource

#include "JSNative.h"

JSClassRef JSNativeAllocator = NULL;

JSObjectRef RtJSNativeAllocator = NULL;
JSObjectRef RtJSNativeAllocatorLease = NULL;
JSObjectRef RtJSNativeAllocatorStack = NULL;
JSObjectRef RtJSNativeAllocatorStackClaim = NULL;

#define JSNativeAllocatorClaim(VAL)	(JSValueRef) (JSTCall(RtJSNativeAllocatorStackClaim, RtJSNativeAllocatorStack, VAL))

static JSObjectRef js_native_allocator_construct(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	return JSTCreateClassObject(JSNativeAllocator, JSTPointer(JSTGetProperty(RtJSNativeAllocatorStack, "length")));
}

static JSValueRef js_native_allocator_lease JSToolsFunction () {

	JSValueRef size = JSTArgument(0), count = JSTArgument(1), zero = JSTArgument(2);

	JSObjectRef o = NULL;
	if (JSTObject(size)) size = JSTGetProperty((JSObjectRef)size, "size");
	unsigned long bytes = ( JSTInteger(size) * JSTInteger(count) );

	if (bytes) {
		if (JSTBoolean(zero)) this = JSNativeMakeAddress(g_malloc0(bytes));
		else this = JSNativeMakeAddress(g_malloc(bytes)); JSNativeAddressSetAllocated(this, true);
		JSNativeAllocatorClaim(this); return (JSValueRef) this;
	}
printf("no allocation\n");
	// error!
	return NULL;

}

static JSValueRef js_native_allocator_release(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	JSValueRef truncate = JSTMakeNumber(JSTGetPrivateInteger(thisObject));
	register JSObjectRef next = JSTCallObject (JSTGetPropertyObject(RtJSNativeAllocatorStack, "slice"), RtJSNativeAllocatorStack, truncate);
	register JSObjectRef address = JSTGetPropertyObject(next, "pop");
	JSTSetProperty(RtJSNativeAllocatorStack, "length", truncate, JSTPropertyRequired);
	while (true) { /* register JSObjectRef globalBase used as temp here ... */
		register JSObjectRef result = JSTCallObject(next, address, NULL);
		if ( JSTReference(result) ) { g_free(JSTGetPrivate(result));
			JSTSetPrivate(result, NULL); JSNativeAddressSetAllocated(result, false);
		} else break;
	}
	return RtJS(undefined);
}

void js_native_allocator_init JSToolsProcedure (JSObjectRef object) {

	JSStaticFunction StaticFunctionArray[] = {
		{ "release", &js_native_allocator_release, JSTPropertyConst },
		{ NULL, 0, 0 }
	};

	JSClassDefinition jsNative = kJSClassDefinitionEmpty;
	jsNative.className = "JSNative.Allocator";
	jsNative.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsNative.staticFunctions = StaticFunctionArray; 
	JSNativeAllocator = JSClassRetain(JSClassCreate(&jsNative));

	RtJSNativeAllocator = JSTMakeConstructor(JSNativeAllocator, &js_native_allocator_construct);
	JSTSetProperty(RtJSNativeAllocator, "globalBase", (RtJSNativeAllocatorStack = JSObjectMakeArray(ctx, 0, NULL, exception)), JSTPropertyProtected);
	RtJSNativeAllocatorStackClaim = (JSObjectRef) JSTGetProperty(RtJSNativeAllocatorStack, "push");
	RtJSNativeAllocatorLease = JSTSetPropertyFunction(RtJSNativeAllocator, "lease", &js_native_allocator_lease);

	JSTSetProperty(object, "Allocator", RtJSNativeAllocator, JSTPropertyConst);

}


