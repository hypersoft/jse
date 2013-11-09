#define JSTScriptEval(p1, o, p2, i) JST(JSTScriptEval_, p1, o, p2, i)
#define JSTScriptCheckSyntax(p1, p2, i) JST(JSTScriptCheckSyntax_, p1, p2, i)
#define JSTScriptNativeEval(pScript, oThis) JSTScriptEval(pScript, oThis, __FILE__, __LINE__)

#define JSTScriptError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("Error", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptEvalError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("EvalError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptRangeError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("RangeError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptReferenceError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("ReferenceError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptSyntaxError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("SyntaxError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptTypeError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("TypeError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptURIError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("URIError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))

#define JSTScriptSetError(e) (*exception = e)
#define JSTScriptHasError (!JSTValueIsVoid(*exception) && JSTValueIsObject(*exception))
#define JSTScriptReportException() (JST(JSTScriptReportException_), JSTValueNull)

#define JSTScriptFunction(sName, iParamCount, pArgNames, sBody, sURL, iLine) JST(JSObjectMakeFunction, sName, iParamCount, pArgNames, sBody sURL, iLine)

#define JSTScriptNativeError(f, ...) (JSTScriptNativeError_(ctx, exception, __FILE__, __LINE__, f, ##__VA_ARGS__), JSTValueNull)

#define JSTScriptRestrictArguments(func, job, count, usage) if (argc != count) return JSTScriptNativeError("%s: %s: %s arguments: usage: %s", func, job, (argc > count)?"too many":"insufficient", usage)

#ifdef JSTools_h

extern JSTValue JSTScriptEval_ JSTUtility(char *p1, JSTObject o, char * p2, size_t i);
extern bool JSTScriptCheckSyntax_ JSTUtility(char *p1, char *p2, size_t i);
extern void JSTScriptNativeError_(JSTContext ctx, JSTValue *exception, char * file, int line, char * format, ...);
extern void JSTScriptReportException_(JSTContext ctx, JSTValue *exception);

#else

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

void JSTScriptNativeError_(register JSTContext ctx, register JSTValue *exception, char * file, int line, char * format, ...) {
	va_list ap; va_start(ap, format); char * message = NULL;
	vasprintf(&message, format, ap);
	JSTScriptEval("throw(this)", JSTScriptError(message), file, line);
	free(message);
}

void JSTScriptReportException_(register JSTContext ctx, register JSTValue *exception) {
	register char *b = JSTValueToUTF8(*exception);
	JSTObject e = (JSTObject) *exception; *exception = NULL;
	fprintf(stderr, "JSE Fatal Error Dump: %s\n", b); free(b);
	b = JSTValueToUTF8(JSTScriptNativeEval("js.trace(this)", e));
	fprintf(stderr, "%s\n", b);
	free(b);
}

#endif

