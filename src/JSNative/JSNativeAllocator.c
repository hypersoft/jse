#define JSNativeAllocatorSource

#include "JSNative.h"

JSClassRef JSNativeAllocator = NULL;

JSObjectRef RtJSNativeAllocator = NULL;
JSObjectRef RtJSNativeAllocatorLease = NULL;
JSObjectRef RtJSNativeAllocatorStack = NULL;
JSObjectRef RtJSNativeAllocatorStackClaim = NULL;

static JSValueRef js_native_allocator_free(JSContextRef ctx, JSObjectRef function, JSObjectRef this, size_t argc, const JSValueRef argv[], JSValueRef *exception);

#define JSNativeAllocatorClaim(VAL)	(JSValueRef) (JSTCall(RtJSNativeAllocatorStackClaim, RtJSNativeAllocatorStack, VAL))

static JSObjectRef js_native_allocator_construct(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	JSObjectRef this = JSTCreateClassObject(JSNativeAllocator, JSTPointer(JSTGetProperty(RtJSNativeAllocatorStack, "length")));
	JSTSetPropertyFunction(this, "free", &js_native_allocator_free);
	return this;
}

static JSValueRef js_native_allocator_lease JSToolsFunction () {

	JSValueRef size = JSTArgument(0), count = JSTArgument(1), zero = JSTArgument(2);

	if (JSTObject(size)) size = JSTGetProperty((JSObjectRef)size, "size");
	unsigned long bytes = ( JSTInteger(size) * JSTInteger(count) );

	if (bytes) {
		if (JSTBoolean(zero)) this = JSNativeMakeAddress(g_malloc0(bytes));
		else this = JSNativeMakeAddress(g_malloc(bytes));
		JSTSetProperty(this, "allocated", JSTMakeBoolean(true), JSTPropertyProtected);
		JSNativeAllocatorClaim(this);
		return (JSValueRef) this;
	}

	JSTTypeError("JSNative.Allocator.lease: result of input parameters is void");
	return RtJS(undefined);

}

static JSValueRef js_native_allocator_release(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	if (! JSValueIsObjectOfClass(ctx, (JSValueRef) thisObject, JSNativeAllocator) ) {
		JSTTypeError("JSNative.Allocator.release: 'this' is not a JSNative.Allocator");
		return RtJS(undefined);
	}
	JSValueRef truncate = JSTMakeNumber(JSTGetPrivateInteger(thisObject));
	register JSObjectRef stack = JSTCallPropertyObject(RtJSNativeAllocatorStack, "slice", truncate);
	register JSObjectRef pop = JSTGetPropertyObject(stack, "pop");
	JSTSetProperty(RtJSNativeAllocatorStack, "length", truncate, JSTPropertyRequired);
	long status = 0;
	register JSObjectRef result;
	while (! JSTUndefined(result = JSTCallObject(pop, stack)) ) {
		JSTCallProperty(thisObject, "free", result);
		status++;
	}
	return JSTMakeNumber(status);
}

static JSValueRef js_native_allocator_free(JSContextRef ctx, JSObjectRef function, JSObjectRef this, size_t argc, const JSValueRef argv[], JSValueRef *exception) {
	JSObjectRef jsAddressObject = JSTParamObject(1);
	if (! JSValueIsObjectOfClass(ctx, (JSValueRef) jsAddressObject, JSNativeAddress) ) {
		JSTTypeError("JSNative.Allocator.free: parameter 1: expected [object JSNative.Address]");
		goto imdead;
	}

	if (! JSTHasProperty(jsAddressObject, "allocated") || JSTHasProperty(jsAddressObject, "deallocated")) goto imdead;
	g_free(JSTGetPrivate(jsAddressObject)); JSTSetPrivate(jsAddressObject, NULL);
	JSTSetProperty(jsAddressObject, "deallocated", JSTMakeBoolean(true), JSTPropertyProtected);

	return JSTMakeBoolean(true);
imdead:
	return JSTMakeBoolean(false);
}


void js_native_allocator_init JSToolsProcedure (JSObjectRef object) {

	JSStaticFunction StaticFunctionArray[] = {
		{ "release", &js_native_allocator_release, JSTPropertyConst },
		{ "free", &js_native_allocator_free, JSTPropertyConst },
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


