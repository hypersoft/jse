/*

Hypersoft Systems JST AND Hypersoft System JSE Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list 
   of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this 
   list of conditions and the following disclaimer in the documentation and/or other 
   materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGE.

*/

#include "jst.h"

#include "license.h"
#include "notice.h"

const char * JSTEvalNaN = "isNaN(this)";
const char * JSTEvalInt = "parseInt(this)";

const char * JSTReservedAddress =

	"Hypersoft Systems JSE Copyright 2013 Triston J. Taylor, All Rights Reserved.";

const char * CODENAME = JSE_CODENAME;
const char * VERSION = JSE_BUILDNO;
const char * VENDOR = JSE_VENDOR;

/* strict argument to integer conversion */
bool JSTArgumentToInt_ JSTUtility(int argc, JSTValue argv[], int bits, int index, void * dest) {

	/*
	   we don't worry about sign, just that the data will fit into the destination !
	   if this function returns false, an error has been set, clean up and return 
	*/

	JSTValue value;
	if (bits != 8 && bits != 16 && bits != 32 && bits != 64) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert argument %i to %i-bit integer: %i-bit integer is not a supported type",
		index, bits, bits
	); else	if (index >= argc) return JSTScriptNativeError(JST_TYPE_ERROR,
		"cannot convert argument %i to %i-bit integer: argument is undefined", index, bits
	); else if (dest == NULL) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"cannot convert argument %i to %i-bit integer: destination is NULL", index, bits
	);

	// first we convert to JavaScript integer which should be an int64
	value = JSTValueParseInt(argv[index]);
	// then we check for NaN
	if (JSTValueIsNaN(value)) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert argument %i to integer: value is not a number", index
	);

	// now we convert to native double
	double integer = JSTValueToDouble(value);

	if (bits == 8) { // char first, most likely repetitive conversion
		if (integer < INT8_MIN) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is less than %i",
			index, 8, integer, INT8_MIN
		); else if (integer > INT8_MAX)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is greater than %i",
			index, 8, integer, INT8_MAX
		);
		*(int8_t*) dest = (int8_t) integer;
	} else if (bits == 32) { // then long, most likely conversion
		if (integer < INT32_MIN) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is less than %i",
			index, 32, integer, INT32_MIN
		); else if (integer > INT32_MAX)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is greater than %i",
			index, 32, integer, INT32_MAX
		);
		*(int32_t*) dest = (int32_t) integer;
	} else if (bits == 16) {  // then short, least likely conversion
		if (integer < INT16_MIN) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is less than %i",
			index, 16, integer, INT16_MIN
		); else if (integer > INT16_MAX)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is greater than %i",
			index, 16, integer, INT16_MAX
		);
		*(int16_t*) dest = (int16_t) integer;
	} else if (bits == 64) { // then this, which is the "fastest conversion"
		// NOTE: long long has more integer bits than double
		// double can only hold 52 bits of integer (2^53)!
		*(int64_t*) dest = (int64_t) integer;
	}
	return true;
}

/* strict value to integer conversion */
bool JSTValueToInt_ JSTUtility(int bits, JSTValue input, void * dest) {

	/*
	   we don't worry about sign, just that the data will fit into the destination !
	   if this function returns false, an error has been set, clean up and return 
	*/

	JSTValue value;
	if (bits != 8 && bits != 16 && bits != 32 && bits != 64) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert value to %i-bit integer: %i-bit integer is not a supported type",
		bits, bits
	); else if (dest == NULL) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"cannot convert value to %i-bit integer: destination is NULL", bits
	);

	// first we convert to JavaScript integer which should be an int64
	value = JSTValueParseInt(input);
	// then we check for NaN
	if (JSTValueIsNaN(value)) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert value to %i-bit integer: input is not a number", bits
	);

	// now we convert to native double
	double integer = JSTValueToDouble(value);

	if (bits == 8) { // char first, most likely repetitive conversion
		if (integer < INT8_MIN) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is less than %i",
			8, integer, INT8_MIN
		); else if (integer > INT8_MAX)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is greater than %i",
			8, integer, INT8_MAX
		);
		*(int8_t*) dest = (int8_t) integer;
	} else if (bits == 32) { // then long, most likely conversion
		if (integer < INT32_MIN) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is less than %i",
			32, integer, INT32_MIN
		); else if (integer > INT32_MAX)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is greater than %i",
			32, integer, INT32_MAX
		);
		*(int32_t*) dest = (int32_t) integer;
	} else if (bits == 16) { // then short, least likely conversion
		if (integer < INT16_MIN) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is less than %i",
			16, integer, INT16_MIN
		); else if (integer > INT16_MAX)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is greater than %i",
			16, integer, INT16_MAX
		);
		*(int16_t*) dest = (int32_t) integer;
	} else if (bits == 64) { // then long long, which is the fastest conversion
		// NOTE: long long has more integer bits than double
		// double can only hold 52 bits of integer (2^53)!
		*(int64_t*) dest = (int64_t) integer;
	}
	return true;
}

