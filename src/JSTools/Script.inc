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

#define JSTScriptFunction(sName, iParamCount, pArgNames, sBody, sURL, iLine) JST(JSObjectMakeFunction, sName, iParamCount, pArgNames, sBody sURL, iLine)

