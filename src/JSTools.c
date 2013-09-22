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
void iStackMachineWrite(iStackMachine * stack, unsigned index, double value);
double iStackMachineRead(iStackMachine * stack, unsigned index);

void iStackMachineWrite(iStackMachine * stack, unsigned index, register double value) {
	register unsigned type = stack->type; register void * address = iStackMachineSlot(stack, index);
	if (type == iStackMachineBool) *(bool*)address = value;
	else if (type == iStackMachineChar) *(char*)address = value;
	else if (type == iStackMachineShort) *(short*)address = value;
	else if (type == iStackMachineInt) *(int*)address = value;
	else if (type == iStackMachineLong) *(long*)address = value;
	else if (type == iStackMachineLongLong) *(long long*)address = value;
	else if (type == iStackMachineFloat) *(float*)address = value;
	else if (type == iStackMachineDouble) *(double*)address = value;
	else if (type == iStackMachineUTF8) *(char*)address = value;
	else if (type == iStackMachineUTF16) *(char16_t*)address = value;
	else if (type == iStackMachineUTF32) *(char32_t*)address = value;
}

double iStackMachineRead(iStackMachine * stack, unsigned index) {
	register unsigned type = stack->type; register void * address = iStackMachineSlot(stack, index);
	if (type == iStackMachineBool) return *(bool*)address;
	else if (type == iStackMachineChar) return *(char*)address;
	else if (type == iStackMachineShort) return *(short*)address;
	else if (type == iStackMachineInt) return *(int*)address;
	else if (type == iStackMachineLong) return *(long*)address;
	else if (type == iStackMachineLongLong) return *(long long*)address;
	else if (type == iStackMachineFloat) return *(float*)address;
	else if (type == iStackMachineDouble) return *(double*)address;
	else if (type == iStackMachineUTF8) return *(char*)address;
	else if (type == iStackMachineUTF16) return *(char16_t*)address;
	else if (type == iStackMachineUTF32) return *(char32_t*)address;
	return 0;
}

JSTObject JSTNativeInit_ JSTUtility(JSTObject js) {
	JSTObject native = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(js, "native", native, JSTPropertyRequired);
	return native;
}

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]) {

	static bool initialized;

	if (! initialized ) {
		initialized++;
	}

	JSTObject js = JSTValueToObject(JSTScriptEval(JSTInitScript, global, "JSTInit.js", 1));

	JSTObject jsRun = JSTValueToObject(JSTObjectGetProperty(js, "run"));
	JSTObjectSetProperty(jsRun, "argc", JSTValueFromDouble(argc), JSTPropertyConst);
	JSTObjectSetProperty(jsRun, "argv", JSTValueFromPointer(argv), JSTPropertyConst);
	JSTObjectSetProperty(jsRun, "envp", JSTValueFromPointer(envp), JSTPropertyConst);

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
