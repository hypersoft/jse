#ifndef JSNativeType

#define JSNativeType JSNativeType

#ifndef JSNativeTypeSource
	extern JSClassRef JSNativeType;
	extern JSObjectRef RtJSNativeType, RtJSNativeTypeAlias;
#endif

typedef unsigned JSNativeTypeCode;

void js_native_type_init JSToolsProcedure (JSObjectRef jsObjJSNative);

#define JSNativeTypeVoid 0
#define JSNativeTypeBool 10	
#define JSNativeTypeChar 20	
#define JSNativeTypeShort 30
#define JSNativeTypeInt 40
#define JSNativeTypeLong 50
#define JSNativeTypeLongLong 60
#define JSNativeTypeFloat 70
#define JSNativeTypeDouble 80
#define JSNativeTypePointer 90

#define JSNativeTypeCodeIsInteger(V) (V < 70 && V > 10)
#define JSNativeTypeCodeIsUnsigned(V) (JSNativeTypeCodeIsInteger(V) && (V % 10))

#define JSNativeTypeDeref(VAL)  ((JSObjectRef) JSTCall(RtJSNativeType, RtJSNative, VAL))

#endif
