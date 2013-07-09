/* Brigadier */
#define JSNativeSource

#include "JSNative.inc"
#include "JSNative.h"

JSObjectRef RtJSNative = NULL;

void js_native_init JSToolsProcedure (int argc, char *argv[], char *envp[]) {

	JSObjectRef global = JSTGetGlobalObject();

	JSTLoadRuntime(global, argc, argv, envp);

	/* our root object */
	JSTSetProperty(global, "JSNative", (RtJSNative = JSTCreateClassObject(NULL, NULL)), JSTPropertyConst);

	JSToolsCall(js_native_type_init, RtJSNative);

	JSTEval(JSNativeSupport, global);

	if (JSTCaughtException) {
		JSTReportError("JSNative script initialization error");
		exit(1);
	}

}

