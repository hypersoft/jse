/* Brigadier */
#include <stdio.h>

#define JSNative true

#include "JSNative.inc"
#include "JSNative.h"

JSObjectRef RtJSNative = NULL;

void js_native_init JSToolsProcedure (JSObjectRef global) {

	/* our root object */
	JSTSetProperty(global, "JSNative", (RtJSNative = JSTCreateClassObject(NULL, NULL)), JSTPropertyConst);

	JSTEval(JSNativeSupport, global);

}

