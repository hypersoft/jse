#ifndef JSTShared

#define JSTShared true

#include "glib.inc"
#include "JavaScriptCore.inc"

typedef struct {
	JSObjectRef
		Global, isNaN, parseInt, parseFloat, escape, unescape, isFinite, decodeURI,
		encodeURI, decodeURIComponent, encodeURIComponent, Array, Boolean, Date,
		Error, Function, JSON, Math, Number, Object, RangeError, ReferenceError,
		RegExp, String, SyntaxError, TypeError, URIError, classOf;
	JSValueRef Infinity, NaN, undefined;
} JSTGlobalRuntime;

#define JSToolsProcedure(...) 		(JSContextRef ctx, ##__VA_ARGS__, JSValueRef * exception)
#define JSToolsCall(NAME, ...)		NAME (ctx, ##__VA_ARGS__, exception)
#define JSToolsFunction()			JSToolsProcedure (JSObjectRef callee, JSObjectRef this, size_t argc, const JSValueRef argv[])
#define JSTFastNativeProcedure(...)	(register JSContextRef ctx, ##__VA_ARGS__, register JSValueRef * exception)
#define JSTNativeConstructor(...)	JSToolsProcedure (JSObjectRef callee, size_t argc, const JSValueRef argv[])
#define JSTNativePropertyReader()	JSToolsProcedure (JSObjectRef object, JSStringRef property)
#define JSTNativePropertyWriter()	JSToolsProcedure (JSObjectRef object, JSStringRef property, JSValueRef value)
#define JSTNativeConvertor()		JSToolsProcedure (JSObjectRef object, JSType type)

#define JSTFunctionRelay(targetFunction)	JSToolsFunction() { return JSTRelayFunctionCall(targetFunction); } 

#define JSTRelayFunctionCall(func) JSObjectCallAsFunction(ctx, func, this, argc, argv, exception)

#define JSTPropertyHidden 		kJSPropertyAttributeDontEnum
#define JSTPropertyReadOnly 	kJSPropertyAttributeReadOnly
#define JSTPropertyRequired 	kJSPropertyAttributeDontDelete
#define JSTPropertyConst 		(JSTPropertyReadOnly | JSTPropertyRequired)
#define JSTPropertyPrivate 		(JSTPropertyHidden | JSTPropertyRequired)
#define JSTPropertyProtected 	(JSTPropertyConst | JSTPropertyPrivate)

#define JSTNumberType kJSTypeNumber
#define JSTStringType kJSTypeString

#define JSTCaughtException 			*exception

#define JSTPropertyMaster long JSTBufferLen; char * JSTBufferRef; JSObjectRef JSTObjectRef; JSStringRef JSTStringRef; JSValueRef JSTValueRef

void _JSTLoadRuntime(register JSContextRef ctx, register JSValueRef * exception);
JSValueRef _JSTCallFunction (register JSContextRef ctx, JSValueRef * exception, JSObjectRef THIS, JSObjectRef FUNC, ...);
JSObjectRef _JSTCallConstructor (register JSContextRef ctx, JSValueRef * exception, JSObjectRef OBJ, ...);
JSObjectRef _JSTCoreCompileFunction(JSContextRef ctx, JSValueRef * exception, JSStringRef jsName, JSStringRef jsBody, ...);
JSObjectRef _JSTCompileFunction(JSContextRef ctx, JSValueRef * exception, char * chrPtrName, char * chrPtrBody, ...);
JSObjectRef _JSTCompilePropertyFunction(JSContextRef ctx, JSValueRef * exception, JSStringRef jsName, char * chrPtrBody, ...);
JSValueRef _JSTRunScript JSToolsProcedure(char * file, JSObjectRef this);

#define JSTRunScript(chrPtrFile, jsObject) JSToolsCall(_JSTRunScript, chrPtrFile, jsObject)

bool
	JSTFreeBuffer(char * buffer),
	JSTFreeString(JSStringRef string),
	_JSTCoreSetProperty JSToolsProcedure (JSObjectRef jsObject, JSStringRef jsStringName, JSValueRef jsValue, long jsAttributes);

JSStringRef
/*-->*/_JSTCreateString(char * chrPtrBuffer, JSStringRef * jsStringPtrResult, bool bFreeBuffer);
#define JSTCreateString(chrPtrBuffer, jsStringPtrResult, bFreeBuffer) (	\
	((*jsStringPtrResult = JSStringCreateWithUTF8CString(chrPtrBuffer)) || true) &&	\
	(((bFreeBuffer != 0) && JSTFreeBuffer(chrPtrBuffer)) || true)	\
) ? *jsStringPtrResult : NULL

