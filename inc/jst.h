/*

Hypersoft Systems JST AND Hypersoft System JSE Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list 
   of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this 
   list of conditions and the following disclaimer in the documentation and/or other 
   materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGE.

*/

/* Architecture/Compiler/Platform Independent Stuff Here */

#include <string.h>

#define jst_type_1 1
#define jst_type_2 2
#define jst_type_4 4
#define jst_type_8 8
#define jst_type_void 16
#define jst_type_constant 32
#define jst_type_integer 64
#define jst_type_signed 128
#define jst_type_utf 256
#define jst_type_value 512
#define jst_type_string 1024
#define jst_type_reference 2048
#define jst_type_array 4096
#define jst_type_struct 8192
#define jst_type_union 16384
#define jst_type_dynamic 32768
#define jst_type_host 65536
#define jst_type_network 131072

/*
	These codes are typically an enumeration of codes to be used with JSTScriptNativeError
	These codes should sync with the error codes found in src/jst/script/error.js
*/

#define JST_ERROR 2
#define JST_SYNTAX_ERROR 3
#define JST_TYPE_ERROR 4
#define JST_RANGE_ERROR 5
#define JST_REFERENCE_ERROR 6
#define JST_EVAL_ERROR 7
#define JST_URI_ERROR 8

#define JSTXPR(r, ...) (__VA_ARGS__, r)
#define JSTCND(e, t, f) ((e) ? (t) : (f))

#define bitsof(v) (sizeof(v) << 3)

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>

#include "JavaScriptCore/JavaScript.h"
#include "JavaScriptCore/JSBase.h"
#include "JavaScriptCore/JSContextRef.h"
#include "JavaScriptCore/JSObjectRef.h"
#include "JavaScriptCore/JSStringRef.h"
#include "JavaScriptCore/JSValueRef.h"
#include "JavaScriptCore/WebKitAvailability.h"

#define JSTValueTypeUndefined kJSTypeUndefined
#define JSTValueTypeNull kJSTypeNull
#define JSTValueTypeBoolean kJSTypeBoolean
#define JSTValueTypeNumber kJSTypeNumber
#define JSTValueTypeString kJSTypeString
#define JSTValueTypeObject kJSTypeObject

#define JSTString JSStringRef
#define JSTContext JSContextRef
#define JSTValue JSValueRef
#define JSTValueNull JSValueMakeNull(ctx)
#define JSTValueUndefined JSValueMakeUndefined(ctx)
#define JSTObject JSObjectRef
#define JSTObjectUndefined ((JSTObject) JSTValueUndefined)
#define JSTObjectNull ((JSTObject) JSTValueNull)
#define JSTObjectToValue(o) ((JSTValue) o)
#define JSTClass JSClassRef
#define JSTClassAccessor JSStaticValue
#define JSTClassFunction JSStaticFunction
#define JSTClassDefinition JSClassDefinition
#define JSTClassEmptyDefinition kJSClassDefinitionEmpty
#define JSTClassCreate(d) JSClassCreate(d)
#define JSTClassRetain(c) JSClassRetain(c)
#define JSTClassRelease(c) JSClassRelease(c)
#define JSTClassPropertyNone kJSClassAttributeNone
#define JSTClassPropertyManualPrototype kJSClassAttributeNoAutomaticPrototype
#define JSTContext JSContextRef
#define JSTContextGroup JSContextGroupRef
#define JSTContextGroupCreate JSContextGroupCreate
#define JSTContextGroupRetain JSContextGroupRetain
#define JSTContextGroupRelease JSContextGroupRelease
#define JSTContextCreate JSGlobalContextCreate
#define JSTContextCreateInGroup JSGlobalContextCreateInGroup
#define JSTContextRetain JSGlobalContextRetain
#define JSTContextRelease JSGlobalContextRelease
#define JSTContextGetGlobalObject JSContextGetGlobalObject
#define JSTContextGetGroup JSContextGetGroup

#define JSTDeclareInitializer(n) void n (register JSTContext ctx, JSTObject object)
#define JSTDeclareFinalizer(n) void n (register JSTObject object)
#define JSTDeclareHasProperty(n) bool n (register JSTContext ctx, JSTObject object, JSTString propertyName)
#define JSTDeclareGetProperty(n) JSTValue n (register JSTContext ctx, JSTObject object, JSTString propertyName, JSTValue* exception)
#define JSTDeclareSetProperty(n) bool n (register JSTContext ctx, JSTObject object, JSTString propertyName, JSTValue value, JSTValue* exception)
#define JSTDeclareDeleteProperty(n) bool n (register JSTContext ctx, JSTObject object, JSTString propertyName, JSTValue* exception)
#define JSTDeclareGetPropertyNames(n) void n (register JSTContext ctx, JSTObject object, JSPropertyNameAccumulatorRef propertyNames)
#define JSTDeclareFunction(...) (register JSTContext ctx, JSObjectRef function, JSObjectRef this, size_t argc, const JSTValue argv[], JSTValue* exception)
#define JSTDeclareConstructor(n) JSTObject n (register JSTContext ctx, JSTObject constructor, size_t argc, const JSTValue argv[], JSTValue* exception)
#define JSTDeclareHasInstance(n) bool n (register JSTContext ctx, JSTObject constructor, JSTValue possibleInstance, JSTValue* exception)
#define JSTDeclareConvertor(n) JSTValue n (register JSTContext ctx, JSTObject object, JSType type, JSTValue* exception)