/* strict argument to pointer conversion */
bool JSTArgumentToPointer_ JSTUtility(int argc, JSTValue argv[], int index, void * dest) {

	/* if this function returns false, an error has been set, clean up and return */

	JSTValue value;
	if (index >= argc) return JSTScriptNativeError(JST_TYPE_ERROR,
		"cannot convert argument %i to pointer: argument is undefined", index
	); else if (dest == NULL) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"cannot convert argument %i to pointer: destination is NULL", index
	);

	// first we convert to JavaScript integer which should be an int64
	value = JSTValueParseInt(argv[index]);
	// then we check for NaN
	if (JSTValueIsNaN(value)) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert argument %i to pointer: value is not a number", index
	);

	// now we convert to native double
	double integer = JSTValueToDouble(value);

	if (integer < 0) return JSTScriptNativeError(JST_RANGE_ERROR,
		"cannot convert argument %i to pointer: %.0g is less than 0",
		index, integer
	); else if (integer > UINTPTR_MAX)  return JSTScriptNativeError(JST_RANGE_ERROR,
		"cannot convert argument %i to pointer: %.0g is greater than %u",
		index, integer, UINTPTR_MAX
	);
	*(uintptr_t*) dest = (uintptr_t) integer;

	return true;
}

/* strict value to pointer */
bool JSTValueToPointer_ JSTUtility(JSTValue input, void * dest) {

	/* if this function returns false, an error has been set, clean up and return */

	JSTValue value;
	if (dest == NULL) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"cannot convert value to pointer: destination is NULL"
	);

	// first we convert to JavaScript integer which should be an int64
	value = JSTValueParseInt(input);
	// then we check for NaN
	if (JSTValueIsNaN(value)) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert value to pointer: input is not a number"
	);

	// now we convert to native double
	double integer = JSTValueToDouble(value);

	if (integer < 0) return JSTScriptNativeError(JST_RANGE_ERROR,
		"cannot convert value to pointer: %.0g is less than 0", integer
	); else if (integer > UINTPTR_MAX)  return JSTScriptNativeError(JST_RANGE_ERROR,
		"cannot convert value to pointer: %.0g is greater than %u",
		integer, UINTPTR_MAX
	);
	*(uintptr_t*) dest = (uintptr_t) integer;

	return true;
}

JSTObject JSTConstructorCall_(register JSTContext ctx, JSTValue *exception, JSTObject c, ...) {
	va_list ap; register size_t argc = 0, count = 0; va_start(ap, c); 
	while(va_arg(ap, void*) != JSTReservedAddress) argc++; va_start(ap, c);
	if (argc > 32) return JSTObjectUndefined; JSTValue argv[argc+1];
	while (count < argc) argv[count++] = va_arg(ap, JSTValue);
	argv[count] = NULL; return JSObjectCallAsConstructor(ctx, c, argc, argv, exception);
}

JSTObject JSTFunctionCallback_ JSTUtility(char * p, void * f) {
	JSTObject result = NULL;
	if (p && f) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSObjectMakeFunctionWithCallback, s, f);
		JSTStringRelease(s);
	}
	return result;
}

JSTValue JSTFunctionCall_(register JSTContext ctx, JSTValue *exception, JSTObject method, JSTObject object, ...) {
	va_list ap; register size_t argc = 0, count = 0; va_start(ap, object); 
	while(va_arg(ap, void*) != JSTReservedAddress) argc++; va_start(ap, object);
	if (argc > 32) return JSTValueUndefined; JSTValue argv[argc+1];
	while (count < argc) argv[count++] = va_arg(ap, JSTValue);
	argv[count] = NULL; return JSObjectCallAsFunction(ctx, method, object, argc, argv, exception);
}