#define JSTCreateStaticString(chrPtrBuffer, jsStringPtrResult) (*jsStringPtrResult = JSStringCreateWithUTF8CString(chrPtrBuffer))

char *
/*-->*/_JSTGetStringBuffer(JSStringRef jsStringSource, char ** chrPtrPtrResult, long * lngPtrSize, bool bFreeSource);
#define JSTGetStringBuffer(jsStringSource, chrPtrPtrResult, lngPtrSize, bFreeSource) (	\
	((*chrPtrPtrResult = NULL) || true) &&	\
	((*lngPtrSize = JSStringGetMaximumUTF8CStringSize(jsStringSource)) || true) &&	\
	((JSStringGetUTF8CString(jsStringSource,	\
		(*chrPtrPtrResult = g_new(char, *lngPtrSize)), *lngPtrSize	\
	)) || true) &&	\
	(((bFreeSource != 0) && JSTFreeString(jsStringSource)) || true)	\
)	? *chrPtrPtrResult : NULL

JSValueRef
/*-->*/_JSTMakeString JSToolsProcedure (JSStringRef jsString, JSValueRef * jsValPtrResult, bool bFreeString);
#define JSTMakeString(jsString, jsValPtrResult, bFreeString) (	\
	((*jsValPtrResult = JSValueMakeString(ctx, jsString)) || true)	&&	\
	(((bFreeString != 0) && JSTFreeString(jsString)) || true)	\
)	? *jsValPtrResult : NULL

JSStringRef
/*-->*/_JSTGetStringValue JSToolsProcedure (JSValueRef jsVal, JSStringRef * jsStringPtrResult);
#define JSTGetStringValue(jsVal, jsStringPtrResult)	(	\
	*jsStringPtrResult = JSValueToStringCopy(ctx, jsVal, exception)	\
)

char *
/*-->*/_JSTLoadStringBuffer (char * chrPtrFile, char ** chrPtrPtrResult, bool bFreeFile);
#define JSTLoadStringBuffer(chrPtrFile, chrPtrPtrResult, bFreeFile) (	\
	((g_file_get_contents(chrPtrFile, chrPtrPtrResult, NULL, NULL)) || true) &&	\
	(((bFreeFile != 0) && JSTFreeBuffer(chrPtrFile)) || true) \
)	? *chrPtrPtrResult : NULL

#define JSTGetValueBuffer(jsVal, chrPtrPtrResult) (*chrPtrPtrResult = JSTGetStringBuffer(JSTGetStringValue(jsVal, &JSTStringRef), chrPtrPtrResult, &JSTBufferLen, true))

#define JSTMakeBufferValue(chrPtr) JSTMakeString(JSTCreateStaticString(chrPtr, &JSTStringRef), &JSTValueRef, true)

#define JSTCoreSetProperty(jsObject, jsStringName, jsValue, jsAttributes)	\
(	\
	JSToolsCall(_JSTCoreSetProperty, jsObject, jsStringName, (JSValueRef) jsValue, jsAttributes)	\
)
#define JSTSetProperty(jsObject, chrPtrName, jsValue, jsAttributes)	\
(	\
	JSTCoreSetProperty(jsObject, JSTCreateStaticString(chrPtrName, &JSTStringRef), jsValue, jsAttributes)	\
) ? (JSTFreeString(JSTStringRef)) : false

#define JSTCoreGetProperty(jsObject, jsStringName)	\
(	\
	JSToolsCall(JSObjectGetProperty, jsObject, jsStringName)	\
)
#define JSTGetProperty(jsObject, chrPtrName)	\
(	\
	( (JSTValueRef = JSTCoreGetProperty(jsObject, JSTCreateStaticString(chrPtrName, &JSTStringRef))) || true) &&	\
	( JSTFreeString(JSTStringRef) )	\
) ? ( JSTValueRef ) : NULL
#define JSTGetPropertyObject(jsObject, chrPtrName)	(JSObjectRef)(JSTGetProperty(jsObject, chrPtrName))

#define JSTDeleteProperty(jsObject, chrPtrName)	\
(	\
	JSToolsCall(JSObjectDeleteProperty, jsObject, JSTCreateStaticString(chrPtrName, &JSTStringRef)) \
) ? (JSTFreeString(JSTStringRef)) : (! JSTFreeString(JSTStringRef))

