/* Shogun */

#ifndef JSTools

#define JSTools true
#include "JSToolsShared.h"

extern JSTGlobalRuntime JSTRuntime;

extern long JSTBufferLen;
extern char * JSTBufferRef;
extern JSObjectRef JSTObjectRef;
extern JSStringRef JSTStringRef;
extern JSValueRef JSTValueRef;

// You must call this once, typically from main()
#define JSTLoadRuntime() _JSTLoadRuntime(ctx, exception)

#endif
