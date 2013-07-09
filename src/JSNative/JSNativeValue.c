#define JSNativeValueSource

#include "JSNative.h"

JSClassRef JSNativeValue = NULL;
JSObjectRef RtJSNativeValue = NULL;

static JSObjectRef Construct JSTNativeConstructor () {
	JSObjectRef this = JSTCreateClassObject(JSNativeValue, NULL);
	return this;
}

static JSValueRef GetProperty JSTNativePropertyReader() {

}

static bool SetProperty JSTNativePropertyWriter() {

}

static JSValueRef Convert JSTNativeConvertor() {

}

void js_native_value_init JSToolsProcedure(JSObjectRef object) {

	JSClassDefinition jsNative = kJSClassDefinitionEmpty;
	jsNative.className = "JSNative.Value";
	jsNative.callAsConstructor = &Construct;
	jsNative.setProperty = &SetProperty;
	jsNative.getProperty = &GetProperty;
	JSNativeValue = JSClassRetain(JSClassCreate(&jsNative));

	RtJSNativeValue = JSTMakeConstructor(JSNativeValue, &Construct);

	JSTSetProperty(object, "Value", RtJSNativeValue, JSTPropertyConst);


}
