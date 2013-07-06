#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <getopt.h>
#include <string.h>

#include "glib.inc"
#include "JavaScriptCore.inc"
#include "dyncall.inc"
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
	JSStringRef f;
	register i = 0; while (i < argc) {
		JSObjectSetPropertyAtIndex(
			MainContext, jsargv, i, JSValueMakeString(
				MainContext, (f = JSStringCreateWithUTF8CString(argv[i]))
			), NULL
		); 
		JSStringRelease(f); i++;
	};
	return jsargv;
}

static JSValueRef jse_fn_puts JSTNativeFunction () {
	if (argc)
	return JSValueMakeNumber(ctx, (double) puts(JSTNativeGetString(JSTParam(1))));
}

int main(int argc, char *argv[], char *envp[]) {

	// we will be using main context group for the duration of runtime.
	MainContextGroup = JSContextGroupRetain(JSContextGroupCreate());

	JSContextRef ctx = (MainContext = jse_create_context(NULL));
	JSValueRef e = NULL;
	JSValueRef * exception = &e;

	JSTCacheRuntime();

	JSStringRef temp = JSTCoreString("puts");
	JSTSetCoreValue(RtJS(Global), temp, JSTCoreMakeFunction(temp, &jse_fn_puts), JSTPropertyConst);
	JSTSetValue(RtJS(Global), "argv", jse_convert_argv(MainContext, argc, argv), JSTPropertyConst);

	js_native_init(ctx, RtJS(Global), exception);

	JSTRunScript(argv[1]);

	if (JSTCaughtException) {
		fprintf(stderr, "%s: Fatal Script Error: \nLine %i: %s\n",
			argv[1], JSTInteger(JSTGet(RtJSException, "line")),
			JSTNativeGetString(*exception)
		);
	}

}