JSTObject JSTObjectSetMethod_ JSTUtility(JSTObject o, char * n, void * m, int a) {
	JSTObject method = NULL;
	if (JSTValueIsObject(o)) {
		JSTString name = (n)?JSTStringFromUTF8(n):NULL; 
		if (m) JSObjectSetProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), name, (method = JSObjectMakeFunctionWithCallback(ctx, name, m)), a, exception); 
		JSTStringRelease(name); 
	}
	return method;
}

JSTObject JSTObjectSetConstructor_ JSTUtility(JSTObject o, char * n, JSTClass c, void * m, int a) {
	JSTObject constructor = NULL;
	if (JSTValueIsObject(o)) {
		JSTString name = (n)?JSTStringFromUTF8(n):NULL;
		JSObjectSetProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), name, (constructor =JSObjectMakeConstructor(ctx, c, m)), a, exception);
		JSTStringRelease(name);
	}
	return constructor;
}

bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSObjectDeleteProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), s, exception);
		JSTStringRelease(s);
	}
	return result;
}

void * JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p) {
	void * result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = (void*) JSObjectGetProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), s, exception);
		JSTStringRelease(s);
	}
	return result;
}

// Returns the value set as a void *
void * JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a) {
	if (p && v) {
		JSTString s = JSTStringFromUTF8(p);
		JSObjectSetProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), s, v, a, exception);
		JSTStringRelease(s);
	}
	return (void*)v;
}

bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSObjectHasProperty, (o)?o:JSContextGetGlobalObject(ctx), s);
		JSTStringRelease(s);
	}
	return result;
}

bool JSTScriptCheckSyntax_ JSTUtility(char *p1, char *p2, size_t i) {
	if (!p1 || !p2) return true;
	JSTString s[2] = {JSTStringFromUTF8(p1), JSTStringFromUTF8(p2)};
	bool result = JSCheckScriptSyntax(ctx, s[0], s[1], i, exception);
	JSTStringRelease(s[0]); JSTStringRelease(s[1]);
}

JSTValue JSTScriptEval_ JSTUtility(const char *p1, JSTObject o, const char * p2, size_t i) {
	if (!p1 || !p2) return NULL;
	JSTString s[2] = {JSTStringFromUTF8(p1), JSTStringFromUTF8(p2)};
	JSTValue result = JSEvaluateScript(ctx, s[0], (o)?o:JSContextGetGlobalObject(ctx), s[1], i, exception);
	JSTStringRelease(s[0]); JSTStringRelease(s[1]);
	return result;
}

void * JSTScriptNativeError_(register JSTContext ctx, register JSTValue *exception, const char * file, int line, int errorType, const char * format, ...) {
	va_list ap; va_start(ap, format); char * message = NULL;
	vasprintf(&message, format, ap);
	if (errorType == 2) JSTScriptEval("throw(this)", JSTScriptError(message), file, line);
	else if (errorType == 3) JSTScriptEval("throw(this)", JSTScriptSyntaxError(message), file, line);
	else if (errorType == 4) JSTScriptEval("throw(this)", JSTScriptTypeError(message), file, line);
	else if (errorType == 5) JSTScriptEval("throw(this)", JSTScriptRangeError(message), file, line);
	else if (errorType == 6) JSTScriptEval("throw(this)", JSTScriptReferenceError(message), file, line);
	else if (errorType == 7) JSTScriptEval("throw(this)", JSTScriptEvalError(message), file, line);
	else if (errorType == 8) JSTScriptEval("throw(this)", JSTScriptURIError(message), file, line);
	else JSTScriptEval("throw(this)", JSTScriptError(message), file, line);
	free(message);
	return NULL;
}

int JSTScriptReportException_(register JSTContext ctx, register JSTValue *exception) {

	JSTObject e = (JSTObject) *exception; *exception = NULL;

	/* properties of e:
		name
		code
		message
		line
		sourceURL
	*/

	char * message = JSTValueToUTF8(JSTObjectGetProperty(e, "message"));
	char * name = JSTValueToUTF8(JSTObjectGetProperty(e, "name"));
	char * url = JSTValueToUTF8(JSTObjectGetProperty(e, "sourceURL"));
	size_t line = JSTValueToDouble(JSTObjectGetProperty(e, "line"));

	fprintf(stderr, "JSE Fatal %s: %s%ssource %s: line %i\n", name, 
		message, (*message)?": ":"", url, line
	);

	free(message), free(url);

	if (!JSTValueIsVoid(JSTObjectGetProperty(e, "stack"))) {
		char * b = JSTValueToUTF8(JSTScriptNativeEval("sys.error.trace(this)", e));
		fprintf(stderr, "%s\n", b); free(b);
	}

	return JSTValueToDouble(JSTObjectGetProperty(e, "code"));

}

