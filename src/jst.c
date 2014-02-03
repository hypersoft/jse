/*

Hypersoft Systems JST AND Hypersoft Systems JSE
Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "jst.h"

#include "license.h"
#include "notice.h"

/* scripts */
#include "jst/error.inc"
#include "jst/extend.inc"
#include "jst/flags.inc"
#include "jst/type.inc"
#include "jst/sys.inc"

#include "jst/english.c"

const utf8 * JSTReservedAddress =

"Hypersoft Systems JSE Copyright 2013 Triston J. Taylor, All Rights Reserved.";

const utf8 * CODENAME = JSE_CODENAME;
const utf8 * VERSION = JSE_BUILDNO;
const utf8 * VENDOR = JSE_VENDOR;

JSTClass jst_root_class = NULL;

void jst_root_class_init() {
	if (jst_root_class) return;
	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	jsClass.className = "Root Class",
	jsClass.attributes = JSTClassPropertyManualPrototype,
	jst_root_class = JSClassRetain(JSClassCreate(&jsClass));
	
}

JSTObject JSTClassRoot_(JSTContext ctx, void * private, JSTObject prototype, void * exception) {
	if (!jst_root_class) jst_root_class_init();
	JSTObject object = JSTClassInstance(jst_root_class, private);
	if (prototype) JSTObjectSetPrototype(object, prototype);
	else JSTObjectSetPrototype(object, JSTValueNull);
	return object;
}

#include "jst/utilities.c"

utf8 * JSTStringToUTF8 (JSTString s, bool release) {
	if (!s) return NULL;
	register gsize bufferLength = 0; register void * buffer = NULL;
	if (s && (bufferLength = JSStringGetMaximumUTF8CStringSize(s)))
		JSStringGetUTF8CString(s, (buffer = g_malloc(bufferLength)), bufferLength);
	if (release) JSTStringRelease(s);
	return buffer;
}

utf32 * JSTStringToUTF32(register JSTString text, gsize len, bool release) {
	if (!text || !len) return NULL;
	const utf16 * utf = JSTStringUTF16(text);
	void * result = g_utf16_to_ucs4(utf, len, NULL, NULL, NULL);
	if (release) JSTStringRelease(text);
	return result;
}

JSTString JSTStringFromUTF32(register utf32 * text, gsize len, bool release) {
	if (!text || !len) return NULL;
	long bytes = 0; utf16 * utf = g_ucs4_to_utf16(text, len, &bytes, NULL, NULL);
	if (release) g_free(text);
	JSTString result = JSTStringFromUTF16(utf, bytes); g_free(utf);
	return result;
}

utf8 * JSTConstructUTF8(const utf8 * format, ...) {
	if (!format) return NULL;
	va_list ap; va_start(ap, format); utf8 * message = NULL;
	g_vasprintf(&message, format, ap); return message;
}

