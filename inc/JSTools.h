/* Shogun */

#include "JSToolsShared.h"

#ifndef JSTools
#define JSTools true

extern JSTGlobalRuntime JSTRuntime;

// You must call this once, typically from main()
#define JSTCacheRuntime() _JSTCacheRuntime(ctx, exception)

#endif
