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

#include "UTF.h"

#include "JavaScriptCore.inc"
#include "glib.inc"
#include "dyncall.inc"

#include "JSTools/Base.inc"
#include "JSTools/Class.inc"
#include "JSTools/Constructor.inc"
#include "JSTools/Context.inc"
#include "JSTools/Declare.inc"
#include "JSTools/Function.inc"
#include "JSTools/Object.inc"
#include "JSTools/Stack.inc"
#include "JSTools/String.inc"
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
extern bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p);
extern void JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a);
extern JSTValue JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p);
extern bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p);
extern JSTValue JSTValueFromJSON_ JSTUtility(char * p);
extern char * JSTConstructUTF8(char * format, ...);

#endif


