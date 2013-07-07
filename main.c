#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <getopt.h>
#include <string.h>

#include "JSNative.h"

JSContextGroupRef MainContextGroup;
JSGlobalContextRef	MainContext;


JSGlobalContextRef jse_create_context(JSClassRef globalObjectClass) {
	return JSGlobalContextRetain(JSGlobalContextCreateInGroup(MainContextGroup, globalObjectClass));
}

void jse_destroy_context(JSGlobalContextRef context) {
	JSGlobalContextRelease(context);
}

JSObjectRef jse_convert_argv(JSContextRef ctx, int argc, char *argv[]) {
	void * jsargv = JSObjectMakeArray(MainContext, 0, NULL, NULL);
	register i = 0; while (i < argc)
	JSObjectSetPropertyAtIndex(ctx, jsargv, i, JSTMakeBufferValue(argv[i++]), NULL); 
	return jsargv;
}

int main(int argc, char *argv[], char *envp[]) {

	// we will be using main context group for the duration of runtime.
	MainContextGroup = JSContextGroupRetain(JSContextGroupCreate());

	JSContextRef ctx = (MainContext = jse_create_context(NULL));
	JSValueRef e = NULL;
	JSValueRef * exception = &e;

	JSTLoadRuntime();

	JSTPropertyMaster;

	JSTSetProperty(RtJS(Global), "argv", jse_convert_argv(ctx, argc, argv), JSTPropertyConst);

//	js_native_init(ctx, RtJS(Global), exception);

	if (argc) JSTCall(RtJS(Global), "loadScript", JSTMakeBufferValue(argv[1]));


}