char * JSTStringToUTF8 (JSTString s, bool release) {
	if (!s) return NULL;
	register size_t bufferLength = 0; register void * buffer = NULL;
	if (s && (bufferLength = JSStringGetMaximumUTF8CStringSize(s)))
		JSStringGetUTF8CString(s, (buffer = malloc(bufferLength)), bufferLength);
	if (release) JSTStringRelease(s);
	return buffer;
}

UTF32 * JSTStringToUTF32(register JSTString jss, size_t len, bool release) {
	if (!jss || !len) return NULL;
	const gunichar2 * utf16 = JSTStringUTF16(jss);
	void * result = g_utf16_to_ucs4(utf16, len, NULL, NULL, NULL);
	if (release) JSTStringRelease(jss);
	return result;
}

JSTString JSTStringFromUTF32(register const gunichar *ucs4, size_t len, bool release) {
	if (!ucs4 || !len) return NULL;
	long bytes = 0; const gunichar2 * utf16 = g_ucs4_to_utf16(ucs4, len, &bytes, NULL, NULL);
	if (release) g_free((void*)ucs4);
	JSTString result = JSTStringFromUTF16(utf16, bytes); g_free((void*)utf16);
	return result;
}

JSTValue JSTValueFromString_ JSTUtility(JSTString s, bool release) {
	if (!s) return NULL;
	JSTValue result = JSValueMakeString(ctx, s);
	if (release) JSTStringRelease(s);
	return result;
}

JSTValue JSTValueFromJSON_ JSTUtility(char * p) {
	JSTValue result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSValueMakeFromJSONString(ctx, s);
		JSTStringRelease(s);
	}
	return result;
}

char * JSTConstructUTF8(char * format, ...) {
	if (!format) return NULL;
	va_list ap; va_start(ap, format); char * message = NULL;
	vasprintf(&message, format, ap); return message;
}

static JSValueRef jst_execute JSTDeclareFunction () {

	int child_status = 0, allocated = 0, deallocated = 0;
	gchar *exec_child_out = NULL, *exec_child_err = NULL; gint exec_child_status = 0;

	JSObjectRef exec = JSTObjectUndefined;

	bool captureOut = false, captureError = false, captureTime = false;

	captureTime = JSTObjectHasProperty(this, "time");
	captureOut = JSTObjectHasProperty(this, "output");
	captureError = JSTObjectHasProperty(this, "error");

	JSObjectRef oargv = (JSTObject) JSTObjectGetProperty(this, "argv");
	int oargc = JSTValueToDouble(JSTObjectGetProperty(oargv, "length"));

	if (JSTScriptHasError) { return JSTValueUndefined; }

	char * argument[oargc + argc + 1];
	char ** dest = argument;

	while (allocated < oargc) {
		JSTString tmp = JSTValueToString(JSTObjectGetPropertyAtIndex(oargv, allocated));
		argument[allocated] = JSTStringToUTF8(tmp, true);
		allocated++;
	}

	dest = argument + oargc; int secondary=0;

	while (secondary < argc) {
		if (JSTValueIsNull(argv[secondary])) {
			JSTScriptReportException();
			return NULL;
		}
		JSTString tmp = JSTValueToString(argv[secondary]);
		dest[secondary] = JSTStringToUTF8(tmp, true);
		secondary++;
	}

	dest[secondary] = NULL; allocated += secondary;
	if (JSTScriptHasError) { goto leaving; }

	exec = JSTClassInstance(NULL, NULL);
	if (captureTime) JSTScriptNativeEval("this.startTime = Object.freeze(new Date())", exec);
	if (g_spawn_sync(NULL, argument, NULL, G_SPAWN_LEAVE_DESCRIPTORS_OPEN | G_SPAWN_SEARCH_PATH | G_SPAWN_CHILD_INHERITS_STDIN, NULL, NULL, (captureOut) ? &exec_child_out : NULL, (captureError) ? &exec_child_err : NULL, &exec_child_status, NULL)) {
		exec_child_status = WEXITSTATUS(exec_child_status);
		JSTObjectSetProperty(exec, "status", JSTValueFromDouble(exec_child_status), 0);
		if (captureOut && exec_child_out) {
			JSTObjectSetProperty(exec, "stdout", JSTValueFromString(JSTStringFromUTF8(exec_child_out), 0), true);
			JSTStringFreeUTF8(exec_child_out);
		}
		if (captureError && exec_child_err) {
			JSTObjectSetProperty(exec, "stderr", JSTValueFromString(JSTStringFromUTF8(exec_child_err), 0), true);
			JSTStringFreeUTF8(exec_child_err);
		} 
	}
	if (captureTime) JSTScriptNativeEval("this.endTime = Object.freeze(new Date())", exec);
leaving:
	while (deallocated < allocated) JSTStringFreeUTF8(argument[deallocated++]);
	return (JSValueRef) exec;
}

