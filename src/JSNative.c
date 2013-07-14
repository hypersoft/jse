/* Brigadier */
#define JSNativeSource

#include "JSNative.inc"
#include "JSNative.h"

JSObjectRef RtJSNative = NULL;

static JSValueRef jsnNewCallVM JSToolsFunction(DCsize size) {
	if (argc != 1) {
		JSTSyntaxError("JSNative: jsnNewCallVM: expected 1 integer argument: size");
		return RtJS(undefined);
	}
	return JSNativeMakeAddress(dcNewCallVM(JSTInteger(JSTParam(1))));
}

static JSValueRef jsnCallVMFree JSToolsFunction(DCCallVM * vm) {
	if (argc != 1) {
		JSTSyntaxError("JSNative: jsnCallVMFree: expected 1 integer argument: (JSNative.CallVM) pointer");
		return RtJS(undefined);
	} 
	dcFree(JSTPointer(JSTParam(1))); return RtJS(undefined);
}

static JSValueRef jsnCallVMGetError JSToolsFunction(DCCallVM * vm) {
	if (argc != 1) {
		JSTSyntaxError("JSNative: jsnCallVMGetError: expected 1 integer argument: (JSNative.CallVM) pointer");
		return RtJS(undefined);
	}
	return JSTMakeNumber(dcGetError(JSTPointer(JSTParam(1))));
}

static JSValueRef jsnCallVMSetMode JSToolsFunction(DCCallVM * vm, DCint mode) {
	if (argc != 1) {
		JSTSyntaxError("JSNative: jsnCallVMSetMode: expected 2 integer arguments: (JSNative.CallVM) pointer, JSNativeCallVMPrototype) mode");
		return RtJS(undefined);
	}
	dcMode(JSTPointer(JSTParam(1)), JSTInteger(JSTParam(2))); return RtJS(undefined);
}

static JSValueRef jsnCallVMReset JSToolsFunction(DCCallVM * vm , DCint mode) {
	if (argc != 2) {
		JSTSyntaxError("JSNative: jsnCallVMReset: expected 2 integer arguments: (JSNative.CallVM) pointer, (CallVMPrototype) mode");
		return RtJS(undefined);
	}
	dcReset(JSTPointer(JSTParam(1))); return RtJS(undefined);
}

static JSValueRef jsnLoadLibrary JSToolsFunction(const char * libpath) {
	if (argc != 1) {
		JSTSyntaxError("JSNative: jsnLoadLibrary: expected 1 integer argument: (char *) libPath");
		return RtJS(undefined);
	}
	char * buffer = JSTGetValueBuffer(JSTParam(1), NULL);
	JSValueRef result = JSNativeMakeAddress(dlLoadLibrary(buffer));
	JSTFreeBuffer(buffer);
	return result;
}

static JSValueRef jsnFreeLibrary JSToolsFunction(void * libhandle) {
	if (argc != 1) {
		JSTSyntaxError("JSNative: jsnFreeLibrary: expected 1 integer argument: (void *) libHandle");
		return RtJS(undefined);
	}
	dlFreeLibrary(JSTPointer(JSTParam(1))); return RtJS(undefined);
}

static JSValueRef jsnFindSymbol JSToolsFunction(void * libhandle , const char * symbol ) {
	if (argc != 2) {
		JSTSyntaxError("JSNative: jsnFindSymbol: expected 2 integer arguments: (void *) libHandle, (char *) symbol");
		return RtJS(undefined);
	}
	char * buffer = JSTGetValueBuffer(JSTParam(2), NULL);
	JSValueRef result = JSNativeMakeAddress(dlFindSymbol(JSTPointer(JSTParam(1)), buffer));
	JSTFreeBuffer(buffer);
	return result;
}

void js_native_init JSToolsProcedure (int argc, char *argv[], char *envp[]) {

	JSObjectRef global = JSTGetGlobalObject();

	JSTLoadRuntime(global, argc, argv, envp);

	/* our root object */
	JSTSetProperty(global, "JSNative", (RtJSNative = JSTCreateClassObject(NULL, NULL)), JSTPropertyConst);

	JSTSetPropertyFunction(RtJSNative, "jsnNewCallVM", &jsnNewCallVM);
	JSTSetPropertyFunction(RtJSNative, "jsnCallVMFree", &jsnCallVMFree);
	JSTSetPropertyFunction(RtJSNative, "jsnCallVMGetError", &jsnCallVMGetError);
	JSTSetPropertyFunction(RtJSNative, "jsnCallVMSetMode", &jsnCallVMSetMode);
	JSTSetPropertyFunction(RtJSNative, "jsnCallVMReset", &jsnCallVMReset);
	JSTSetPropertyFunction(RtJSNative, "jsnNewCallVM", &jsnNewCallVM);
	JSTSetPropertyFunction(RtJSNative, "jsnLoadLibrary", &jsnLoadLibrary);
	JSTSetPropertyFunction(RtJSNative, "jsnFreeLibrary", &jsnFreeLibrary);
	JSTSetPropertyFunction(RtJSNative, "jsnFindSymbol", &jsnFindSymbol);

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