#define JSTCoreHasProperty(jsObject, jsStringName)	\
(	\
	JSToolsCall(JSObjectHasProperty, jsObject, jsStringName)	\
)
#define JSTHasProperty(jsObject, chrPtrName)	\
(	\
	JSTCoreHasProperty(jsObject, JSTCreateStaticString(chrPtrName, &JSTStringRef)) \
) ? (JSTFreeString(JSTStringRef)) : (! JSTFreeString(JSTStringRef))

#define JSTEval(chrPtrScript, jsObject) (	\
(	\
	( (JSTValueRef = JSToolsCall(JSEvaluateScript, JSTCreateStaticString(chrPtrScript, &JSTStringRef), jsObject, NULL, 1)) || true) &&	\
	( JSTFreeString(JSTStringRef) )	\
) ? ( JSTValueRef ) : NULL )
#define JSTEvalObject(chrPtrScript, jsObject) (JSObjectRef)(JSTEval(chrPtrScript, jsObject))

#define JSTCoreMakeFunction(STR, PROC)	JSObjectMakeFunctionWithCallback (ctx, STR, PROC)
#define JSTMakeFunction(chrPtrName, fnPtr) (	\
(	\
	( (JSTObjectRef = JSTCoreMakeFunction(JSTCreateStaticString(chrPtrName, &JSTStringRef), fnPtr)) || true) &&	\
	( JSTFreeString(JSTStringRef) )	\
) ? ( JSTObjectRef ) : NULL )

#define JSTSetPropertyFunction(jsObject, chrPtrName, fnPtr) \
( ( (JSTCoreSetProperty (	\
	jsObject, JSTStringRef, (JSTObjectRef = JSTCoreMakeFunction (	\
		JSTCreateStaticString(chrPtrName, &JSTStringRef), fnPtr	\
	) ), JSTPropertyConst) || true) && JSTFreeString(JSTStringRef)	\
) ? JSTObjectRef : NULL )

#define JSTCoreCompileFunction(jsName, jsScriptBody, ...) _JSTCompileFunction(ctx, exception, jsName, jsScriptBody, ##__VA_ARGS__, NULL)
#define JSTCompileFunction(chrPtrName, chrPtrScript, ...) _JSTCompileFunction(ctx, exception, chrPtrName, chrPtrScript, ##__VA_ARGS__, NULL)
#define JSTCompilePropertyFunction(jsName, chrPtrScript, ...) _JSTCompilePropertyFunction(ctx, exception, jsName, chrPtrScript, ##__VA_ARGS__, NULL)

