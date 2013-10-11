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

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]) {

	static bool initialized;
	char buffer[PATH_MAX];

	if (! initialized )	initialized++;

	// The argument that gtk could be used easily from scripting was far too compelling.
	seed_init_with_context (&argc, &argv, (SeedGlobalContext) ctx);

	JSTObject js = JSTValueToObject(JSTScriptEval(JSTInitScript, global, "JSTInit.js", 1));
	JSTObjectSetProperty(global, "js", js, JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);

	JSTObjectSetProperty(js, "exec", JSTFunctionCallback("exec", jsExecute), JSTObjectPropertyRequired);

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

	int oargc = JSTValueToDouble(JSTObjectGetProperty(this, "argc"));
	JSObjectRef oargv = (JSTObject) JSTObjectGetProperty(this, "argv");
	JSTObject shift = (JSTObject) JSTObjectGetProperty(oargv, "shift");

	char * nargv[oargc + argc + 1];
	char ** dest = nargv;

	while (i < oargc) {
		nargv[i] = JSTStringToUTF8(JSTValueToString(JSTFunctionCall(shift, oargv)), true);
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
		i = 0; while (i < argc) JSTStringFreeUTF8(nargv[i++]);
	}

	JSTObjectSetPrototype(exec, JSTScriptNativeEval("js.exec.prototype", NULL));

	return (JSValueRef) exec;
}

