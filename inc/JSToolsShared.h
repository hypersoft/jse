#ifndef JSTShared

#define JSTShared true

#include "glib.inc"
#include "JavaScriptCore.inc"

#define JSTNativeProcedure(...) 	(JSContextRef ctx, ##__VA_ARGS__, JSValueRef * exception)
#define JSTFastNativeProcedure(...)	(register JSContextRef ctx, ##__VA_ARGS__, register JSValueRef * exception)
#define JSTNativeCall(NAME, ...)	NAME (ctx, ##__VA_ARGS__, exception)
#define JSTNativeFunction()		JSTNativeProcedure (JSObjectRef callee, JSObjectRef this, size_t argc, const JSValueRef argv[])
#define JSTCaughtException 			*exception

char * _JSTSeekLineEnding(register char * BUFFER);
void _JSTCacheRuntime(register JSContextRef ctx, register JSValueRef * exception);
JSStringRef _JSTCoreString(register char * BUFFER);
JSValueRef _JSTLoadString JSTNativeProcedure (JSValueRef file);
char * _JSTNativeLoadString JSTNativeProcedure (char * file);
JSStringRef _JSTCoreLoadString JSTNativeProcedure (JSStringRef file);
char * _JSTNativeString (register JSStringRef STR);
JSValueRef _JSTCallFunction (register JSContextRef ctx, JSValueRef * exception, JSObjectRef THIS, JSObjectRef FUNC, ...);
JSObjectRef _JSTCallConstructor (register JSContextRef ctx, JSValueRef * exception, JSObjectRef OBJ, ...);
bool JSTCoreRelease(register JSStringRef string);	// ...
bool JSTNativeRelease(register char * string);		// always returns true
JSValueRef _JSTRunScript JSTNativeProcedure (char * FILE);
#define JSTRunScript(FILE) JSTNativeCall(_JSTRunScript, FILE);

typedef struct {
	JSObjectRef
		Global, isNaN, parseInt, parseFloat, escape, unescape, isFinite, decodeURI,
		encodeURI, decodeURIComponent, encodeURIComponent, Array, Boolean, Date,
		Error, Function, JSON, Math, Number, Object, RangeError, ReferenceError,
		RegExp, String, SyntaxError, TypeError, URIError;
	JSValueRef Infinity, NaN, undefined;
	JSStringRef classOf;
} JSTGlobalRuntime;

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

#define JSTArgument(INDEX) 			((INDEX < argc) ? (RtJSArguments[INDEX]) : RtJS(undefined))
#define JSTArgumentObject(INDEX)	(JSObjectRef) JSTArgument(INDEX)
#define JSTParam(INDEX) 			JSTArgument((INDEX - 1))
#define JSTParamObject(INDEX) 		(JSObjectRef) JSTParam(INDEX)
#define JSTArgumentsObject() 		JSObjectMakeArray(ctx, RtJSArgumentsLength,  RtJSArguments, exception) 
#define JSTArgumentsValue() 		(JSValueRef) JSTArgumentsObject()
#define JSTArguments 				JSTArgumentsValue

#define JSTNativeConstructor(...)	JSTNativeProcedure (JSObjectRef callee, size_t argc, const JSValueRef argv[])
#define JSTNativePropertyReader()	JSTNativeProcedure (JSObjectRef object, JSStringRef property)
#define JSTNativePropertyWriter()	JSTNativeProcedure (JSObjectRef object, JSStringRef property, JSValueRef value)
#define JSTNativeConvertor()		JSTNativeProcedure (JSObjectRef object, JSType type)

#define JSTPropertyHidden 		kJSPropertyAttributeDontEnum
#define JSTPropertyReadOnly 	kJSPropertyAttributeReadOnly
#define JSTPropertyRequired 	kJSPropertyAttributeDontDelete
#define JSTPropertyConst 		(JSTPropertyReadOnly | JSTPropertyRequired)
#define JSTPropertyPrivate 		(JSTPropertyHidden | JSTPropertyRequired)
#define JSTPropertyProtected 	(JSTPropertyConst | JSTPropertyPrivate)

#define JSTNumberType kJSTypeNumber
#define JSTStringType kJSTypeString

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
#define JSTCoreMakeFunction(STR, PROC)	JSObjectMakeFunctionWithCallback (ctx, STR, PROC)
#define JSTMakeFunction(BUFFER, PROC)	JSTCoreMakeFunction(JSTCoreString(BUFFER), PROC)

#define JSTLoadString(VAL)			JSTNativeCall(_JSTLoadString, VAL)
#define JSTCoreLoadString(STR) 		JSTNativeCall(_JSTCoreLoadString, STR)
#define JSTNativeLoadString(BUFFER)	JSTNativeCall(_JSTNativeLoadString, BUFFER)

#define JSTCoreStringLength(STR)				JSStringGetMaximumUTF8CStringSize(STR)
#define JSTNativeStringCopy(STR, DEST, SIZE)	((char *) JSStringGetUTF8CString(STR, DEST, SIZE) ? BUFFER : NULL)
#define JSTCoreEqualsNative 					JSStringIsEqualToUTF8CString
#define JSTCoreEquals 							JSStringIsEqual

#define JSTCoreString(BUFFER)	((JSStringRef) _JSTCoreString(BUFFER))
#define JSTCoreMakeString(STR)	JSValueMakeString(ctx, STR)
#define JSTCoreGetString(VAL)	JSTNativeCall(JSValueToStringCopy, VAL)

#define JSTNativeString(STR)		((char *)		_JSTNativeString(STR))	
#define JSTNativeMakeString(BUFFER) JSTCoreMakeString(JSTCoreString(BUFFER))
#define JSTNativeGetString(VAL)		JSTNativeString(JSTCoreGetString(VAL))

