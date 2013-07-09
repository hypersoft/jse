extern JSClassRef JSNativeAddress;
extern JSObjectRef RtJSNativeAddress;

void js_native_address_init JSToolsProcedure (JSObjectRef object);

#define JSNativeMakeAddress(VAL) JSTCreateClassObject(JSNativeAddress, VAL)
#define JSNativeAddressSetAllocated(OBJ, BOOL) JSTSetProperty(OBJ, "allocated", JSTMakeBoolean(BOOL), JSTPropertyConst)
