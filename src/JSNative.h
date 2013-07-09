#include "JSTools.h"

#ifndef JSNative

#define JSNative JSNative

#ifndef JSNativeSource
	extern JSObjectRef RtJSNative;
#endif

void js_native_init JSToolsProcedure (int argc, char *argv[], char *envp[]);

#include "JSNativeType.h"
#include "JSNativeAddress.h"
#include "JSNativeAllocator.h"

#endif


