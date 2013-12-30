#define jse_type_const 1
#define jse_type_signed 2
#define jse_type_int 4
#define jse_type_struct 8
#define jse_type_union 16
#define jse_type_utf 32
#define jse_type_void 64
#define jse_type_bool 128
#define jse_type_char 256
#define jse_type_short 512
#define jse_type_long 1024
#define jse_type_size 2048
#define jse_type_pointer 4096
#define jse_type_int64 8192
#define jse_type_float 16384
#define jse_type_double 32768
#define jse_type_value 65536
#define jse_type_string 131072

#include "sys/error.inc"
#include "sys/env.inc"
#include "sys/io.inc"
#include "sys/memory.inc"
#include "sys/object.inc"
#include "sys/string.inc"

JSObjectRef jst_passwd_struct_to_object(JSContextRef ctx, uid_t uid, JSValueRef * exception) {
	JSObjectRef result = JSTClassInstance(NULL, NULL);
	struct passwd * pws = getpwuid(uid);
	if (pws) {
		JSTObjectSetProperty(result, "name", JSTValueFromUTF8(pws->pw_name), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "password", JSTValueFromUTF8(pws->pw_passwd), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "id", JSTValueFromDouble(uid), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "gid", JSTValueFromDouble(pws->pw_gid), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "comment", JSTValueFromUTF8(pws->pw_gecos), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "home", JSTValueFromUTF8(pws->pw_dir), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "shell", JSTValueFromUTF8(pws->pw_shell), JSTObjectPropertyRequired);
	} else {
		char out[PATH_MAX];
		sprintf(out, "%s: errno %lu", "JSE: unable to construct system user object", errno);
		JSTScriptReferenceError(out);
		return JSTObjectUndefined;
	}
	return result;
}

static JSTValue jst_user_of JSTDeclareFunction () {
	if (JSTValueIsNumber(argv[0]))
	return (JSTValue) jst_passwd_struct_to_object(ctx, JSTValueToDouble(argv[0]), exception);
	else return (JSTValue) jst_passwd_struct_to_object(ctx, geteuid(), exception);
}

static JSValueRef jst_run JSTDeclareFunction (file, [global object]) {
	JSTValue result;
	char *script = NULL, *src = NULL, *file = (argc)?JSTValueToUTF8(argv[0]):NULL;
	if (g_file_get_contents(file, &src, NULL, NULL)) {
		script = src; int c = 0;
		if (*script == '#' && *(script+1) == '!') {
			script+=2; while ((c = *script) && c != '\n') script++;
		}
		result = JSTScriptEval(script, (argc == 2)?(JSTObject)argv[1]:NULL, file, 1); g_free(src);
	} else {
		JSTScriptNativeError("sys.run: unable to get contents of file: `%s'", file);
	}
	JSTStringFreeUTF8(file);
	return result;
}

static JSValueRef jst_eval JSTDeclareFunction(source, file, line) {
	char * source = JSTValueToUTF8(argv[0]), * file = JSTValueToUTF8(argv[1]);
	JSTValue result = JSTScriptEval(source, this, file, JSTValueToDouble(argv[2]));
	free(file); free(source); return result;
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
	if (argc == 1) exit(JSTValueToDouble(argv[0]));
	else exit(0);
	return JSTValueNull;
}

