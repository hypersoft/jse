#include <stdarg.h>

#include "glib.inc"
#include "JavaScriptCore.inc"

#define JSTNativeProcedure(...) 	(JSContextRef ctx, ##__VA_ARGS__, JSValueRef * exception)
#define JSTFastNativeProcedure(...)	(register JSContextRef ctx, ##__VA_ARGS__, register JSValueRef * exception)
#define JSTNativeCall(NAME, ...)	NAME (ctx, ##__VA_ARGS__, exception)
#define JSTNativeFunction(...)		JSTNativeProcedure (JSObjectRef callee, JSObjectRef this, size_t argc, const JSValueRef argv[])
#define JSTCaughtException 			((bool) (exception) && (bool) (*exception))


void _JSTCacheRuntime(register JSContextRef ctx, register JSValueRef * exception);
JSStringRef _JSTCoreString(register char * BUFFER);
char * _JSTNativeString (register JSStringRef STR);
JSValueRef _JSTCallFunction (register JSContextRef ctx, JSValueRef * exception, JSObjectRef THIS, JSObjectRef FUNC, ...);
JSObjectRef _JSTCallConstructor (register JSContextRef ctx, JSValueRef * exception, JSObjectRef OBJ, ...);
bool JSTCoreRelease(register JSStringRef string);	// ...
bool JSTNativeRelease(register char * string);		// always returns true

typedef struct {
	JSObjectRef
		Global, isNaN, parseInt, parseFloat, escape, unescape, isFinite, decodeURI,
		encodeURI, decodeURIComponent, encodeURIComponent, Array, Boolean, Date,
		Error, Function, JSON, Math, Number, Object, RangeError, ReferenceError,
		RegExp, String, SyntaxError, TypeError, URIError;
	JSValueRef Infinity, NaN, undefined;
	JSStringRef classOf;
} JSTGlobalRuntime;


