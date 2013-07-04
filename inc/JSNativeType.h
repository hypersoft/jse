typedef unsigned int JSNativeTypeCode;

extern JSClassRef JSNativeType;

extern JSObjectRef RtJSNativeType;
extern JSObjectRef RtJSNativeTypeAlias;

#define JSNativeTypeLookup(VAL)  ( (JSObjectRef) JSTCallFunction(RtJSNative, RtJSNativeType, VAL, NULL))
#define JSNativeTypeCodeIsInteger(V) (V < 70 && V > 10)
#define JSNativeTypeCodeIsUnsigned(V) (JSNativeTypeCodeIsInteger(V) && (V % 10))

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

