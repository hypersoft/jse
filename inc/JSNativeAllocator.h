extern JSClassRef JSNativeAllocator;
extern JSObjectRef
RtJSNativeAllocator, RtJSNativeAllocatorLease, RtJSNativeAllocatorStack, RtJSNativeAllocatorStackClaim;

#define JSNativeAllocatorClaim(VAL)	(JSValueRef) \
(JSTCallFunction(RtJSNativeAllocatorStack, RtJSNativeAllocatorStackClaim, VAL, NULL))

#define JSNativeAllocatorLease(TYPE, COUNT, ZERO) JSTCallFunction(RtJSNativeAllocator, RtJSNativeAllocatorLease, TYPE, COUNT, ZERO, NULL)

