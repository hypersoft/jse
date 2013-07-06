/* Brigadier */

#include "JSTools.h"

#include "JSNative.inc"

JSObjectRef RtJSNative = NULL;


void js_native_init JSTNativeProcedure (JSObjectRef global) {

	JSTCacheRuntime();

	/* our root object */
	JSTSet(global, "JSNative", (RtJSNative = JSTCreateObject(NULL, NULL)), JSTPropertyConst);
	JSTCall(global, "parseInt");

}

