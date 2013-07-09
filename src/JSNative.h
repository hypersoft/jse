#include "JSTools.h"

#ifndef JSNative

#define JSNative true

#ifndef JSNativeSource
	extern JSObjectRef RtJSNative;
#endif

void js_native_init JSToolsProcedure (int argc, char *argv[], char *envp[]);

#endif


