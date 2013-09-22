#define JSTScriptEval(p1, o, p2, i) JST(JSTScriptEval_, p1, o, p2, i)
#define JSTScriptCheckSyntax(p1, p2, i) JST(JSTScriptCheckSyntax_, p1, p2, i)
#define JSTScriptNativeEval(pScript, oThis) JSTScriptEval(pScript, oThis, __FILE__, __LINE__)

#define JSTScriptError(p) JSTConstructorCall(JSTScriptNativeEval("Error", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptEvalError(p) JSTConstructorCall(JSTScriptNativeEval("EvalError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptRangeError(p) JSTConstructorCall(JSTScriptNativeEval("RangeError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptReferenceError(p) JSTConstructorCall(JSTScriptNativeEval("ReferenceError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptSyntaxError(p) JSTConstructorCall(JSTScriptNativeEval("SyntaxError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptTypeError(p) JSTConstructorCall(JSTScriptNativeEval("TypeError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptURIError(p) JSTConstructorCall(JSTScriptNativeEval("URIError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))

#define JSTScriptSetError(e) (*exception = e)
#define JSTScriptHasError (!JSTValueIsVoid(*exception) && JSTValueIsObject(*exception))

#define JSTScriptFunction(sName, iParamCount, pArgNames, sBody, sURL, iLine) JST(JSObjectMakeFunction, sName, iParamCount, pArgNames, sBody sURL, iLine)