static JSValueRef jst_execute JSTDeclareFunction () {

	int child_status = 0, allocated = 0, deallocated = 0;
	utf8 * exec_child_out = NULL, * exec_child_err = NULL; gint exec_child_status = 0;

	JSObjectRef exec = JSTObjectUndefined;

	bool captureOut = false, captureError = false, captureTime = false;

	captureTime = JSTObjectHasProperty(this, "time");
	captureOut = JSTObjectHasProperty(this, "output");
	captureError = JSTObjectHasProperty(this, "error");

	JSObjectRef oargv = (JSTObject) JSTObjectGetProperty(this, "argv");
	int oargc = JSTValueToDouble(JSTObjectGetProperty(oargv, "length"));

	if (JSTScriptHasError) { return JSTValueUndefined; }

	utf8 * argument[oargc + argc + 1];
	utf8 ** dest = argument;

	while (allocated < oargc) {
		JSTString tmp = JSTValueToString(JSTObjectGetPropertyAtIndex(oargv, allocated));
		argument[allocated] = JSTStringToUTF8(tmp, true);
		allocated++;
	}

	dest = argument + oargc; int secondary=0;

	while (secondary < argc) {
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
	utf8 * script = NULL, * src = NULL, * file = (argc)?JSTValueToUTF8(argv[0]):NULL;

	if (g_file_get_contents(file, &src, NULL, NULL)) {
		script = src; int c = 0;
		if (* script == '#' && *(script+1) == '!') {
			script+=2; while ((c = * script) && c != '\n') script++;
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
	if (argc != 2) return
		JSTScriptNativeError
		(JST_TYPE_ERROR, "expected arguments object, prototype");
	JSTObjectSetPrototype(argv[0], argv[1]);
	return argv[0];
}

static JSValueRef jst_set_path JSTDeclareFunction () {

	if (argc != 1)
		return JSTScriptNativeError(JST_TYPE_ERROR, "expected path argument");

	utf8 * val = JSTValueToUTF8(argv[0]);
	int status = g_chdir(val);
	JSValueRef result = (status)?NULL:JSTValueFromDouble(status);
	JSTStringFreeUTF8(val);

	if (status) return JSTScriptNativeError(JST_URI_ERROR, g_strerror(errno));
	
	return result;
}

static JSValueRef jst_get_path JSTDeclareFunction () {

	if (argc != 0)
		return JSTScriptNativeError(JST_TYPE_ERROR, "unexpected argument");

	utf8 * path = g_get_current_dir();
	JSTValue result = JSTValueFromUTF8(path);
	g_free(path);

	return result;
}

// Get the last path component of argument pathInput.
// The path need not exist on the file system. 
// if pathInput is not supplied, the current working directory will be substituted.
static JSValueRef jst_get_path_basename JSTDeclareFunction (String pathInput) {

	utf8 * pathResult, * pathInput = NULL;

	if (argc == 0) {
		pathInput = g_get_current_dir();
	} else {
		if (argc > 1)
			return JSTScriptNativeError(JST_TYPE_ERROR, "expected single path argument");
		else if (JSTValueIsNull(argv[0]))
			return JSTScriptNativeError(JST_URI_ERROR, "path argument is null");
		pathInput = JSTValueToUTF8(argv[0]);
	}

	if (pathInput == NULL || * pathInput == 0) {
		g_free(pathInput);
		return JSTScriptNativeError(JST_URI_ERROR, "path argument is empty");
	}

	pathResult = g_path_get_basename(pathInput);
	g_free(pathInput);

	JSTValue basename = JSTValueFromUTF8(pathResult);
	g_free(pathResult);

	return basename;

}

static JSValueRef jst_get_path_link JSTDeclareFunction (String pathInput) {
	utf8 * pathResult = NULL, * pathInput = NULL;
	JSTValue result = NULL;
	if (argc) {
		GError * error = NULL; 
		pathInput = JSTValueToUTF8(argv[0]);
		pathResult = g_file_read_link(pathInput, &error);
		if (error) {
			JSTScriptNativeError(JST_URI_ERROR, error->message);
			g_error_free(error);
		} else {
			result = JSTValueFromUTF8(pathResult);
		}
		g_free(pathInput), g_free(pathResult);
	} else JSTScriptNativeError(JST_REFERENCE_ERROR,
		"expected argument: path"
	);
	return result;
}

static JSValueRef jst_get_path_directory JSTDeclareFunction (String pathInput) {

	utf8 * pathResult, * pathInput = NULL;

	if (argc != 1) return
		JSTScriptNativeError(JST_TYPE_ERROR, "expected single path argument");
	else if (JSTValueIsNull(argv[0])) return
		JSTScriptNativeError(JST_URI_ERROR, "path argument is null");

	pathInput = JSTValueToUTF8(argv[0]);

	if (pathInput == NULL || * pathInput == 0) {
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

	utf8 * input = JSTValueToUTF8(argv[0]);
	gsize len = strlen(input);
	utf8 * base64 = g_base64_encode(input, len);

	g_free(input);
	JSValueRef result = JSTValueFromUTF8(base64);
	g_free(base64);

	return result;
}

static JSValueRef jst_base64_decode JSTDeclareFunction () {

	if (argc != 1) return 
		JSTScriptNativeError(JST_TYPE_ERROR, "expected string argument");

	utf8 * input = JSTValueToUTF8(argv[0]);
	gsize len = strlen(input);
	utf8 * output = g_base64_decode(input, &len);

	g_free(input);
	JSValueRef result = JSTValueFromUTF8(output);
	g_free(output);

	return result;
}

#include "jst/sys/class.c"
#include "jst/sys/memory.c"

#include "jst/error.c"

void jst_sys_init JSTUtility(JSTObject global, utf8 * engineName) {
	JSTObject sys = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(global, "sys", sys, JSTObjectPropertyState);
	JSTObjectSetProperty(global, "global", global, JSTObjectPropertyState);
	JSTScriptNativeEval("Object.defineProperties(sys, { "
		"	argc: {value:argc, configurable:false},"
		"	argv: {value:argv, configurable:false}, "
		"	envp: {value:envp, configurable:false}, "
		"}); delete argc; delete argv; delete envp;",
		global
	);
	/* engine stats */ JSTObject engine = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "engine", engine, 0);
	JSTObjectSetProperty(engine, "copyright", JSTValueFromUTF8(JSTReservedAddress), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "license", JSTValueFromUTF8(SoftwareLicenseText), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "notice", JSTValueFromUTF8(SoftwareNoticeText), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "vendor", JSTValueFromUTF8(VENDOR), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "version", JSTValueFromUTF8(VERSION), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "codeName", JSTValueFromUTF8(CODENAME), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "os", JSTValueFromUTF8(JSTOperatingSystem), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "arch", JSTValueFromDouble(JSTArchitecture), JSTObjectPropertyAPI);
	JSTObjectSetProperty(engine, "path", JSTValueFromUTF8(engineName), JSTObjectPropertyAPI);
		/* advanced memory interface */
	JSTObject memory = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "memory", memory, JSTObjectPropertyAPI);
	JSTObjectSetMethod(sys, "memory_allocate", jst_memory_allocate, JSTObjectPropertyAPI);
	JSTObjectSetMethod(sys, "memory_free", jst_memory_free, JSTObjectPropertyAPI);
	JSTObjectSetMethod(sys, "memory_read", jst_memory_read, JSTObjectPropertyAPI);
	JSTObjectSetMethod(sys, "memory_write", jst_memory_write, JSTObjectPropertyAPI);
	/* sys.memory constant numeric data */
	JSTObjectSetProperty(memory, "align", JSTValueFromDouble(G_MEM_ALIGN), JSTObjectPropertyAPI);
	JSTObject memoryType = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(memoryType, "bool", JSTValueFromDouble(sizeof(bool)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memoryType, "char", JSTValueFromDouble(sizeof(char)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memoryType, "short", JSTValueFromDouble(sizeof(short)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memoryType, "int", JSTValueFromDouble(sizeof(int)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memoryType, "long", JSTValueFromDouble(sizeof(long)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memoryType, "longLong", JSTValueFromDouble(sizeof(long long)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memoryType, "size_t", JSTValueFromDouble(sizeof(size_t)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memoryType, "pointer", JSTValueFromDouble(sizeof(void*)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memoryType, "float", JSTValueFromDouble(sizeof(float)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memoryType, "double", JSTValueFromDouble(sizeof(double)), JSTObjectPropertyAPI);
	JSTObjectSetProperty(memory, "type", memoryType, JSTObjectPropertyAPI);
	/* configure sys.memory.byteOrder */
	JSTObject memoryByteOrder = JSTObjectSetProperty(
		memory, "byteOrder", JSTValueToObject
		(JSTValueFromDouble(G_BYTE_ORDER)), JSTObjectPropertyAPI
	);
	JSTObjectSetProperty(
		memoryByteOrder, "bigEndian", JSTValueFromBoolean
		(G_BYTE_ORDER == G_BIG_ENDIAN), JSTObjectPropertyAPI
	);
	JSTObjectSetProperty(
		memoryByteOrder, "littleEndian", JSTValueFromBoolean
		(G_BYTE_ORDER == G_LITTLE_ENDIAN), JSTObjectPropertyAPI
	);
	/* base64 transcoding */
	JSTObjectSetMethod(sys, "base64Encode", jst_base64_encode, JSTObjectPropertyAPI);
	JSTObjectSetMethod(sys, "base64Decode", jst_base64_decode, JSTObjectPropertyAPI);	
	/* kaboom: class system */
	JSTObjectSetMethod(sys, 
		"class_define", jst_class_define, JSTObjectPropertyAPI
	);
	JSTObjectSetMethod(sys,
		"class_instance", jst_class_instance, JSTObjectPropertyAPI
	);
	/* common environment utilities */
	JSTObjectSetMethod(sys, "exit", jst_exit, JSTObjectPropertyAPI);
	JSTObjectSetMethod(sys, "execute", jst_execute, JSTObjectPropertyAPI);
	JSTObjectSetMethod(sys, "include", jst_include, JSTObjectPropertyAPI);
	JSTObjectSetMethod(sys, "set_prototype", jst_set_prototype, JSTObjectPropertyAPI);
	/* error base */
	JSTObjectSetMethod(sys, "error_number", jst_error_number, JSTObjectPropertyAPI);
	JSTObjectSetMethod(sys, "error_message", jst_error_message, JSTObjectPropertyAPI);
	
}

JSTObject JSTInit_ JSTUtility(JSTObject global, gsize argc, utf8 * argv[], utf8 * envp[]) {

	/* system constant numeric data */
	JSTObjectSetProperty(global, "argc", JSTValueFromDouble(argc), 0);
	JSTObjectSetProperty(global, "argv", JSTValueFromPointer(argv), 0);
	JSTObjectSetProperty(global, "envp", JSTValueFromPointer(envp), 0);

	jst_sys_init(ctx, global, argv[0], exception);
		
	JSTScriptEval(JSTErrorScript, global, "jse(src/script/error.js)", 1);
	if (JSTScriptHasError) exit(JSTScriptReportException());
	
	JSTScriptEval(JSTExtendScript, global, "jse(src/script/extend.js)", 1);
	if (JSTScriptHasError) exit(JSTScriptReportException());

	JSTScriptEval(JSTFlagsScript, global, "jse(src/script/flags.js)", 1);
	if (JSTScriptHasError) exit(JSTScriptReportException());

	JSTScriptEval(JSTTypeScript, global, "jse(src/script/type.js)", 1);
	if (JSTScriptHasError) exit(JSTScriptReportException());

	/* script based augmentation */
	JSTScriptEval(JSTSysScript, global, "jse(src/jst/script/sys.js)", 1);
	if (JSTScriptHasError) exit(JSTScriptReportException());

	return global;

}
