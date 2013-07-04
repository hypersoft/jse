#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "glib.inc"
#include "dyncall.inc"
#include "JavaScriptCore.inc"

extern JSStringRef * JSTCoreStringTable(long *, ...);

#define js_throw_exception(C, STR, E) puts(STR)

#define JSTProcedure(...)  (JSContextRef ctx, __VA_ARGS__, JSValueRef * exception)

/* give up two registers for a "fast call frame" */
#define JSTFastProcedure(...)   (register JSContextRef ctx, __VA_ARGS__, register JSValueRef * exception)

// The following definitions take null arguments for the developers convenience
#define JSTNativeFunction(...) 		JSTProcedure (JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[])
#define JSTNativeConstructor(...)	JSTProcedure (JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[])

#define JSTNativePropertyReader()	JSTProcedure (JSObjectRef object, JSStringRef propertyName)
#define JSTNativePropertyWriter()	JSTProcedure (JSObjectRef object, JSStringRef propertyName, JSValueRef value)
#define JSTNativeClassConvertor()	JSTProcedure (JSObjectRef object, JSType type)

#define JSTExec(...) (ctx, __VA_ARGS__, exception)
#define JSTSubExec(...) (ctx, __VA_ARGS__)

#define JSTGlobalObject (JSContextGetGlobalObject(ctx))

#define JSTCoreString(VAL)	 	(JSStringRef) (JSValueToStringCopy JSTExec (VAL))
#define JSTCoreStringDead(STR) 	(void) JSStringRelease(STR)
#define JSTMakeCoreString(STR)	(JSValueRef) (JSValueMakeString JSTSubExec (STR))

#define JSTNativeString(STR)			(char *) 		(_JSTNativeString JSTExec(STR))
#define JSTNativeStringDead(BUFFER)		(void) 			g_free(BUFFER)
#define JSTMakeNativeString(BUFFER)		(JSStringRef)	(JSStringCreateWithUTF8CString(BUFFER))

#define JSTString(VAL)			(bool)	 	(JSValueIsString JSTSubExec(VAL))
#define JSTStringCopy(VAL)		(char *)	(_JSTStringCopy JSTExec(VAL))
#define JSTStringDead(BUFFER)	(void)		 g_free(BUFFER)
#define JSTMakeString(BUFFER)	(JSValueRef) (_JSTMakeString JSTExec(BUFFER))

#define JSTPropertyHidden 		(kJSPropertyAttributeDontEnum)
#define JSTPropertyReadOnly 	(kJSPropertyAttributeReadOnly)
#define JSTPropertyRequired 	(kJSPropertyAttributeDontDelete)
#define JSTPropertyConst 		(JSTPropertyReadOnly | JSTPropertyRequired)
#define JSTPropertyPrivate 		(JSTPropertyHidden | JSTPropertyRequired)
#define JSTPropertyProtected 	(JSTPropertyConst | JSTPropertyPrivate)

#define JSTPointer(VAL) (void*) ((long) JSTNumber(VAL))

#define JSTObject(VAL) 					(bool)			(JSValueIsObject(ctx, VAL))
#define JSTMakeObject(VAL) 				(JSObjectRef)	(JSValueToObject(ctx, VAL, exception))
#define JSTCreateObject(CLASS, PRIV) 	(JSObjectRef)	(JSObjectMake(ctx, CLASS, (void *) PRIV))
#define JSTKnownObject(VAL)				(JSObjectRef)	(VAL)

#define JSTBoolean(VAL) 		(bool)			(JSValueToBoolean(ctx, VAL))
#define JSTMakeBoolean(BOOL) 	(JSValueRef)	(JSValueMakeBoolean(ctx, BOOL))

#define JSTUndefined(VAL)		(bool)			(JSValueIsUndefined(ctx, VAL))
#define JSTMakeUndefined()		(JSValueRef)	(JSValueMakeUndefined(ctx))

