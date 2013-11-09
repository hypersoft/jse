#ifndef JST

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <unistd.h>

#include <errno.h>

#include <string.h>
#include <wchar.h>
#include <uchar.h>
#include <pwd.h>
#include <signal.h>

typedef char32_t UTF32;
typedef char16_t UTF16;
typedef char UTF8;

#include "JavaScriptCore.inc"
#include "glib.inc"
#include "dyncall.inc"

#include "JSTools/Base.inc"
#include "JSTools/Class.inc"
#include "JSTools/Constructor.inc"
#include "JSTools/Context.inc"
#include "JSTools/Declare.inc"
#include "JSTools/Function.inc"
#include "JSTools/String.inc"
#include "JSTools/Object.inc"
#include "JSTools/Value.inc"
#include "JSTools/Script.inc"

#endif

#ifdef JSTools_h

extern const void * JSTReservedAddress;

extern JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]);
extern JSTObject JSTNativeInit_ JSTUtility(JSTObject js);
extern JSTValue JSTFunctionCall_(JSTContext ctx, JSTValue *exception, JSTObject method, JSTObject object, ...);
extern JSTObject JSTFunctionCallback_ JSTUtility(char * p, void * f);
extern JSTValue JSTValueFromString_ JSTUtility(JSTString s, bool release);
extern JSTValue JSTValueFromJSON_ JSTUtility(char * p);
extern char * JSTConstructUTF8(char * format, ...);

#endif


