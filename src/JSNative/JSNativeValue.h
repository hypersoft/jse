#ifndef JSNativeValue
#define JSNativeValue JSNativeValue

#ifndef JSNativeValueSource
	extern JSClassRef JSNativeValue;
	extern JSObjectRef RtJSNativeValue;
#endif

void js_native_value_init JSToolsProcedure(JSObjectRef object);

#endif