#define JSTNull(VAL)			(bool)			(JSValueIsNull(ctx, VAL))
#define JSTMakeNull()			(JSValueRef)	(JSValueMakeNull(ctx))

#define JSTNumber(VAL) 			(double)		(JSValueToNumber(ctx, VAL, exception))
#define JSTMakeNumber(DOUBLE) 	(JSValueRef)	(JSValueMakeNumber(ctx, DOUBLE))

#define JSTReference(VAL)	(bool)	( VAL && ! (JSTNull((JSValueRef)VAL)) && ! (JSTUndefined((JSValueRef)VAL)) )

#define JSTNaN(VAL)	(bool)	(JSTBoolean(JSTCall(NULL, "isNaN", VAL)))

#define JSTSetPrivate(OBJ, DATA)	(bool) 			(JSObjectSetPrivate(OBJ, (void *) DATA))
#define JSTGetPrivate(OBJ)			(void *)		(JSObjectGetPrivate(OBJ))
#define JSTGetPrivateValue(OBJ)		(JSValueRef)	(JSTGetPrivate(OBJ))
#define JSTGetPrivateObject(OBJ)	(JSObjectRef)	(JSTGetPrivate(OBJ))

#define JSTGetPrivateInteger(OBJ)			(long)	(JSTGetPrivate(OBJ))
#define JSTSetPrivateInteger(OBJ, INTEGER)	(bool)	(JSTSetPrivate(OBJ, (long) INTEGER))

#define JSTFunction(OBJ)				(bool)			(JSObjectIsFunction(ctx, OBJ))
#define JSTCoreMakeFunction(STR, PROC)	(JSObjectRef)	(JSObjectMakeFunctionWithCallback (ctx, STR, PROC))

#define JSTMakeFunction(BUFFER, PROC) (JSObjectRef) (_JSTMakeFunction JSTExec (BUFFER, PROC))

#define JSTConstructor(OBJ)				(bool)			(JSObjectIsConstructor(ctx, OBJ))
#define JSTMakeConstructor(CLASS, PROC) (JSObjectRef)	(JSObjectMakeConstructor(ctx, CLASS, PROC))

#define JSTCoreEval(STR, OBJ) (JSValueRef) (JSEvaluateScript JSTExec (STR, OBJ, NULL, 1))

#define JSTEval(BUFFER, OBJ) (JSValueRef) (_JSTEval JSTExec (BUFFER, OBJ))

#define JSTCoreProperty(STR) 		(JSValueRef)	(JSTCoreEval(STR, NULL))
#define JSTCorePropertyThis(STR)	(JSValueRef)	(JSTCoreEval(STR, thisObject))

#define JSTCoreObjectProperty(OBJ, STR) 		(JSValueRef)	(JSTCoreEval(STR, OBJ))
#define JSTSetCoreProperty(OBJ, STR, VAL, ATTR)	(void)			(JSObjectSetProperty JSTExec (OBJ, STR, VAL, ATTR))
#define JSTGetCoreProperty(OBJ, STR)			(JSValueRef)	(JSObjectGetProperty JSTExec (OBJ, STR))

#define JSTProperty(BUFFER) 	(JSValueRef)	(JSTEval(BUFFER, NULL))
#define JSTCallbackProperty(BUFFER) (JSValueRef)	(JSTGetProperty(thisObject, BUFFER))

#define JSTObjectProperty(THIS, BUFFER) 			(JSValueRef)	(JSTEval(BUFFER, THIS))
#define JSTSetProperty(OBJ, BUFFER, VAL, ATTR) 		(void)			_JSTSetProperty JSTExec (OBJ, BUFFER, VAL, ATTR)
#define JSTGetProperty(OBJ, BUFFER)					(JSValueRef)	(_JSTGetProperty JSTExec (OBJ, BUFFER))
#define JSTGetPropertyIndex(OBJ, INDEX) 			(JSValueRef)	(JSObjectGetPropertyAtIndex JSTExec (OBJ, (unsigned long) INDEX))
#define JSTSetPropertyIndex(OBJ, INDEX, VAL, ATTR) 	(void)			(JSObjectSetPropertyAtIndex JSTExec (OBJ, (unsinged long) INDEX, VAL, ATTR))