static JSValueRef jst_exit JSTDeclareFunction () {

	int status = (argc == 1)?JSTValueToDouble(argv[0]):0;
	if (status == 0) {
		//TODO: maybe clean up ctx & ctxgroup?
	}
	exit(status);

	return JSTValueNull;
}

static JSValueRef jst_include JSTDeclareFunction (file) {

	JSTValue result;
	char *script = NULL, *src = NULL, *file = (argc)?JSTValueToUTF8(argv[0]):NULL;

	if (g_file_get_contents(file, &src, NULL, NULL)) {
		script = src; int c = 0;
		if (*script == '#' && *(script+1) == '!') {
			script+=2; while ((c = *script) && c != '\n') script++;
		}
		result = JSTScriptEval(script, NULL, file, 1); g_free(src);
	} else {
		JSTScriptNativeError(JST_URI_ERROR, "unable to get contents of file: %s", file);
	}

	JSTStringFreeUTF8(file);

	return result;
}

// this function must be parameter checked by src/jst/script/init.js script!
static JSValueRef jst_set_prototype JSTDeclareFunction () {

	if (argc != 2)
		return JSTScriptNativeError(JST_TYPE_ERROR, "expected arguments object, prototype");

	JSTObjectSetPrototype(argv[0], argv[1]);

	return argv[0];
}

static JSValueRef jst_set_path JSTDeclareFunction () {

	if (argc != 1)
		return JSTScriptNativeError(JST_TYPE_ERROR, "expected path argument");

	char * val = JSTValueToUTF8(argv[0]);
	int status = g_chdir(val);
	JSValueRef result = (status)?NULL:JSTValueFromDouble(status);
	JSTStringFreeUTF8(val);

	if (status) return JSTScriptNativeError(JST_URI_ERROR, strerror(errno));
	
	return result;
}

static JSValueRef jst_get_path JSTDeclareFunction () {

	if (argc != 0)
		return JSTScriptNativeError(JST_TYPE_ERROR, "unexpected argument");

	char * path = g_get_current_dir();
	JSTValue result = JSTValueFromUTF8(path);
	g_free(path);

	return result;
}

// Get the last path component of argument pathInput.
// The path need not exist on the file system. 
// if pathInput is not supplied, the current working directory will be substituted.
static JSValueRef jst_get_path_basename JSTDeclareFunction (String pathInput) {

	char * pathResult, * pathInput = NULL;

	if (argc == 0) {
		pathInput = g_get_current_dir();
	} else {
		if (argc > 1)
			return JSTScriptNativeError(JST_TYPE_ERROR, "expected single path argument");
		else if (JSTValueIsNull(argv[0]))
			return JSTScriptNativeError(JST_URI_ERROR, "path argument is null");
		pathInput = JSTValueToUTF8(argv[0]);
	}

	if (pathInput == NULL || *pathInput == 0) {
		g_free(pathInput);
		return JSTScriptNativeError(JST_URI_ERROR, "path argument is empty");
	}

	pathResult = g_path_get_basename(pathInput);
	g_free(pathInput);

	JSTValue basename = JSTValueFromUTF8(pathResult);
	g_free(pathResult);

	return basename;

}

