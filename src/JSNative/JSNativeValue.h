#ifndef JSNativeValue
#define JSNativeValue JSNativeValue

#ifndef JSNativeValueSource
	extern JSClassRef JSNativeValue;
	extern JSObjectRef RtJSNativeValue;
	extern JSObjectRef RtJSNativeArray;
#endif

void js_native_value_init JSToolsProcedure(JSObjectRef object);

#endif