#define JSTInit(o, c, a, e) JSTInit_(ctx, o, c, a, e, exception)
#define JSTClassInstance(c, p) JSObjectMake(ctx, c, p)
#define JSTConstructorCallback(c, f) JSObjectMakeConstructor(ctx, c, f)

#define JSTFunctionCallback(p, f) JSTFunctionCallback_(ctx, p, f, exception)
#define JSTObjectGetPrototype(o) (JSTObject) JSObjectGetPrototype(ctx, (JSTObject) o)
#define JSTObjectGetConstructor(o) (JSTObject)JSTObjectGetProperty(JSTObjectGetPrototype(o), "constructor")
#define JSTObjectSetPrototype(o, p) JSObjectSetPrototype(ctx, (JSTObject) o, (JSTObject) p)
#define JSTObjectHasProperty(o, p) JSTObjectHasProperty_(ctx, o, p, exception)
#define JSTObjectGetProperty(o, p) JSTObjectGetProperty_(ctx, (JSTObject) o, p, exception)
#define JSTObjectGetObject(o, p) ((JSTObject) JSTObjectGetProperty((JSTObject)o, p))
#define JSTObjectSetProperty(o, p, v, a) JSTObjectSetProperty_(ctx, o, p, v, a, exception)
#define JSTObjectDeleteProperty(o, p) JSTObjectDeleteProperty_(ctx, o, p, exception)
#define JSTObjectGetPropertyAtIndex(o, i) JSObjectGetPropertyAtIndex(ctx,(JSTObject) o, i, exception)
#define JSTObjectSetPropertyAtIndex(o, i, v) JSObjectSetPropertyAtIndex(ctx, o, i, v, exception)
#define JSTObjectGetPrivate(o) JSObjectGetPrivate(o)
#define JSTObjectSetPrivate(o, p) JSObjectSetPrivate(o, p)
#define JSTObjectSetMethod(o, n, m, a) JSTObjectSetMethod_(ctx, o, n, m, a, exception)
#define JSTObjectSetConstructor(o, n, c, m, a) JSTObjectSetConstructor_(ctx, o, n, c, m, a, exception)
#define JSTObjectIsFunction(o) JSObjectIsFunction(ctx, o)
#define JSTObjectIsConstructor(o) JSObjectIsConstructor(ctx, o)
#define JSTObjectArray(argc, argv) JSObjectMakeArray(ctx, argc, argv, exception)

#define JSTObjectPropertyHidden kJSPropertyAttributeDontEnum
#define JSTObjectPropertyReadOnly kJSPropertyAttributeReadOnly
#define JSTObjectPropertyRequired kJSPropertyAttributeDontDelete
#define JSTObjectPropertyAPI (JSTObjectPropertyHidden | JSTObjectPropertyReadOnly | JSTObjectPropertyRequired)

#define JSTScriptEval(p1, o, p2, i) JSTScriptEval_(ctx, p1, o, p2, i, exception)
#define JSTScriptCheckSyntax(p1, p2, i) JSTScriptCheckSyntax_(ctx, p1, p2, i, exception)
#define JSTScriptNativeEval(pScript, oThis) JSTScriptEval(pScript, (JSTObject) oThis, __FILE__, __LINE__)