static JSValueRef jst_get_path_directory JSTDeclareFunction (String pathInput) {

	char * pathResult, * pathInput = NULL;

	if (argc != 1) return
		JSTScriptNativeError(JST_TYPE_ERROR, "expected single path argument");
	else if (JSTValueIsNull(argv[0])) return
		JSTScriptNativeError(JST_URI_ERROR, "path argument is null");

	pathInput = JSTValueToUTF8(argv[0]);

	if (pathInput == NULL || *pathInput == 0) {
		g_free(pathInput);
		return JSTScriptNativeError(JST_URI_ERROR, "path argument is empty");
	}

	pathResult = g_path_get_dirname(pathInput);
	g_free(pathInput);

	JSTValue directory = JSTValueFromUTF8(pathResult);
	g_free(pathResult);

	return directory;

}

static JSValueRef jst_base64_encode JSTDeclareFunction () {

	if (argc != 1) return
		JSTScriptNativeError(JST_TYPE_ERROR, "expected string argument");

	char * input = JSTValueToUTF8(argv[0]);
	size_t len = strlen(input);
	char * base64 = g_base64_encode(input, len);

	g_free(input);
	JSValueRef result = JSTValueFromUTF8(base64);
	g_free(base64);

	return result;
}

static JSValueRef jst_base64_decode JSTDeclareFunction () {

	if (argc != 1) return 
		JSTScriptNativeError(JST_TYPE_ERROR, "expected string argument");

	char * input = JSTValueToUTF8(argv[0]);
	size_t len = strlen(input);
	char * output = g_base64_decode(input, &len);

	g_free(input);
	JSValueRef result = JSTValueFromUTF8(output);
	g_free(output);

	return result;
}

