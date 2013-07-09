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
	if (JSTCaughtException) JSTReportFatalException(1, "JSNative Type initialization error");

	JSToolsCall(js_native_address_init, RtJSNative);
	if (JSTCaughtException) JSTReportFatalException(1, "JSNative Address initialization error");

	JSToolsCall(js_native_allocator_init, RtJSNative);
	if (JSTCaughtException) JSTReportFatalException(1, "JSNative Allocator initialization error");

	JSToolsCall(js_native_value_init, RtJSNative);
	if (JSTCaughtException) JSTReportFatalException(1, "JSNative Value initialization error");

	JSTEval(JSNativeSupport, global);
	if (JSTCaughtException) JSTReportFatalException(1, "JSNative script initialization error");

}