#define JSTScriptError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("Error", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptSyntaxError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("SyntaxError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptTypeError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("TypeError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptRangeError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("RangeError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptReferenceError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("ReferenceError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptEvalError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("EvalError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptURIError(p) JSTConstructorCall((JSTObject)JSTScriptNativeEval("URIError", NULL), JSTStringToValue(JSTStringFromUTF8(p), true))
#define JSTScriptSetError(e) (*exception = e)
#define JSTScriptHasError (!JSTValueIsVoid(*exception) && JSTValueIsObject(*exception))
#define JSTScriptReportException() (JSTScriptReportException_(ctx, exception))
#define JSTScriptFunction(sName, iParamCount, pArgNames, sBody, sURL, iLine) JSObjectMakeFunction(ctx, sName, iParamCount, pArgNames, sBody sURL, iLine, exception)
#define JSTStringRelease(s) JSStringRelease(s)
#define JSTStringFromUTF8(p) JSStringCreateWithUTF8CString(p)
#define JSTStringFreeUTF8(p) g_free(p)
#define JSTStringFreeUTF32(p) g_free(p)
#define JSTStringFromUTF16(b, c) JSStringCreateWithCharacters(b, c)
#define JSTStringUTF16(s) JSStringGetCharactersPtr(s)
#define JSTStringUTF16Length(s) JSStringGetLength(s)
#define JSTStringRetain(s) JSStringRetain(s)
#define JSTStringCompare(s1, s2) JSStringIsEqual(s1, s2)
#define JSTStringCompareToUTF8(s, p) JSStringIsEqualToUTF8CString(s, p)
#define JSTStringToValue(s, r) JSTValueFromString(s, r)
#define JSTStringFromValue(v) JSTValueToString(v)

#define JSTValueGetType(v) JSValueGetType(ctx, v)
#define JSTValueIsUndefined(v) JSValueIsUndefined(ctx, v)
#define JSTValueIsNull(v) JSValueIsNull(ctx, v)
#define JSTValueIsBoolean(v) JSValueIsBoolean(ctx, v)
#define JSTValueIsNumber(v) JSValueIsNumber(ctx, v)
#define JSTValueIsString(v) JSValueIsString(ctx, v)
#define JSTValueIsObject(v) JSValueIsObject(ctx, v)
#define JSTValueIsClassInstance(v, c) JSValueIsObjectOfClass(ctx, v, c)
#define JSTValueIsEqual(v1, v2) JSValueIsEqual(ctx, v1, v2, exception)
#define JSTValueIsStrictEqual(v1, v2) JSValueIsStrictEqual(ctx, v1, v2)
#define JSTValueIsConstructorInstance(v, c) JSValueIsInstanceOfConstructor(ctx, v, c)

#define JSTValueFromBoolean(b) JSValueMakeBoolean(ctx, b)
#define JSTValueFromDouble(d) JSValueMakeNumber(ctx, d)
#define JSTValueFromString(s, r) JSTValueFromString_(ctx, s, r, exception)
#define JSTValueFromUTF8(p) JSTValueFromString(JSTStringFromUTF8(p), true)
#define JSTValueFromJSON(p) JSTValueFromJSON(ctx, p, exception)

#define JSTValueToJSON(v, indent) JSTStringToUTF8(JSValueCreateJSONString(ctx, v, indent, exception), true)
#define JSTValueToBoolean(v) JSValueToBoolean(ctx, v)
#define JSTValueToDouble(v) JSValueToNumber(ctx, v, exception)
#define JSTValueToString(v) JSValueToStringCopy(ctx, v, exception)
#define JSTValueToObject(v) JSValueToObject(ctx, v, exception)
#define JSTValueToUTF8(v) JSTStringToUTF8(JSTValueToString(v), true)
#define JSTValueProtect(v) JSValueProtect(ctx, v)
#define JSTValueUnprotect(v) JSValueUnprotect(ctx, v)
#define JSTValueParseInt(v) JSTScriptNativeEval(JSTEvalInt, v)

#define JSTValueIsVoid(v) (!v || JSTValueIsNull(v) || JSTValueIsUndefined(v))
#define JSTValueIsFunction(v) (v && JSTValueIsObject(v) && JSTObjectIsFunction(v))
#define JSTValueIsConstructor(v) (v && JSTValueIsObject(v) && JSTObjectIsConstructor(v))
#define JSTValueIsNaN(v) JSTValueToBoolean(JSTScriptNativeEval(JSTEvalNaN, v))

#define JSTArgumentToInt(i, d) JSTArgumentToInt_(ctx, argc, (const struct OpaqueJSValue **)argv, bitsof(*d), i, d, exception)
#define JSTArgumentToPointer(i, d) JSTArgumentToPointer_(ctx, argc, (const struct OpaqueJSValue **)argv, i, d, exception)
#define JSTValueToInt(v, d) JSTValueToInt_(ctx, bitsof(*d), v, d, exception)
#define JSTValueToPointer(v, d) JSTValueToPointer_(ctx, v, d, exception)
#define JSTValueFromPointer(p) ((JSTValue) JSTValueFromDouble(((unsigned long) p)))

/* Test system compiler */
#ifdef __GNUC__
	#include "jst-comp-gcc.h"
#elif defined (_MSC_VER)
	#if _MSC_VER >= 1600
		#include "jst-comp-vs-10.h"
	#else
		#error Compiler Not Supported
	#endif
#else
	/* Not listed? Check: http://sourceforge.net/p/predef/wiki/Compilers/ */
	/* you will also need to make sure that your OS & Architecture detection is up to spec */
	#error Compiler Not Supported
#endif

