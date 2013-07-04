/* Brigadier */

#include "JSTools.h"
#include "JSNative.inc"

JSObjectRef RtJSNative = NULL;

void js_native_init JSTProcedure (JSObjectRef object) {

	RtJSNative = JSTCreateObject(NULL, NULL); /* our root object */

	JSTSetProperty(object, "JSNative", RtJSNative, JSTPropertyRequired);

	js_native_type_class_init 		JSTExec	(RtJSNative);
	js_native_address_class_init 	JSTExec	(RtJSNative);
	js_native_allocator_class_init	JSTExec	(RtJSNative);
	js_native_value_class_init 		JSTExec	(RtJSNative);

	JSTEval(JSNativeSupport, object);

}

