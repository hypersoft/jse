#include "JSTools.inc"
#include "JSTInit.inc"

const char * JSTReservedAddress = "Hypersoft Systems JSE Copyright 2013 Triston J. Taylor, All Rights Reserved.";

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]);
JSTObject JSTNativeInit_ JSTUtility(JSTObject js);
char * JSTStringToUTF8_ JSTUtility(JSTString s, bool release);
JSTObject JSTConstructorCall_(JSTContext ctx, JSTValue *exception, JSTObject c, ...);
JSTValue JSTFunctionCall_(JSTContext ctx, JSTValue *exception, JSTObject method, JSTObject object, ...);
JSTObject JSTFunctionCallback_ JSTUtility(char * p, void * f);
JSTValue JSTValueFromString_ JSTUtility(JSTString s, bool release);
bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p);
void JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a);
JSTValue JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p);
bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p);
JSTValue JSTScriptEval_ JSTUtility(char *p1, JSTObject o, char * p2, size_t i);
bool JSTScriptCheckSyntax_ JSTUtility(char *p1, char *p2, size_t i);
JSTValue JSTValueFromJSON_ JSTUtility(char * p);
static JSValueRef jsExecute JSTDeclareFunction ();

#include "JSTools/Native.inc"
#include <seed.h>

JSObjectRef jsToolsPasswdToObject(JSContextRef ctx, uid_t uid, JSValueRef * exception) {
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

extern char *secure_getenv(const char *name);

static JSValueRef jsToolsEnvRead JSTDeclareFunction (String) {

	if (argc < 1) {
		JSTScriptSyntaxError("js.env.read: expected argument: string");
		return JSTValueUndefined;
	}

	if (!JSTValueIsString(argv[0])) {
		JSTScriptTypeError("js.env.read: expected string");
	} else {
		char * key = JSTStringToUTF8(JSTValueToString(argv[0]), true);
		char * value = secure_getenv(key); JSTStringFreeUTF8(key);
		return value?JSTValueFromUTF8(value):JSTValueUndefined;
	}

	return JSTValueUndefined;

}

static JSValueRef jsToolsEnvWrite JSTDeclareFunction (String) {

	if (argc < 2) {
		JSTScriptSyntaxError("js.env.write: expected string arguments: key, value");
		return JSTValueUndefined;
	}

	JSValueRef result = JSTValueUndefined;

	if (!JSTValueIsString(argv[0]) || !JSTValueIsString(argv[1])) {
		JSTScriptTypeError("js.env.write: expected string arguments");
	} else {
		char * key = JSTStringToUTF8(JSTValueToString(argv[0]), true);
		char * value = JSTStringToUTF8(JSTValueToString(argv[1]), true);
		result = JSTValueFromDouble(setenv(key, value, true));
		JSTStringFreeUTF8(key); JSTStringFreeUTF8(value);
	}

	return result;

}

static JSValueRef jsToolsEnvDelete JSTDeclareFunction (string) {

	if (argc < 1) {
		JSTScriptSyntaxError("js.env.delete: expected argument: string");
		return JSTValueUndefined;
	}

	JSValueRef result = JSTValueUndefined;

	if (!JSTValueIsString(argv[0])) {
		JSTScriptTypeError("js.env.delete: expected string");
	} else {
		char * key = JSTStringToUTF8(JSTValueToString(argv[0]), true);
		result = JSTValueFromDouble(unsetenv(key));
		JSTStringFreeUTF8(key);
	}

	return result;

}

static JSValueRef jsToolsEnvKeys JSTDeclareFunction (string) {
	char **env = JSTValueToPointer(JSTScriptNativeEval("js.run.envp", NULL));
	size_t count = 0; while(env[count++]);
	size_t i = 0; char key[PATH_MAX];
	JSObjectRef result = JSTClassInstance(NULL, NULL);
	while(env[i]) {
		sscanf(env[i++], "%[^=]", key);
		JSTObjectSetProperty(result, key, JSTValueUndefined, 0);
	}
	return result;
}

static JSValueRef jsToolsEnvCWD JSTDeclareFunction () {
	char buffer[PATH_MAX]; getcwd(buffer, PATH_MAX);
	return JSTValueFromUTF8(buffer);
}

static JSValueRef jsToolsEnvChDir JSTDeclareFunction () {
	char * val; JSValueRef
	result = JSTValueFromDouble(chdir(JSTStringToUTF8(JSTStringFromValue(argv[0]), true)));
	JSTStringFreeUTF8(val);
	return result;
}

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]) {

	static bool initialized;
	char buffer[PATH_MAX];

	if (! initialized )	initialized++;

	// The argument that gtk could be used easily from scripting was far too compelling.
	seed_init_with_context (&argc, &argv, (SeedGlobalContext) ctx);

	JSTObject js = JSTValueToObject(JSTScriptEval(JSTInitScript, global, "JSTInit.js", 1));
	JSTObjectSetProperty(global, "js", js, JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);

	JSTObjectSetProperty(js, "user", jsToolsPasswdToObject(ctx, getuid(), exception), JSTObjectPropertyRequired);
	JSTObjectSetProperty(js, "exec", JSTFunctionCallback("exec", jsExecute), JSTObjectPropertyRequired);

	JSObjectRef env = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(js, "env", env, JSTObjectPropertyRequired);
	JSTObjectSetProperty(env, "read", JSTFunctionCallback("read", jsToolsEnvRead), JSTObjectPropertyRequired);
	JSTObjectSetProperty(env, "write", JSTFunctionCallback("write", jsToolsEnvWrite), JSTObjectPropertyRequired);
	JSTObjectSetProperty(env, "keys", JSTFunctionCallback("keys", jsToolsEnvKeys), JSTObjectPropertyRequired);
	JSTObjectSetProperty(env, "delete", JSTFunctionCallback("delete", jsToolsEnvDelete), JSTObjectPropertyRequired);
	JSTObjectSetProperty(env, "cwd", JSTFunctionCallback("cwd", jsToolsEnvCWD), JSTObjectPropertyRequired);
	JSTObjectSetProperty(env, "chdir", JSTFunctionCallback("chdir", jsToolsEnvChDir), JSTObjectPropertyRequired);

	JSTObject jsRun = JSTValueToObject(JSTObjectGetProperty(js, "run"));

	JSTObjectSetProperty(jsRun, "argc", JSTValueFromDouble(argc), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(jsRun, "argv", JSTValueFromPointer(argv), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(jsRun, "envp", JSTValueFromPointer(envp), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(jsRun, "uid", JSTValueFromDouble(getuid()), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(jsRun, "euid", JSTValueFromDouble(geteuid()), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(jsRun, "gid", JSTValueFromDouble(getgid()), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(jsRun, "pid", JSTValueFromDouble(getpid()), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(jsRun, "path", JSTValueFromString(JSTStringFromUTF8(getcwd(buffer, PATH_MAX)),true), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(jsRun, "date", JSTScriptNativeEval("Object.freeze(new Date())", NULL), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);

	JSTNativeInit(js);

	return js;

}

JSTValue JSTValueFromJSON_ JSTUtility(char * p) {
	JSTValue result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSValueMakeFromJSONString, s);
		JSTStringRelease(s);
	}
	return result;
}

bool JSTScriptCheckSyntax_ JSTUtility(char *p1, char *p2, size_t i) {
	JSTString s[2] = {JSTStringFromUTF8(p1), JSTStringFromUTF8(p2)};
	bool result = JST(JSCheckScriptSyntax, s[0], s[1], i);
	JSTStringRelease(s[0]); JSTStringRelease(s[1]);
}

JSTValue JSTScriptEval_ JSTUtility(char *p1, JSTObject o, char * p2, size_t i) {
	JSTString s[2] = {JSTStringFromUTF8(p1), JSTStringFromUTF8(p2)};
	JSTValue result = JST(JSEvaluateScript, s[0], o, s[1], i);
	JSTStringRelease(s[0]); JSTStringRelease(s[1]);
	return result;
}

bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JST(JSObjectDeleteProperty, o, s);
		JSTStringRelease(s);
	}
	return result;
}

JSTValue JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p) {
	JSTValue result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JST(JSObjectGetProperty, o, s);
		JSTStringRelease(s);
	}
	return result;
}

void JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a) {
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		JST(JSObjectSetProperty, o, s, v, a);
		JSTStringRelease(s);
	}
}

bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSObjectHasProperty, o, s);
		JSTStringRelease(s);
	}
	return result;
}

JSTObject JSTFunctionCallback_ JSTUtility(char * p, void * f) {
	JSTObject result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSObjectMakeFunctionWithCallback, s, f);
		JSTStringRelease(s);
	}
	return result;
}

JSTValue JSTValueFromString_ JSTUtility(JSTString s, bool release) {
	JSTValue result = JSTAction(JSValueMakeString, s);
	if (release) JSTStringRelease(s);
	return result;
}

char * JSTStringToUTF8_ JSTUtility(JSTString s, bool release) {
	register size_t bufferLength = 0; register void * buffer = NULL;
	if (s && (bufferLength = JSStringGetMaximumUTF8CStringSize(s)))
		JSStringGetUTF8CString(s, (buffer = malloc(bufferLength)), bufferLength);
	if (release) JSTStringRelease(s);
	return buffer;
}

JSTObject JSTConstructorCall_(JSTContext ctx, JSTValue *exception, JSTObject c, ...) {
	va_list ap; register size_t argc = 0, count = 0; va_start(ap, c); 
	while(va_arg(ap, void*) != JSTReservedAddress) argc++; va_start(ap, c);
	if (argc > 32) return JSTObjectUndefined; JSTValue argv[argc+1];
	while (count < argc) argv[count++] = va_arg(ap, JSTValue);
	argv[count] = NULL; return JST(JSObjectCallAsConstructor, c, argc, argv);
}

