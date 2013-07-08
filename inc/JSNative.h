#include "JSTools.h"

#ifndef JSNative
#define JSNative true
extern JSObjectRef RtJSNative;
void js_native_init JSToolsProcedure (JSObjectRef global);
#endif
