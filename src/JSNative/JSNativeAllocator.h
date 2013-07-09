#ifndef JSNativeAllocator
#define JSNativeAllocator JSNativeAllocator

#ifndef JSNativeAllocatorSource
	extern JSClassRef JSNativeAllocator;
	extern JSObjectRef RtJSNativeAllocator;
	extern JSObjectRef RtJSNativeAllocatorLease;
	extern JSObjectRef RtJSNativeAllocatorStack;
	extern JSObjectRef RtJSNativeAllocatorStackClaim;
#endif

#define JSNativeAllocatorClaim(VAL)	(JSValueRef) (JSTCall(RtJSNativeAllocatorStackClaim, RtJSNativeAllocatorStack, VAL))

void js_native_allocator_init JSToolsProcedure (JSObjectRef object);

#endif

