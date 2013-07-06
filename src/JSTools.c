
#include "JSToolsShared.h"

JSTGlobalRuntime JSTRuntime;

char * _JSTSeekLineEnding(register char * BUFFER) {
	if (BUFFER) do BUFFER++; while (*BUFFER && *BUFFER != 10);
	return BUFFER;
}

void _JSTCacheRuntime(register JSContextRef ctx, register JSValueRef * exception) {

	register JSObjectRef global = (JSTRuntime.Global = JSContextGetGlobalObject(ctx));

#define mkObjLnk(V) JSTRuntime.V = JSTGetObject(global, #V)
	mkObjLnk(isNaN); mkObjLnk(parseInt); mkObjLnk(parseFloat); mkObjLnk(escape); 
	mkObjLnk(isFinite); mkObjLnk(decodeURI); mkObjLnk(encodeURI); 
	mkObjLnk(decodeURIComponent); mkObjLnk(encodeURIComponent); 
	mkObjLnk(Array); mkObjLnk(Boolean); mkObjLnk(Date); mkObjLnk(Error);
	mkObjLnk(Function); mkObjLnk(JSON); mkObjLnk(Math); mkObjLnk(Number);
	mkObjLnk(Object); mkObjLnk(RangeError); mkObjLnk(ReferenceError);
	mkObjLnk(RegExp); mkObjLnk(String); mkObjLnk(SyntaxError); mkObjLnk(TypeError);
	mkObjLnk(URIError);

#define mkValLnk(V) JSTRuntime.V = JSTGet(global, #V)
	mkValLnk(Infinity); mkValLnk(NaN); mkValLnk(undefined);

#define mkStrLnk(NAM, VAL) JSTRuntime.NAM = JSTCoreString(VAL)
	mkStrLnk(classOf, "Object.prototype.toString.call(this).slice(8,-1)");

}

bool JSTCoreRelease(register JSStringRef s) { if (s) JSStringRelease(s); return true; }
bool JSTNativeRelease(register char * s) { if (s) g_free(s); return true; }

JSStringRef _JSTCoreString(register char * BUFFER) {
	static JSStringRef buffer; if (buffer) { JSStringRelease(buffer); buffer = NULL; }
	return (buffer = JSStringCreateWithUTF8CString(BUFFER));
}

JSValueRef _JSTLoadString JSTNativeProcedure (JSValueRef file) {
	char * content = NULL; g_file_get_contents(JSTNativeGetString(file), &content, NULL, NULL);
	JSValueRef val = JSTNativeMakeString(content);
	JSTNativeMakeString("free");
	return val;
}

JSStringRef _JSTCoreLoadString JSTNativeProcedure (JSStringRef file) {
	char * content = NULL; g_file_get_contents(JSTNativeString(file), &content, NULL, NULL);
	JSStringRef val = JSTCoreString(content);
	g_free(content); JSTEval("", NULL);
	return val; // must be freed
}

char * _JSTNativeLoadString JSTNativeProcedure (char * file) {
	char * content = NULL; g_file_get_contents(file, &content, NULL, NULL);
	return content; // must be freed
}

char * _JSTNativeString (register JSStringRef STR) {
	static char * buffer; if (buffer) { g_free(buffer); buffer = NULL; }
	register unsigned long size = JSStringGetMaximumUTF8CStringSize(STR);
	JSStringGetUTF8CString(STR, (buffer = g_new(char, size)), size);
	return buffer;
}

JSValueRef _JSTCallFunction (register JSContextRef ctx, JSValueRef * exception, JSObjectRef THIS, JSObjectRef FUNC, ...) {
 if ( ! THIS ) THIS = (JSTRuntime.Global);
 if ( ! (_Bool) ( FUNC && ! ((_Bool) (JSValueIsNull(ctx, (JSValueRef)FUNC))) && ! ((_Bool) (JSValueIsUndefined(ctx, (JSValueRef)FUNC))) ) || ! (_Bool) (JSObjectIsFunction(ctx, FUNC))) {
  return JSValueMakeUndefined(ctx);
 }
	register int argc = 0, argi = 0; va_list ap; JSValueRef arg = NULL;
	va_start(ap, FUNC);
		while ((arg = (JSValueRef) va_arg(ap, JSValueRef))) argc++;
	va_end(ap);
	if (argc) {
	JSValueRef argv[argc+1];
	argv[argc] = NULL;
	va_start(ap, FUNC);
		while (argi <= argc) argv[argi++] = va_arg(ap, JSValueRef);
	va_end(ap);
	return JSObjectCallAsFunction(ctx, FUNC, THIS, argc, argv, exception);
	} else {
	return JSObjectCallAsFunction(ctx, FUNC, THIS, 0, NULL, exception);
	}
}

JSObjectRef _JSTCallConstructor(register JSContextRef ctx, JSValueRef * exception, JSObjectRef OBJ, ...) {
 if ( ! (_Bool) ( OBJ && ! ((_Bool) (JSValueIsNull(ctx, (JSValueRef)OBJ))) && ! ((_Bool) (JSValueIsUndefined(ctx, (JSValueRef)OBJ))) ) || ! JSObjectIsConstructor(ctx, OBJ)) {
  return (JSObjectRef) (JSValueMakeUndefined(ctx));
 }
	register int argc = 0, argi = 0; va_list ap; JSValueRef arg = NULL;
	va_start(ap, OBJ);
		while ((arg = (JSValueRef) va_arg(ap, JSValueRef))) argc++;
	va_end(ap);
	if (argc) {
	JSValueRef argv[argc+1];
	argv[argc] = NULL;
	va_start(ap, OBJ);
		while (argi <= argc) argv[argi++] = va_arg(ap, JSValueRef);
	va_end(ap);
	return JSObjectCallAsConstructor(ctx, OBJ, argc, argv, exception);
	} else {
	return JSObjectCallAsConstructor(ctx, OBJ, 0, NULL, exception);
	}
}

JSValueRef _JSTRunScript JSTNativeProcedure (char * file) {
	char * script = JSTNativeLoadString(file);
	JSValueRef val = JSTEval(JSTSheBang(script), NULL); JSTNativeRelease(script);
	if (JSTCaughtException) JSTSetValue((JSObjectRef)*exception, "file", JSTNativeMakeString(file), JSTPropertyConst);
	return val;
}

