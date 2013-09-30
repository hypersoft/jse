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

#include "JSTools/Native.inc"

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]) {

	static bool initialized;
	char buffer[PATH_MAX];

	if (! initialized )	initialized++;

	JSTObject js = JSTValueToObject(JSTScriptEval(JSTInitScript, global, "JSTInit.js", 1));
	JSTObjectSetProperty(global, "js", js, JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);

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

