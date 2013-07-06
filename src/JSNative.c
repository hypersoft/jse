/* Brigadier */
#include <stdio.h>

#define JSNative true

#include "JSNative.inc"
#include "JSNative.h"

JSObjectRef RtJSNative = NULL;

void js_native_init JSTNativeProcedure (JSObjectRef global) {

	/* our root object */
	JSTSet(global, "JSNative", (RtJSNative = JSTCreateObject(NULL, NULL)), JSTPropertyConst);

	JSTEval(JSNativeSupport, global);

	if (JSTCaughtException) {
		fprintf(stderr, "JSNative.c: js_native_init: WARNING: JSNativeSupport Script Exception: \nLine %i: %s\n",
			JSTInteger(JSTGet(RtJSException, "line")), JSTNativeGetString(*exception)
		);
		*exception = NULL;
	}

}