/* Test system architecture */
#if defined (__ia64__) || defined(_M_IA64)
#define JSTArchitecture 64
#elif defined (_M_IX86) || defined (i386)
#define JSTArchitecture 32
#else
/* Not listed? Check: http://sourceforge.net/p/predef/wiki/Architectures/ */
#error CPU Architecture Not Supported
#endif

/* Test operating system */
#ifdef __gnu_linux__
#define JSTOperatingSystem "GNU/Linux"
#include "jst-os-linux.h"
#elif __ANDROID__   
#define JSTOperatingSystem "Android"
#include "jst-os-linux.h"
#elif defined (_WIN64) || defined (_WIN32)
#define JSTOperatingSystem "Windows"
/*
  you will need to add a jst-os-windows.h file to .\inc providing the missing data
  you should be clear to comment out this error, to allow the compiler to notify
  you of the missing symbols required.
*/
//#include "jst-os-windows.h"
#error No Microsoft Windows Solution known for missing Linux headers!
/* add other operating systems as supported */
/*
#elif __BEOS__    
#define JSTOperatingSystem "BeOS"
#elif __FreeBSD__
#define JSTOperatingSystem "FreeBSD"
#elif __NetBSD__
#define JSTOperatingSystem "NetBSD"
#elif __OpenBSD__
#define JSTOperatingSystem "OpenBSD"
#elif __bsdi__
#define JSTOperatingSystem "BSD/OS"
#elif __CYGWIN__
#define JSTOperatingSystem "Cygwin"
#elif __DragonFly__
#define JSTOperatingSystem "DragonFly"
#elif __gnu_hurd__
#define JSTOperatingSystem "GNU/Hurd"
#elif macintosh
#define JSTOperatingSystem "MacOS"
#elif defined (__APPLE__) && defined (__MACH__)
#define JSTOperatingSystem "MacOS X"
#elif OS2
#define JSTOperatingSystem "OS/2"
#elif __SYMBIAN32__
#define JSTOperatingSystem "Symbian OS"
*/
#else
/* Not listed? Check: http://sourceforge.net/p/predef/wiki/OperatingSystems/ */
#error Operating System Not Supported
#endif

#ifndef JSTUtility
// stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick
#warning See URL in comments
#error You do not have a compiler known to support optional variadic macro arguments.
#endif

#define JSTUTF8 char
#define JSTUTF16 char16_t
#define JSTUTF32 char32_t

extern const char * JSTEvalNaN;
extern const char * JSTEvalInt;

extern const char * JSTReservedAddress;

extern const unsigned char SoftwareLicenseText[];
extern const unsigned char SoftwareNoticeText[];

extern const char * CODENAME;
extern const char * VERSION;
extern const char * VENDOR;

extern bool JSTArgumentToInt_ JSTUtility(int argc, JSTValue argv[], int bits, int index, void * dest);
extern bool JSTValueToInt_ JSTUtility(int bits, JSTValue input, void * dest);
extern bool JSTArgumentToPointer_ JSTUtility(int argc, JSTValue argv[], int index, void * dest);
extern bool JSTValueToPointer_ JSTUtility(JSTValue input, void * dest);

extern JSTObject JSTConstructorCall_(register JSTContext ctx, JSTValue *exception, JSTObject c, ...);
extern JSTValue JSTFunctionCall_(register JSTContext ctx, JSTValue *exception, JSTObject method, JSTObject object, ...);
extern JSTObject JSTFunctionCallback_ JSTUtility(char * p, void * f);
extern JSTObject JSTObjectSetMethod_ JSTUtility(JSTObject o, char * n, void * m, int a);
extern JSTObject JSTObjectSetConstructor_ JSTUtility(JSTObject o, char * n, JSTClass c, void * m, int a);
extern bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p);
extern void * JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a);
extern void * JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p);
extern bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p);
extern JSTValue JSTScriptEval_ JSTUtility(const char *p1, JSTObject o, const char * p2, size_t i);
extern bool JSTScriptCheckSyntax_ JSTUtility(char *p1, char *p2, size_t i);
extern void * JSTScriptNativeError_(JSTContext ctx, JSTValue *exception, const char * file, int line, int errorType, const char * format, ...);
extern int JSTScriptReportException_(JSTContext ctx, JSTValue *exception);
extern char * JSTStringToUTF8 (JSTString s, bool release);
extern UTF32 * JSTStringToUTF32(register JSTString jss, size_t len, bool release);
extern JSTString JSTStringFromUTF32(register const gunichar *ucs4, size_t len, bool release);
extern JSTValue JSTValueFromString_ JSTUtility(JSTString s, bool release);
extern JSTValue JSTValueFromJSON_ JSTUtility(char * p);
extern char * JSTConstructUTF8(char * format, ...);
extern JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]);

