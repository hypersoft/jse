/* Shogun */

#ifndef JSTools

#define	JSTools

#include "glib.inc"
#include "JavaScriptCore.inc"

typedef struct JSTGlobalRuntime {
	JSObjectRef
		Global, isNaN, parseInt, parseFloat, escape, unescape, isFinite, decodeURI,
		encodeURI, decodeURIComponent, encodeURIComponent, Array, Boolean, Date,
		Error, Function, JSON, Math, Number, Object, RangeError, ReferenceError,
		RegExp, String, SyntaxError, TypeError, URIError, classOf;
	JSValueRef Infinity, NaN, undefined;
	int argc;
	char **argv, **envp;
} JSTGlobalRuntime;

#ifndef JSToolsSource
	extern JSTGlobalRuntime JSTRuntime;
#endif

#define JSToolsProcedure(...)		(JSContextRef ctx, ##__VA_ARGS__, JSValueRef * exception)
#define JSToolsCall(NAME, ...)		NAME (ctx, ##__VA_ARGS__, exception)
#define JSToolsFunction()			JSToolsProcedure (JSObjectRef callee, JSObjectRef this, size_t argc, const JSValueRef argv[])
#define JSTFastNativeProcedure(...)	(register JSContextRef ctx, ##__VA_ARGS__, register JSValueRef * exception)
#define JSTNativeConstructor(...)	JSToolsProcedure (JSObjectRef callee, size_t argc, const JSValueRef argv[])
#define JSTNativePropertyReader()	JSToolsProcedure (JSObjectRef object, JSStringRef property)
#define JSTNativePropertyWriter()	JSToolsProcedure (JSObjectRef object, JSStringRef property, JSValueRef value)
#define JSTNativeConvertor()		JSToolsProcedure (JSObjectRef object, JSType type)

#define JSTPropertyHidden		kJSPropertyAttributeDontEnum
#define JSTPropertyReadOnly		kJSPropertyAttributeReadOnly
#define JSTPropertyRequired		kJSPropertyAttributeDontDelete
#define JSTPropertyConst		(JSTPropertyReadOnly | JSTPropertyRequired)
#define JSTPropertyPrivate		(JSTPropertyHidden | JSTPropertyRequired)
#define JSTPropertyProtected	(JSTPropertyConst | JSTPropertyPrivate)

#define JSTNumberType kJSTypeNumber
#define JSTStringType kJSTypeString

#define RtJS(SYM)		JSTRuntime.SYM
#define RtJSValue(SYM)	((JSValueRef)	RtJS(SYM))
#define RtJSObject(SYM)	((JSObjectRef)	RtJS(SYM))
#define RtJSArgumentsCallee callee
#define RtJSConstructor callee
#define RtJSFunction callee
#define RtJSThis this
#define RtJSArgumentsLength argc
#define RtJSArguments argv
#define RtJSException (JSObjectRef) *exception
#define RtJSProperty property

#define JSTArgument(INDEX)			((INDEX < argc) ? (RtJSArguments[INDEX]) : RtJS(undefined))
#define JSTArgumentObject(INDEX)	(JSObjectRef) JSTArgument(INDEX)
#define JSTParam(INDEX)				(((unsigned) INDEX > 0 && INDEX <= argc) ? (RtJSArguments[INDEX - 1]) : RtJS(undefined))
#define JSTParamObject(INDEX)		(JSObjectRef) JSTParam(INDEX)
#define JSTArgumentsObject()		JSObjectMakeArray(ctx, RtJSArgumentsLength,  RtJSArguments, exception) 
#define JSTArgumentsValue()			(JSValueRef) JSTArgumentsObject()
#define JSTArguments				JSTArgumentsValue

#define JSTCoreGetProperty(jsObject, jsStringName) JSToolsCall(JSObjectGetProperty, jsObject, jsStringName)
#define JSTCoreMakeFunction(STR, PROC)	JSObjectMakeFunctionWithCallback (ctx, STR, PROC)
#define JSTGetGlobalObject() JSContextGetGlobalObject(ctx)
#define JSTGetIndex(OBJ, INDEX)	JSToolsCall(JSObjectGetPropertyAtIndex, OBJ, INDEX))
#define JSTSetIndex(OBJ, INDEX, VAL, ATTR)	(void)	JSToolsCall(JSObjectSetPropertyAtIndex, OBJ, INDEX, (JSValueRef) VAL, ATTR)

