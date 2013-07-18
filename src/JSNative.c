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
	if (argc != 2) {
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

static JSValueRef jsnArgBool JSToolsFunction(DCCallVM * vm, bool arg) {
	dcArgBool(JSTPointer(JSTParam(1)), JSTBoolean(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnArgChar JSToolsFunction(DCCallVM * vm, char arg) {
	dcArgChar(JSTPointer(JSTParam(1)), JSTChar(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnArgShort JSToolsFunction(DCCallVM * vm, short arg) {
	dcArgShort(JSTPointer(JSTParam(1)), JSTShort(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnArgInt JSToolsFunction(DCCallVM * vm, int arg) {
	dcArgInt(JSTPointer(JSTParam(1)), JSTInteger(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnArgLong JSToolsFunction(DCCallVM * vm, long arg) {
	dcArgLong(JSTPointer(JSTParam(1)), JSTLong(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnArgLongLong JSToolsFunction(DCCallVM * vm, long long arg) {
	dcArgLongLong(JSTPointer(JSTParam(1)), JSTLongLong(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnArgFloat JSToolsFunction(DCCallVM * vm, float arg) {
printf("val: %g\n", JSTFloat(JSTParam(2)));
	dcArgFloat(JSTPointer(JSTParam(1)), JSTFloat(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnArgDouble JSToolsFunction(DCCallVM * vm, double arg) {
	dcArgDouble(JSTPointer(JSTParam(1)), JSTDouble(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnArgPointer JSToolsFunction(DCCallVM * vm, void * arg) {
	dcArgPointer(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnCallVoid JSToolsFunction (DCCallVM * vm, void * func) {
	dcCallVoid(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2)));
	return RtJS(undefined);
}

static JSValueRef jsnCallBool JSToolsFunction (DCCallVM * vm, void * func) {
	return JSTMakeBoolean(dcCallBool(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2))));
}

static JSValueRef jsnCallChar JSToolsFunction (DCCallVM * vm, void * func) {
	return JSTMakeNumber(dcCallChar(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2))));
}

static JSValueRef jsnCallShort JSToolsFunction (DCCallVM * vm, void * func) {
	return JSTMakeNumber(dcCallShort(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2))));
}

static JSValueRef jsnCallInt JSToolsFunction (DCCallVM * vm, void * func) {
	return JSTMakeNumber(dcCallInt(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2))));
}

static JSValueRef jsnCallLong JSToolsFunction (DCCallVM * vm, void * func) {
	return JSTMakeNumber(dcCallLong(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2))));
}

static JSValueRef jsnCallLongLong JSToolsFunction (DCCallVM * vm, void * func) {
	return JSTMakeNumber(dcCallLongLong(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2))));
}

static JSValueRef jsnCallFloat JSToolsFunction (DCCallVM * vm, void * func) {
	return JSTMakeNumber(dcCallFloat(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2))));
}

static JSValueRef jsnCallDouble JSToolsFunction (DCCallVM * vm, void * func) {
	return JSTMakeNumber(dcCallDouble(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2))));
}

static JSValueRef jsnCallPointer JSToolsFunction (DCCallVM * vm, void * func) {
	return JSNativeMakeAddress(dcCallPointer(JSTPointer(JSTParam(1)), JSTPointer(JSTParam(2))));
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
	JSTSetPropertyFunction(RtJSNative, "jsnLoadLibrary", &jsnLoadLibrary);
	JSTSetPropertyFunction(RtJSNative, "jsnFreeLibrary", &jsnFreeLibrary);
	JSTSetPropertyFunction(RtJSNative, "jsnFindSymbol", &jsnFindSymbol);

	JSTSetPropertyFunction(RtJSNative, "jsnArgBool", &jsnArgBool);
	JSTSetPropertyFunction(RtJSNative, "jsnArgChar", &jsnArgChar);
	JSTSetPropertyFunction(RtJSNative, "jsnArgShort", &jsnArgShort);
	JSTSetPropertyFunction(RtJSNative, "jsnArgInt", &jsnArgInt);
	JSTSetPropertyFunction(RtJSNative, "jsnArgLong", &jsnArgLong);
	JSTSetPropertyFunction(RtJSNative, "jsnArgLongLong", &jsnArgLongLong);
	JSTSetPropertyFunction(RtJSNative, "jsnArgFloat", &jsnArgFloat);
	JSTSetPropertyFunction(RtJSNative, "jsnArgDouble", &jsnArgDouble);
	JSTSetPropertyFunction(RtJSNative, "jsnArgPointer", &jsnArgPointer);

	JSTSetPropertyFunction(RtJSNative, "jsnCallVoid", &jsnCallVoid);
	JSTSetPropertyFunction(RtJSNative, "jsnCallBool", &jsnCallBool);
	JSTSetPropertyFunction(RtJSNative, "jsnCallChar", &jsnCallChar);
	JSTSetPropertyFunction(RtJSNative, "jsnCallShort", &jsnCallShort);
	JSTSetPropertyFunction(RtJSNative, "jsnCallInt", &jsnCallInt);
	JSTSetPropertyFunction(RtJSNative, "jsnCallLong", &jsnCallLong);
	JSTSetPropertyFunction(RtJSNative, "jsnCallLongLong", &jsnCallLongLong);
	JSTSetPropertyFunction(RtJSNative, "jsnCallFloat", &jsnCallFloat);
	JSTSetPropertyFunction(RtJSNative, "jsnCallDouble", &jsnCallDouble);
	JSTSetPropertyFunction(RtJSNative, "jsnCallPointer", &jsnCallPointer);

	JSToolsCall(js_native_type_init, RtJSNative);
	JSToolsCall(js_native_address_init, RtJSNative);
	JSToolsCall(js_native_allocator_init, RtJSNative);
	JSToolsCall(js_native_value_init, RtJSNative);

	JSTEvalScript(JSNativeSupport, global, "JSNativeSupport");
	if (JSTCaughtException) JSTReportFatalException(1);

}