JSTValue JSTFunctionCall_(JSTContext ctx, JSTValue *exception, JSTObject method, JSTObject object, ...) {
	va_list ap; register size_t argc = 0, count = 0; va_start(ap, object); 
	while(va_arg(ap, void*) != JSTReservedAddress) argc++; va_start(ap, object);
	if (argc > 32) return JSTValueUndefined; JSTValue argv[argc+1];
	while (count < argc) argv[count++] = va_arg(ap, JSTValue);
	argv[count] = NULL; return JST(JSObjectCallAsFunction, method, object, argc, argv);
}

static JSValueRef jsExecute JSTDeclareFunction () {

	int child_status = 0, i = 0;
	gchar *exec_child_out = NULL, *exec_child_err = NULL; gint exec_child_status = 0;

	JSObjectRef exec = JSTObjectUndefined;

	bool captureOut = false, captureError = false;

	captureOut = JSTObjectHasProperty(this, "output");
	captureError = JSTObjectHasProperty(this, "error");

	JSObjectRef oargv = (JSTObject) JSTObjectGetProperty(this, "argv");
	int oargc = JSTValueToDouble(JSTObjectGetProperty(oargv, "length"));

	char * nargv[oargc + argc + 1];
	char ** dest = nargv;

	while (i < oargc) {
		nargv[i] = JSTStringToUTF8(JSTValueToString(JSTObjectGetPropertyAtIndex(oargv, i)), true);
		i++;
	}

	dest = nargv + oargc; i=0;

	while (i < argc) {
		dest[i] = JSTStringToUTF8(JSTValueToString(argv[i]), true);
		i++;
	}

	dest[i] = NULL;

	if (true) {
		if (g_spawn_sync(NULL, nargv, NULL, G_SPAWN_LEAVE_DESCRIPTORS_OPEN | G_SPAWN_SEARCH_PATH | G_SPAWN_CHILD_INHERITS_STDIN, NULL, NULL, (captureOut) ? &exec_child_out : NULL, (captureError) ? &exec_child_err : NULL, &exec_child_status, NULL)) {
			exec_child_status = WEXITSTATUS(exec_child_status);
			bool success = (exec_child_status == 0);
			exec = JSTClassInstance(NULL, NULL);

			if (success) {
				JSTObjectSetProperty(exec, "status", JSTValueFromBoolean(success), 0);
			} else {
				JSTObjectSetProperty(exec, "status", JSTValueFromDouble(exec_child_status), 0);
			}

			if (captureOut && exec_child_out) {
				JSTObjectSetProperty(exec, "stdout", JSTValueFromString(JSTStringFromUTF8(exec_child_out), 0), true);
				JSTStringFreeUTF8(exec_child_out);
			}
			if (captureError && exec_child_err) {
				JSTObjectSetProperty(exec, "stderr", JSTValueFromString(JSTStringFromUTF8(exec_child_err), 0), true);
				JSTStringFreeUTF8(exec_child_err);
			} 
		}
		argc += oargc + 1;
		i = 0; while (i < argc) JSTStringFreeUTF8(nargv[i++]);
	}

	JSTObjectSetPrototype(exec, JSTScriptNativeEval("js.exec.prototype", NULL));

	return (JSValueRef) exec;
}

