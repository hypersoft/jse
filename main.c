#include "JSNative.h"

int main(int argc, char *argv[], char *envp[]) {

	// we will be using main context group for the duration of runtime.
	JSContextGroupRef cgroup = JSContextGroupRetain(JSContextGroupCreate());
	JSContextRef ctx = JSGlobalContextRetain(JSGlobalContextCreateInGroup(cgroup, NULL));
	JSValueRef e = NULL;
	JSValueRef * exception = &e;

	js_native_init(ctx, argc, argv, envp, exception);

	if (e) {
		// native init exception...
	}

	if (argc) JSTRunScript(argv[1], RtJS(Global));

	if (e) {
		// script exception...
	}

}