bool		JSTFreeBuffer (char * buffer);
bool		JSTFreeString (JSStringRef string);

char		*JSTGetStringBuffer (JSStringRef jsStringSource, long * lngPtrSize, char ** chrPtrResult, bool bFreeSource);
JSStringRef	JSTCreateString (char * chrPtrBuffer, JSStringRef * jsStringRefResult, bool bFreeBuffer);

void		_JSTLoadRuntime (register JSContextRef ctx, JSObjectRef global, int argc, char *argv[], char *envp[], register JSValueRef * exception);
#define		JSTLoadRuntime(global, argc, argv, envp) _JSTLoadRuntime(ctx, global, argc, argv, envp, exception)

bool		_JSTCoreSetProperty JSToolsProcedure (JSObjectRef jsObject, JSStringRef jsStringName, JSValueRef jsValue, long jsAttributes);
#define		JSTCoreSetProperty(jsObject, jsStringName, jsValue, jsAttributes) JSToolsCall(_JSTCoreSetProperty, jsObject, jsStringName, (JSValueRef) jsValue, jsAttributes)

JSValueRef	_JSTCallFunction (register JSContextRef ctx, JSValueRef * exception, JSObjectRef THIS, JSObjectRef FUNC, ...);
#define		JSTCall(FUNC, THIS, ...) _JSTCallFunction(ctx, exception, THIS, FUNC, ##__VA_ARGS__, NULL)

JSObjectRef	_JSTCallConstructor (register JSContextRef ctx, JSValueRef * exception, JSObjectRef OBJ, ...);
#define		JSTCallConstructor(THIS, ...) 		_JSTCallConstructor(ctx, exception, THIS, ##__VA_ARGS__, NULL)

JSObjectRef	_JSTCoreCompileFunction (JSContextRef ctx, JSValueRef * exception, JSStringRef jsName, JSStringRef jsBody, ...);
#define		JSTCoreCompileFunction(jsName, jsScriptBody, ...) _JSTCompileFunction(ctx, exception, jsName, jsScriptBody, ##__VA_ARGS__, NULL)

JSObjectRef	_JSTCompileFunction (JSContextRef ctx, JSValueRef * exception, char * chrPtrName, char * chrPtrBody, ...);
#define		JSTCompileFunction(chrPtrName, chrPtrScript, ...) _JSTCompileFunction(ctx, exception, chrPtrName, chrPtrScript, ##__VA_ARGS__, NULL)

JSObjectRef	_JSTCompilePropertyFunction (JSContextRef ctx, JSValueRef * exception, JSStringRef jsName, char * chrPtrBody, ...);
#define		JSTCompilePropertyFunction(jsName, chrPtrScript, ...) _JSTCompilePropertyFunction(ctx, exception, jsName, chrPtrScript, ##__VA_ARGS__, NULL)

JSValueRef	_JSTEval JSToolsProcedure (char * chrPtrScript, JSObjectRef jsObject);
#define		JSTEval(chrPtrScript, jsObject) JSToolsCall(_JSTEval, chrPtrScript, jsObject)

JSValueRef	_JSTMakeString JSToolsProcedure (JSStringRef jsString, JSValueRef * jsValRefResult, bool bFreeString);
#define		JSTMakeString(jsString, jsValPtrResult, bFreeString) JSToolsCall(_JSTMakeString, jsString, jsValPtrResult, bFreeString)

JSStringRef	_JSTGetValueString JSToolsProcedure (JSValueRef jsValueRef, JSStringRef * jsStringRefPtrResult);
#define		JSTGetValueString(jsValueRef, jsStringRefPtrResult)	JSToolsCall(_JSTGetValueString, jsValueRef, jsStringRefPtrResult)