#include "jst/error.c"
#include "jst/init.inc"
#include "jst/env.c"
#include "jst/memory.c"
#include "jst/file.c"

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]) {

	JSTObject sys = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(global, "sys", sys, JSTObjectPropertyAPI);
	JSTObjectSetProperty(sys, "global", global, JSTObjectPropertyAPI);

	/* error base */
	JSTObjectSetMethod(global, "sys_error_number", jst_error_number, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_error_message", jst_error_message, JSTObjectPropertyAPI);

	JSTScriptEval(JSTErrorScript, global, "src/jst/script/error.js", 1);
	if (JSTScriptHasError) exit(JSTScriptReportException());

	/* engine stats */
	JSTObject engine = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "engine", engine, 0);
	JSTObjectSetProperty(engine, "copyright", JSTValueFromUTF8(JSTReservedAddress), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "license", JSTValueFromUTF8(SoftwareLicenseText), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "notice", JSTValueFromUTF8(SoftwareNoticeText), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "vendor", JSTValueFromUTF8(VENDOR), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "version", JSTValueFromUTF8(VERSION), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "codeName", JSTValueFromUTF8(CODENAME), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "os", JSTValueFromUTF8(JSTOperatingSystem), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "arch", JSTValueFromDouble(JSTArchitecture), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "path", JSTValueFromUTF8(argv[0]), JSTObjectPropertyAPI);

	/* common environment utilities */
	JSTObjectSetMethod(global, "sys_exit", jst_exit, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_execute", jst_execute, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_include", jst_include, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_set_prototype", jst_set_prototype, JSTObjectPropertyAPI);

	/* portable path operators */
	JSTObjectSetMethod(global, "sys_set_path", jst_set_path, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_get_path", jst_get_path, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_get_path_basename", jst_get_path_basename, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_get_path_directory", jst_get_path_directory, JSTObjectPropertyAPI);

	/* since we can't setup path until after script augmentation of the object these are full names */
	JSTObjectSetProperty(global, "sys_path_separator", JSTValueFromUTF8(G_DIR_SEPARATOR_S), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_path_splitter", JSTValueFromUTF8(G_SEARCHPATH_SEPARATOR_S), JSTObjectPropertyAPI);

	/* base64 transcoding */
	JSTObjectSetMethod(global, "sys_base64_encode", jst_base64_encode, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_base64_decode", jst_base64_decode, JSTObjectPropertyAPI);

	jst_env_default = envp;
	/*
		environment variable constructs
		Briefly, function takes no arguments and works with the startup envp.
		Constructor takes an optional argument, that will default to a copy
		of envp, for use as an argument to exec functions accepting envp parameter.
		These functions are augmented in script to provide standard javascript
		constructor function behavior.
	*/
	JSTObjectSetMethod(global, "sys_env_function", jst_env_function, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_env_constructor", jst_env_constructor, JSTObjectPropertyAPI);

	/* system constant numeric data */
	JSTObjectSetProperty(sys, "argc", JSTValueFromDouble(argc), JSTObjectPropertyAPI);
	JSTObjectSetProperty(sys, "argv", JSTValueFromPointer(argv), JSTObjectPropertyAPI);
	JSTObjectSetProperty(sys, "envp", JSTValueFromPointer(envp), JSTObjectPropertyAPI);
	JSTObjectSetProperty(sys, "uid", JSTValueFromDouble(getuid()), JSTObjectPropertyAPI);
	JSTObjectSetProperty(sys, "euid", JSTValueFromDouble(geteuid()), JSTObjectPropertyAPI);
	JSTObjectSetProperty(sys, "gid", JSTValueFromDouble(getgid()), JSTObjectPropertyAPI);
	JSTObjectSetProperty(sys, "pid", JSTValueFromDouble(getpid()), JSTObjectPropertyAPI);

	JSTObjectSetProperty(global, "sys_int8_min", JSTValueFromDouble(INT8_MIN), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_int8_max", JSTValueFromDouble(INT8_MAX), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_uint8_max", JSTValueFromDouble(UINT8_MAX), JSTObjectPropertyAPI);

	JSTObjectSetProperty(global, "sys_int16_min", JSTValueFromDouble(INT16_MIN), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_int16_max", JSTValueFromDouble(INT16_MAX), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_uint16_max", JSTValueFromDouble(UINT16_MAX), JSTObjectPropertyAPI);

	JSTObjectSetProperty(global, "sys_int32_min", JSTValueFromDouble(INT32_MIN), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_int32_max", JSTValueFromDouble(INT32_MAX), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_uint32_max", JSTValueFromDouble(UINT32_MAX), JSTObjectPropertyAPI);

	JSTObjectSetProperty(global, "sys_int64_min", JSTValueFromDouble(INT64_MIN), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_int64_max", JSTValueFromDouble(INT64_MAX), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_uint64_max", JSTValueFromDouble(UINT64_MAX), JSTObjectPropertyAPI);


	/* advanced memory interface */
	JSTObject memory = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "memory", memory, JSTObjectPropertyAPI);

	JSTObjectSetMethod(global, "sys_memory_allocate", jst_memory_allocate, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_memory_free", jst_memory_free, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_memory_resize", jst_memory_resize, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_memory_copy", jst_memory_copy, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_memory_new_copy", jst_memory_new_copy, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_memory_clear", jst_memory_clear, JSTObjectPropertyAPI);

	JSTObjectSetMethod(global, "sys_memory_read", jst_memory_read, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_memory_write", jst_memory_write, JSTObjectPropertyAPI);

	/* sys.memory constant numeric data */
	JSTObjectSetProperty(memory, "align", JSTValueFromDouble(G_MEM_ALIGN), JSTObjectPropertyAPI);

	/* configure sys.memory.byteOrder */
	JSTObject memoryByteOrder = JSTObjectSetProperty(
		memory, "byteOrder", JSTValueToObject(JSTValueFromDouble(G_BYTE_ORDER)),
		JSTObjectPropertyAPI
	);
	JSTObjectSetProperty(
		memoryByteOrder, "bigEndian",
		JSTValueFromBoolean(G_BYTE_ORDER == G_BIG_ENDIAN), JSTObjectPropertyAPI
	);
	JSTObjectSetProperty(
		memoryByteOrder, "littleEndian",
		JSTValueFromBoolean(G_BYTE_ORDER == G_LITTLE_ENDIAN), JSTObjectPropertyAPI
	);

	/* file stuff */
	JSTObjectSetMethod(global, "sys_file_test", jst_file_test, JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_file_is_regular", JSTValueFromDouble(G_FILE_TEST_IS_REGULAR), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_file_is_symlink", JSTValueFromDouble(G_FILE_TEST_IS_SYMLINK), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_file_is_directory", JSTValueFromDouble(G_FILE_TEST_IS_DIR), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_file_is_executable", JSTValueFromDouble(G_FILE_TEST_IS_EXECUTABLE), JSTObjectPropertyAPI);
	JSTObjectSetProperty(global, "sys_file_exists", JSTValueFromDouble(G_FILE_TEST_EXISTS), JSTObjectPropertyAPI);

	/* script based augmentation */
	JSTScriptEval(JSTInitScript, global, "src/jst/script/init.js", 1);
	if (JSTScriptHasError) exit(JSTScriptReportException());

	return global;

}