#define JSTCallFunction(THIS, FUNC, ...) 	(JSValueRef)	(_JSTCallFunction(ctx, THIS, FUNC, exception, __VA_ARGS__, NULL))
#define JSTCoreCall(THIS, STR, ...)			(JSValueRef)	(JSTCallFunction(THIS, (JSObjectRef) JSTCoreObjectProperty(THIS, STR), __VA_ARGS__, NULL))
#define JSTCall(THIS, BUFFER, ...)			(JSValueRef)	(JSTCallFunction((JSObjectRef)THIS, (JSObjectRef) JSTObjectProperty(THIS, BUFFER), __VA_ARGS__, NULL))

#define JSTCallConstructor(OBJ, ...) (JSValueRef) (_JSTCallConstructor(ctx, OBJ, exception, __VA_ARGS__, NULL))
#define JSTMakeCorePropertyFunction(OBJ, STR, PROC) (JSObjectRef) (_JSTMakeCorePropertyFunction JSTExec (OBJ, STR, PROC))
#define JSTMakePropertyFunction(OBJ, BUFFER, PROC) (JSObjectRef) (_JSTMakePropertyFunction JSTExec (OBJ, BUFFER, PROC))
#define JSTMakeCoreConstructorProperty(OBJ, STR, CLASS, PROC) (JSObjectRef) (_JSTMakeCoreConstructorProperty JSTExec (OBJ, STR, CLASS, PROC))
#define JSTMakeConstructorProperty(OBJ, BUFFER, CLASS, PROC) (JSObjectRef) (_JSTMakeConstructorProperty JSTExec (OBJ, BUFFER, CLASS, PROC))


#define JSTFarExceptionThrown (bool) (exception && *exception)
#define JSTOwnExceptionThrown(E) (bool) (E)

#define JSTMaybeReturnValueException(FMT, ...) if (JSTFarExceptionThrown) { if (FMT && *FMT) {  fprintf (stderr, FMT, __VA_ARGS__); } return JSValueMakeNull(ctx); }
#define JSTMaybeReturnCustomException(RET, FMT, ...) if (JSTFarExceptionThrown) { if (FMT && *FMT) {  fprintf (stderr, FMT, __VA_ARGS__); } RET; }
#define JSTMaybeReturnObjectException(FMT, ...) if (JSTFarExceptionThrown) { if (FMT && *FMT) {  fprintf (stderr, FMT, __VA_ARGS__); } return (JSObjectRef) JSValueMakeUndefined(ctx); }
#define JSTMaybeReturnBooleanException(BOOL, FMT, ...) if (JSTFarExceptionThrown) { if (FMT && *FMT) {  fprintf (stderr, FMT, __VA_ARGS__); } return BOOL; }

#define JSTArgument(INDEX) (JSValueRef) ((INDEX < argumentCount) ? (arguments[INDEX]) : (JSTMakeUndefined()))

#define JSTParseInt(VAL) (JSValueRef) (JSTCall(NULL, "parseInt", VAL))

#define JSTCoreEqualsString JSStringIsEqualToUTF8CString
#define JSTCoreIsEqual JSStringIsEqual

#define JSTNumberType kJSTypeNumber
#define JSTStringType kJSTypeString

typedef struct JSTCharBuffer {

    char *pointer;               /* the location of the data */
    unsigned long allocated;    /* how much data is allocated for the pointer */
    unsigned long index;        /* read/write index */
    unsigned long length;       /* known string length */
    unsigned long dynamic;      /* if its not dynamic it cannot be resized */
    
} JSTCharBuffer;