char		* _JSTLoadStringBuffer (char * chrPtrFile, char ** chrPtrPtrResult, bool bFreeFile);
#define		JSTLoadStringBuffer(chrPtrFile, chrPtrPtrResult, bFreeFile) _JSTLoadStringBuffer(chrPtrFile, chrPtrPtrResult, bFreeFile)

bool		_JSTSetProperty JSToolsProcedure (JSObjectRef jsObject, char * chrPtrName, JSValueRef jsValue, long jsAttributes);
#define		JSTSetProperty(jsObject, chrPtrName, jsValue, jsAttributes)	JSToolsCall(_JSTSetProperty, jsObject, chrPtrName, jsValue, jsAttributes)

JSValueRef	_JSTGetProperty JSToolsProcedure (JSObjectRef jsObject, char * chrPtrName);
#define		JSTGetProperty(jsObject, chrPtrName) JSToolsCall(_JSTGetProperty, jsObject, chrPtrName)

bool		_JSTHasProperty (JSContextRef ctx, JSObjectRef jsObject, char * chrPtrName);
#define		JSTHasProperty(jsObject, chrPtrName) _JSTHasProperty(ctx, jsObject, chrPtrName)

bool		_JSTDeleteProperty JSToolsProcedure (JSObjectRef jsObject, char * chrPtrName);
#define		JSTDeleteProperty(jsObject, chrPtrName)	JSToolsCall(_JSTDeleteProperty, jsObject, chrPtrName)

JSObjectRef	_JSTMakeFunction JSToolsProcedure (char * chrPtrName, void * fnPtr);
#define		JSTMakeFunction(chrPtrName, fnPtr) JSToolsCall(_JSTMakeFunction, chrPtrName, fnPtr)

JSObjectRef	_JSTSetPropertyFunction JSToolsProcedure (JSObjectRef jsObject, char * chrPtrName, void * fnPtr);
#define		JSTSetPropertyFunction(jsObject, chrPtrName, fnPtr) JSToolsCall(_JSTSetPropertyFunction, jsObject, chrPtrName, fnPtr)

JSObjectRef	_JSTSetPropertyScript (JSContextRef ctx, JSValueRef * exception, JSObjectRef jsObject, char * chrPtrName, char * chrPtrBody, ...);
#define		JSTSetPropertyScript(jsObject, chrPtrName, chrPtrScript, ...)	_JSTSetPropertyScript(ctx, exception, jsObject, chrPtrName, chrPtrScript, ##__VA_ARGS__, NULL)

JSValueRef	_JSTRunScript JSToolsProcedure (char * file, JSObjectRef jsObject);
#define		JSTRunScript(chrPtrFile, jsObject) JSToolsCall(_JSTRunScript, chrPtrFile, jsObject)

#define JSTCreateStaticString(chrPtrBuffer, jsStringRefPtrResult)	JSTCreateString(chrPtrBuffer, jsStringRefPtrResult, false)
#define JSTGetValueBuffer(jsValueRef, chrPtrPtrResult)	JSTGetStringBuffer(JSTGetValueString(jsValueRef, NULL), NULL, chrPtrPtrResult, true)
#define JSTMakeBufferValue(chrPtr) JSTMakeString(JSTCreateStaticString(chrPtr, NULL), NULL, true)
#define JSTEvalObject(chrPtrScript, jsObject)	(JSObjectRef)(JSTEval(chrPtrScript, jsObject))
#define JSTCallObject(FUNC, THIS, ...)	((JSObjectRef) JSTCall(FUNC, THIS, ##__VA_ARGS__))
#define JSTCallProperty(jsObjRefThis, chrPtrFuncName, ...)	JSTCall(JSTGetPropertyObject(jsObjRefThis, chrPtrFuncName), jsObjRefThis, ##__VA_ARGS__)
#define JSTCallPropertyObject(jsObjRefThis, chrPtrFuncName, ...)	((JSObjectRef) JSTCallProperty(jsObjRefThis, chrPtrFuncName, ##__VA_ARGS__))
#define JSTCallConstructorValue(THIS, ...)	((JSValueRef) JSTCallConstructor(THIS, ##__VA_ARGS__))
#define JSTGetPropertyObject(jsObject, chrPtrName)	(JSObjectRef)(JSTGetProperty(jsObject, chrPtrName))
#define JSTGetIndexObject(OBJ, INDEX)	(JSObjectRef)(JSTGetIndex(OBJ, INDEX))