#define JSTGetCoreValue(OBJ, STR)	JSTNativeCall(JSObjectGetProperty, OBJ, STR)
#define JSTGetCoreObject(OBJ, STR)	((JSObjectRef)	JSTGetCoreValue(OBJ, STR))
#define JSTGetValue(OBJ, BUFFER)	JSTGetCoreValue(OBJ, JSTCoreString(BUFFER))
#define JSTGetObject(OBJ, BUFFER)	JSTGetCoreObject(OBJ, JSTCoreString(BUFFER))
#define JSTGet 						JSTGetValue

#define JSTSetCoreValue(OBJ, STR, VAL, ATTR)	(void)	JSTNativeCall(JSObjectSetProperty, OBJ, STR, VAL, ATTR)
#define JSTSetCoreObject(OBJ, STR, VAL, ATTR)	JSTSetCoreValue(OBJ, STR, (JSValueRef) VAL, ATTR)
#define JSTSetValue(OBJ, STR, VAL, ATTR)		JSTSetCoreValue(OBJ, JSTCoreString(STR), VAL, ATTR)
#define JSTSetObject(OBJ, STR, VAL, ATTR)		JSTSetCoreObject(OBJ, JSTCoreString(STR), VAL, ATTR)
#define JSTSet 									JSTSetValue

#define JSTGetIndexValue(OBJ, INDEX)	JSTNativeCall(JSObjectGetPropertyAtIndex, OBJ, INDEX))
#define JSTGetIndexObject(OBJ, INDEX)	((JSObjectRef) JSTGetIndexValue(OBJ, INDEX))
#define JSTGetIndex 					JSTGetIndexValue

#define JSTSetIndexValue(OBJ, INDEX, VAL, ATTR)		(void)	JSTNativeCall(JSObjectSetPropertyAtIndex, OBJ, INDEX, VAL, ATTR)
#define JSTSetIndexObject(OBJ, INDEX, VAL, ATTR)	(void)	JSTSetIndexValue(OBJ, INDEX, (JSValueRef) VAL, ATTR)
#define JSTSetIndex 								JSTSetIndexValue

#define JSTDeleteCoreProperty(OBJ, STR) (bool) (JSTNativeCall(JSObjectDeleteProperty, OBJ, STR))
#define JSTDeleteProperty(OBJ, STR)		(bool) (JSTNativeCall(JSObjectDeleteProperty, OBJ, JSTCoreString(STR)))
#define JSTDelete JSTDeleteProperty

#define JSTHasCoreProperty(OBJ, STR)	JSObjectHasProperty(ctx, OBJ, STR)
#define JSTHasProperty(OBJ, BUFFER)		JSTHasCoreProperty(OBJ, JSTCoreString(BUFFER))

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
#define JSTCallFunctionValue(OBJ, FUNC, ...)	_JSTCallFunction(ctx, exception, OBJ, FUNC, ##__VA_ARGS__, NULL)
#define JSTCallFunctionObject(OBJ, FUNC, ...)	((JSObjectRef) JSTCallFunctionValue(OBJ, FUNC, ##__VA_ARGS__))
#define JSTCallCoreValue(OBJ, STR, ...) 		JSTCallFunctionValue(OBJ, JSTGetCoreObject(OBJ, STR), ##__VA_ARGS__)
#define JSTCallCoreObject(OBJ, STR, ...)		((JSObjectRef) JSTCallCoreValue(OBJ, STR, ##__VA_ARGS__))
#define JSTCallValue(OBJ, BUFFER, ...)			JSTCallCoreValue(OBJ, JSTCoreString(BUFFER), ##__VA_ARGS__)
#define JSTCallObject(OBJ, BUFFER, ...)			((JSObjectRef) JSTCallValue(OBJ, BUFFER, ##__VA_ARGS__))
#define JSTCall(OBJ, BUFFER, ...)				JSTCallValue(OBJ, BUFFER, ##__VA_ARGS__)
#define JSTCallFunction 						JSTCallFunctionValue

#define JSTCreateObject(CLASS, PRIV) 	JSObjectMake(ctx, CLASS, PRIV)

// OBJ[, ...]
#define JSTCallConstructor(OBJ, ...) _JSTCallConstructor(ctx, exception, OBJ, ##__VA_ARGS__, NULL)
#define JSTConstructObject(OBJ, ...) JSTCallConstructor(OBJ, ##__VA_ARGS__)
#define JSTConstructValue(OBJ, ...)	((JSValueRef) JSTConstructObject(OBJ, ##__VA_ARGS__))
#define JSTConstruct 				JSTConstructObject

#define JSTEvalCoreValue(STR, OBJ)	JSTNativeCall(JSEvaluateScript, STR, OBJ, NULL, 1)
#define JSTEvalCoreObject(STR, OBJ)	((JSObjectRef) JSTEvalCoreValue(STR, OBJ))
#define JSTEvalValue(STR, OBJ)		JSTEvalCoreValue(JSTCoreString(STR), OBJ)
#define JSTEvalObject(STR, OBJ)		JSTEvalCoreObject(JSTCoreString(STR), OBJ)
#define JSTEval(STR, OBJ) 			JSTEvalValue(STR, OBJ)

/* Value Added Hacks */

#define JSTParseClass(OBJ)	JSTCoreEval(RtJS(classOf), OBJ)

#define JSTSheBang(BUFFER) ((BUFFER && *BUFFER == '#' && *(BUFFER + 1) == '!') ? (_JSTSeekLineEnding(BUFFER)) : (BUFFER))

#define JSTReturnValueException return RtJSValue(undefined)
#define JSTReturnObjectException return RtJSObject(undedfined)

#endif
