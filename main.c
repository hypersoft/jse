#include "JSNative.h"

int main(int argc, char *argv[], char *envp[]) {

	// we will be using main context group for the duration of runtime.
	JSContextGroupRef cgroup = JSContextGroupRetain(JSContextGroupCreate());
	JSContextRef ctx = JSGlobalContextRetain(JSGlobalContextCreateInGroup(cgroup, NULL));
	JSValueRef e = NULL;
	JSValueRef * exception = &e;

	JSToolsCall(js_native_init, argc, argv, envp);

	if (argc && JSTBoolean(JSTEval("canReadFile(this);", JSTMakeBufferValue(argv[1])))) {
		int result = JSTInteger(JSTRunScript(argv[1], RtJS(Global)));
		if (JSTCaughtException) JSTReportFatalException(1);
		return result;
	}

	if (JSTCaughtException) JSTReportFatalException(1);

	return 0;

}

