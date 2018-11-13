/*
 * File:   jse.h
 * Author: triston
 *
 * Created on December 26, 2014, 5:07 AM
 * PUBLIC API
 */

#ifndef JSE_H
#define	JSE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/stat.h>
#include <fcntl.h>

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "dyncall/dyncall.h"
#include "dyncall/dynload.h"
#include "dyncall/dyncall_callback.h"

#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <JavaScriptCore/JavaScript.h>

#include <inttypes.h>

#define JSDefineConstant(CTX, O, V)	JSObjectSetUtf8Property(CTX, O, #V, JSValueFromNumber(CTX, V), kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete)

typedef enum JseOptionType {
	OPT_NONE = 0,
	OPT_MINUS = 1,
	OPT_PLUS = 2,
	OPT_LONG = 4,
	OPT_ARG = 8,
	OPT_WORD = 16,
	OPT_COMBO = 32,
	OPT_UNIFIED = (OPT_PLUS | OPT_MINUS | OPT_LONG)
} JseOptionType;

typedef struct JseOption {
	JseOptionType flag;
	char shortName;
	char * longName;
} JseOption;

typedef GError * (*JseOptionCallback)(
	JseOptionType flag,
	JseOption * option,
	const char * value
);

typedef struct JseOptionParseResult {
	GError * error;
	int argi, argc;
	char * argv[];
} JseOptionParseResult;

void jse_option_parse_result_free(JseOptionParseResult * result);
JseOptionParseResult * jse_option_parse_main(int argc, char ** argv, JseOption option[], JseOptionCallback handler);

typedef JSValueRef JSValue;
typedef JSObjectRef JSObject;
typedef JSStringRef JSString;
typedef JSContextGroupRef JSUniverse;
typedef JSGlobalContextRef JSContext;
typedef JSClassRef JSClass;
#define JSClassDefinitionEmpty kJSClassDefinitionEmpty
typedef JSPropertyNameAccumulatorRef JSPropertyNameAccumulator;

JSUniverse JSContextGetUniverse();

typedef JSValue (*JSFunction) (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception);
typedef JSObject (*JSConstructor) (JSContext ctx, JSObject this, size_t argc, const JSValue argv[], JSValue * exception);
JSObject JSConstructorCreate(JSContext ctx, char * name, JSClass class, JSFunction function);
JSObject JSConstructorGetPrototype(JSContext ctx, JSObject constructor, JSValue * exception);

static char JSELOCALBUFFER[2048];

JSValue JSEvaluateUtf8(JSContext ctx, char * script, JSObject object, char * file, int line, JSValue * exception);
#define JSInlineEval(ctx, script, object, exception) JSEvaluateUtf8(ctx, script, object, (sprintf(JSELOCALBUFFER, "%s[%i]", __FILE__, __LINE__), JSELOCALBUFFER), 1, exception)

void JSTerminate(unsigned status);

char * JSStringToUtf8(JSString string);
#define JSStringFromUtf8 JSStringCreateWithUTF8CString
#define JSStringUtf8Size JSStringGetMaximumUTF8CStringSize

#define JSValueFromNumber(context, double) JSValueMakeNumber(context, double)
JSValue JSValueFromUtf8(JSContext ctx, char * string);
char * JSValueToUtf8(JSContext c, JSValue v);
JSString JSValueToString(JSContext c, JSValue v, JSValue * e);


bool JSObjectIsFrozen(JSContext ctx, JSObject object);
JSObject JSObjectCreateFunction(JSContext ctx, JSObject this, char * name, void * callback);
JSObject JSObjectCreateConstructor(JSContext ctx, JSClass class, JSObject (*constructor)(JSContext ctx, JSObject constructor, size_t argc, const JSValue argv[], JSValue* exception));
bool JSObjectHasUtf8Property(JSContext ctx, JSObject object, char * property);
JSValue JSObjectGetUtf8Property(JSContext ctx, JSObject object, char * property);
bool JSObjectSetUtf8Property(JSContext ctx, JSObject object, char * property, JSValue value, JSPropertyAttributes attributes);
bool JSObjectDeleteUtf8Property(JSContext ctx, JSObject object, char * property);
bool JSObjectModifyUtf8Property(JSContext ctx, JSObject object, char * property, JSPropertyAttributes attribs, JSValue * exception);

GQuark JSErrorQuark;
#define JSExceptionCaught(e) (e && *e)
JSValue JSExceptionFromUtf8(JSContext ctx, char * globalConstructorName, char * fmt, ...);
JSValue JSExceptionThrowUtf8(JSContext ctx, char * globalConstructorName, JSValue * exception, char * fmt, ...);
JSValue JSExceptionFromGError(JSContext ctx, GError * error);
gchar * JSExceptionToUtf8 (JSContext ctx, JSValue e);
GError * JSExceptionToGError(JSContext ctx, JSValue exception);
void JSReportException(JSContext ctx, char * command, JSValue exception);
void JSReportError(char * command, GError * error);

void JSInit(char * command, JSContext ctx, bool secureMode);
void JSInitScriptArguments(JSContext ctx, int argc, char ** argv);
void JSAddPluginPath(char * path);
JSValue JSLoadPlugin(JSContext ctx, char * plugin, JSObject object, JSValue * error);
gboolean JSUnloading();

#ifdef	__cplusplus
}
#endif

#endif	/* JSE_H */

