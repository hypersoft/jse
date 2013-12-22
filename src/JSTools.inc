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
#include <sys/stat.h>
#include <fcntl.h>

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
#include "JSTools/String.inc"
#include "JSTools/Function.inc"
#include "JSTools/Object.inc"
#include "JSTools/Value.inc"
#include "JSTools/Script.inc"

#endif

#ifdef JSTools_h

extern JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]);
extern char * JSTConstructUTF8(char * format, ...);

#else

const char * JSTReservedAddress =

	"Hypersoft Systems JSE Copyright 2013 Triston J. Taylor, All Rights Reserved.";

const char * CODENAME = JSE_CODENAME;
const char * VERSION = JSE_BUILDNO;
const char * VENDOR = JSE_VENDOR;

char * JSTConstructUTF8(char * format, ...) {
	va_list ap; va_start(ap, format); char * message = NULL;
	vasprintf(&message, format, ap); return message;
}

#endif