#define JSTObject(VAL)			JSValueIsObject(ctx, VAL)
#define JSTString(VAL)			JSValueIsString(ctx, VAL)
#define JSTBoolean(VAL)			JSValueToBoolean(ctx, VAL)
#define JSTUndefined(VAL)		JSValueIsUndefined(ctx, VAL)
#define JSTNull(VAL)			JSValueIsNull(ctx, VAL)
#define JSTNumber(VAL)			JSValueIsNumber(ctx, VAL)
#define JSTConstructor(OBJ)		JSObjectIsConstructor(ctx, OBJ)
#define JSTFunction(OBJ)		JSObjectIsFunction(ctx, OBJ)
#define JSTReference(VAL)		( VAL && ! (JSTNull((JSValueRef)VAL)) && ! (JSTUndefined((JSValueRef)VAL)) )

#define JSTDouble(VAL)			JSValueToNumber(ctx, VAL, exception)
#define JSTValue(TYPE, VAL)		((TYPE)	JSTDouble(VAL))
#define JSTInteger(VAL)			JSTValue(long, VAL)
#define JSTUnsignedInteger(VAL) JSTValue(unsigned long, VAL)
#define JSTPointer(VAL)			((void*) JSTInteger(VAL))

#define JSTMakeObject(VAL)				JSValueToObject(ctx, VAL, exception)
#define JSTMakeBoolean(BOOL)			JSValueMakeBoolean(ctx, BOOL)
#define JSTMakeUndefined()				JSValueMakeUndefined(ctx)
#define JSTMakeNull()					JSValueMakeNull(ctx)
#define JSTMakeNumber(DOUBLE)			JSValueMakeNumber(ctx, (double) DOUBLE)
#define JSTMakePointer(PTR)				JSValueMakeNumber(ctx, (double) (long) PTR)
#define JSTMakeConstructor(CLASS, PROC) JSObjectMakeConstructor(ctx, CLASS, PROC)

#define JSTCoreStringLength(STR)		JSStringGetMaximumUTF8CStringSize(STR)
#define JSTCoreEqualsNative				JSStringIsEqualToUTF8CString
#define JSTCoreEquals					JSStringIsEqual

#define JSTGetPrivate(OBJ)				JSObjectGetPrivate(OBJ)
#define JSTSetPrivate(OBJ, PTR)			JSObjectSetPrivate(OBJ, PTR)
#define JSTGetPrivateInteger(OBJ)		(long) JSTGetPrivate(OBJ)
#define JSTSetPrivateInteger(OBJ, VAL)	JSTSetPrivate(OBJ, (void *) (long) VAL)

#define JSTGetPrototype(OBJ)			JSObjectGetPrototype(ctx, OBJ)
#define JSTGetPrototypeObject(OBJ)		((JSObjectRef)	JSTGetPrototype(OBJ))

#define JSTSetPrototype(OBJ, VAL)		(void)	JSObjectSetPrototype(ctx, OBJ, (JSValueRef) VAL)
#define JSTSetPrototypeObject(OBJ, VAL)	(void)	JSTSetPrototype(OBJ, VAL)

#define JSTCreateClassObject(CLASS, PRIV)	JSObjectMake(ctx, CLASS, PRIV)

#define JSTParseClass(OBJ)	JSTCall(RtJS(classOf), OBJ)

#define JSTReturnValueException return RtJSValue(undefined)
#define JSTReturnObjectException return RtJSObject(undefined)

#define JSTFunctionRelay(targetFunction)	JSToolsFunction() { return JSTRelayFunctionCall(targetFunction); } 
#define JSTRelayFunctionCall(func) JSObjectCallAsFunction(ctx, func, this, argc, argv, exception)

#define JSTCaughtException			*exception

#endif