#define JSTSetPropertyScript(jsObject, chrPtrName, chrPtrScript, ...) 	\
(	\
	(JSTObjectRef = JSTCompilePropertyFunction (JSTCreateStaticString(chrPtrName, &JSTStringRef), chrPtrScript, ##__VA_ARGS__)) && \
	JSTCoreSetProperty(jsObject, JSTStringRef, JSTObjectRef, JSTPropertyConst) &&	\
	JSTFreeString(JSTStringRef)	\
)	? JSTObjectRef : NULL

#define JSTGetIndex(OBJ, INDEX)						JSToolsCall(JSObjectGetPropertyAtIndex, OBJ, INDEX))
#define JSTGetIndexObject(OBJ, INDEX)				(JSObjectRef)(JSTGetIndex(OBJ, INDEX))
#define JSTSetIndex(OBJ, INDEX, VAL, ATTR)			(void)	JSToolsCall(JSObjectSetPropertyAtIndex, OBJ, INDEX, (JSValueRef) VAL, ATTR)

#define RtJS(SYM) 		JSTRuntime.SYM
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

#define JSTArgument(INDEX) 			((INDEX < argc) ? (RtJSArguments[INDEX]) : RtJS(undefined))
#define JSTArgumentObject(INDEX)	(JSObjectRef) JSTArgument(INDEX)
#define JSTParam(INDEX) 			JSTArgument((INDEX - 1))
#define JSTParamObject(INDEX) 		(JSObjectRef) JSTParam(INDEX)
#define JSTArgumentsObject() 		JSObjectMakeArray(ctx, RtJSArgumentsLength,  RtJSArguments, exception) 
#define JSTArgumentsValue() 		(JSValueRef) JSTArgumentsObject()
#define JSTArguments 				JSTArgumentsValue

#define JSTObject(VAL) 			JSValueIsObject(ctx, VAL)
#define JSTString(VAL)			JSValueIsString(ctx, VAL)
#define JSTBoolean(VAL) 		JSValueToBoolean(ctx, VAL)
#define JSTUndefined(VAL)		JSValueIsUndefined(ctx, VAL)
#define JSTNull(VAL)			JSValueIsNull(ctx, VAL)
#define JSTNumber(VAL)			JSValueIsNumber(ctx, VAL)
#define JSTConstructor(OBJ)		JSObjectIsConstructor(ctx, OBJ)
#define JSTFunction(OBJ)		JSObjectIsFunction(ctx, OBJ)
#define JSTReference(VAL)		( VAL && ! (JSTNull((JSValueRef)VAL)) && ! (JSTUndefined((JSValueRef)VAL)) )

#define JSTDouble(VAL) 			JSValueToNumber(ctx, VAL, exception)
#define JSTValue(TYPE, VAL)		((TYPE)	JSTDouble(VAL))
#define JSTInteger(VAL) 		JSTValue(long, VAL)
#define JSTUnsignedInteger(VAL) JSTValue(unsigned long, VAL)
#define JSTPointer(VAL) 		((void*) JSTInteger(VAL))

#define JSTMakeObject(VAL) 				JSValueToObject(ctx, VAL, exception)
#define JSTMakeBoolean(BOOL)		 	JSValueMakeBoolean(ctx, BOOL)
#define JSTMakeUndefined()				JSValueMakeUndefined(ctx)
#define JSTMakeNull()					JSValueMakeNull(ctx)
#define JSTMakeNumber(DOUBLE) 			JSValueMakeNumber(ctx, (double) DOUBLE)
#define JSTMakePointer(PTR)				JSValueMakeNumber(ctx, (double) (long) PTR)
#define JSTMakeConstructor(CLASS, PROC) JSObjectMakeConstructor(ctx, CLASS, PROC)

#define JSTCoreStringLength(STR)				JSStringGetMaximumUTF8CStringSize(STR)
#define JSTCoreEqualsNative 					JSStringIsEqualToUTF8CString
#define JSTCoreEquals 							JSStringIsEqual

#define JSTGetPrivate(OBJ)				JSObjectGetPrivate(OBJ)
#define JSTSetPrivate(OBJ, PTR)			JSObjectSetPrivate(OBJ, PTR)
#define JSTSetPrivateInteger(OBJ, VAL)	JSTSetPrivate(OBJ, (void *) (long) VAL)
#define JSTGetPrivateInteger(OBJ)		(long) JSTGetPrivate(OBJ)

#define JSTGetPrototypeValue(OBJ)	JSObjectGetPrototype(ctx, OBJ)
#define JSTGetPrototypeObject(OBJ)	((JSObjectRef)	JSTGetPrototypeValue(OBJ))
#define JSTGetPrototype				JSTGetPrototypeValue

#define JSTSetPrototypeValue(OBJ, VAL)		(void)	JSObjectSetPrototype(ctx, OBJ, VAL)
#define JSTSetPrototypeObject(OBJ, PROTO)	(void)	JSTSetPrototypeValue(OBJ, (JSValueRef) PROTO)
#define JSTSetPrototype 					JSTSetPrototypeValue

// OBJ, FUNC[, ...]
#define JSTCallFunction(OBJ, FUNC, ...)			_JSTCallFunction(ctx, exception, OBJ, FUNC, ##__VA_ARGS__, NULL)
#define JSTCallFunctionObject(OBJ, FUNC, ...)	((JSObjectRef) JSTCallFunction(OBJ, FUNC, ##__VA_ARGS__))

#define JSTCall(OBJ, BUFFER, ...)				JSTCallFunction(OBJ, JSTGetPropertyObject(OBJ, BUFFER), ##__VA_ARGS__)
#define JSTCallObject(OBJ, BUFFER, ...)			((JSObjectRef) JSTCall(OBJ, BUFFER, ##__VA_ARGS__))

#define JSTCreateClassObject(CLASS, PRIV) 	JSObjectMake(ctx, CLASS, PRIV)

// OBJ[, ...]
#define JSTCallConstructor(OBJ, ...) _JSTCallConstructor(ctx, exception, OBJ, ##__VA_ARGS__, NULL)
#define JSTConstructObject(OBJ, ...) JSTCallConstructor(OBJ, ##__VA_ARGS__)
#define JSTConstructValue(OBJ, ...)	((JSValueRef) JSTConstructObject(OBJ, ##__VA_ARGS__))
#define JSTConstruct 				JSTConstructObject

/* Value Added Hacks */

#define JSTParseClass(OBJ)	JSTCoreEval(RtJS(classOf), OBJ)

#define JSTSheBang(BUFFER) ((BUFFER && *BUFFER == '#' && *(BUFFER + 1) == '!') ? (_JSTSeekLineEnding(BUFFER)) : (BUFFER))

#define JSTReturnValueException return RtJSValue(undefined)
#define JSTReturnObjectException return RtJSObject(undefined)

#endif
