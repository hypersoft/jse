extern JSClassRef JSNativeAddress;

#define JSNativeMakeAddress(VAL) JSTCreateObject(JSNativeAddress, VAL)
#define JSNativeAddressSetAllocated(OBJ, BOOL) JSTSetProperty(OBJ, "allocated", JSTMakeBoolean(BOOL), JSTPropertyConst)

