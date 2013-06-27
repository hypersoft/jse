/* Candy Land */

#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>

// The following include is a makefile auto generated binary inclusion script
#include "JSNative.inc"

JSClassRef JSNative = NULL;

// Better performance from macros / reduced memory i/o footprint
typedef enum JSNativeTypeCode {
	JSNativeTypeVoid = 0,			JSNativeTypeBool = 10,	
	JSNativeTypeChar = 20,			JSNativeTypeShort = 30,
	JSNativeTypeInt = 40,			JSNativeTypeLong = 50,
	JSNativeTypeLongLong = 60,		JSNativeTypeFloat = 70,
	JSNativeTypeDouble = 80,		JSNativeTypePointer = 90,
} JSNativeTypeCode;

#define JSNativeUnsignedValue(V) (V % 10)

typedef struct {
	void * location;		int typeCode;	unsigned long count;
	bool constant;			bool allocated;
} JSNativePrivate;

static JSValueRef js_native_eval_scriptlet(JSContextRef ctx, char * script, JSObjectRef object, JSValueRef * exception) {
	void * translation = JSStringCreateWithUTF8CString(script);
	JSValueRef value = JSEvaluateScript(ctx, translation, object, NULL, 1, exception); // forward exception
	JSStringRelease(translation);
	return value;
}

static void js_native_throw_exception(JSContextRef ctx, char * message, JSValueRef * exception) {
	unsigned long int count = strlen(message) + 19;
	char output[count];
	output[count - 1] = 0;
	sprintf(output,"throw new Error(\"%s\");", message);
	js_native_eval_scriptlet(ctx, output, NULL, exception);
}

static bool js_native_jsstring_parse_int(JSContextRef ctx, JSStringRef s, unsigned long int * i, JSValueRef * exception) {
	unsigned long int count = JSStringGetMaximumUTF8CStringSize(s);
	char copy[count], output[count + 19];
	JSStringGetUTF8CString (s, copy, count);
	snprintf(output, count + 19, "isNaN(parseInt(\"%s\"));", copy);
	if ( ! JSValueToBoolean(ctx, js_native_eval_scriptlet(ctx, output, NULL, exception))) {
		snprintf(output, count + 11, "parseInt(\"%s\");", copy);
		*i = (unsigned long int) JSValueToNumber(ctx, js_native_eval_scriptlet(ctx, output, NULL, exception), exception);		
	} else {
		return false;
	}
	return true;
}

static JSValueRef js_native_call_function(JSContextRef ctx, char * function, JSObjectRef object, int argc, JSValueRef arguments[], JSValueRef * exception) {
	JSObjectRef call = (void*) js_native_eval_scriptlet(ctx, function, object, exception); // check exception!
	if (exception && *exception) return JSValueMakeNull(ctx);
	return JSObjectCallAsFunction(ctx, call, object, argc, arguments, exception); // forward exception
}

bool js_native_is_native_object(JSContextRef ctx, JSObjectRef object) {
	JSValueRef arguments[] = { (JSValueRef) object, NULL };
	return JSValueToBoolean(ctx, js_native_call_function(ctx, "JSNative.isNativeObject", NULL, 1, arguments, NULL));
}

static JSObjectRef js_native_construct_object (JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	JSNativePrivate * p = g_new0(JSNativePrivate, 1);
	p->allocated = true;
	JSValueRef arg[] = { arguments[0], NULL };
	JSValueRef jsTypeCode = js_native_call_function (
		ctx, "JSNative.typeCode", NULL, 1, arg, exception
	); if (exception && *exception) { g_free(p); return (JSObjectRef) JSValueMakeUndefined(ctx); }
	p->typeCode = (int) JSValueToNumber(ctx, jsTypeCode, exception);
	if (exception && *exception) { g_free(p); return (JSObjectRef) JSValueMakeUndefined(ctx); }
	if (argumentCount >= 2) {
		p->count = JSValueToNumber(ctx, arguments[1], exception);
		if (exception && *exception) { g_free(p); return (JSObjectRef) JSValueMakeUndefined(ctx); }
	} else p->count = 1;
	bool isUnsigned = JSNativeUnsignedValue(p->typeCode);
	if (isUnsigned) p->typeCode--;
	if (p->typeCode == JSNativeTypeVoid) {
		p->allocated = false; p->typeCode = JSNativeTypeVoid;
		p->location = NULL;
	} else if (p->typeCode == JSNativeTypeBool) p->location = g_new0(bool, p->count);
	else if (p->typeCode == JSNativeTypeChar) p->location = g_new0(char, p->count);
	else if (p->typeCode == JSNativeTypeShort) p->location = g_new0(short, p->count);
	else if (p->typeCode == JSNativeTypeInt) p->location = g_new0(int, p->count);
	else if (p->typeCode == JSNativeTypeLong) p->location = g_new0(long, p->count);
	else if (p->typeCode == JSNativeTypeLongLong) p->location = g_new0(long long, p->count);
	else if (p->typeCode == JSNativeTypeFloat) p->location = g_new0(float, p->count);
	else if (p->typeCode == JSNativeTypeDouble) p->location = g_new0(double, p->count);
	else if (p->typeCode == JSNativeTypePointer) p->location = g_new0(void *, p->count);
	else { js_native_throw_exception(ctx, "js_native_construct_object: unknown type code", exception);
		g_free(p); return (JSObjectRef) JSValueMakeUndefined(ctx);
	}
	if (isUnsigned) p->typeCode++;
	return JSObjectMake(ctx, JSNative, p);
}

JSValueRef js_native_get_property_at_index(JSContextRef ctx, JSNativePrivate * p, unsigned long index, JSValueRef * exception) {
	char * e[] = { // error strings
		"js_native_get_property_at_index: no native data available",
		"js_native_get_property_at_index: cannot read null base pointer",
		"js_native_get_property_at_index: cannot read range overflow",
		"js_native_get_property_at_index: cannot read void type",
		"js_native_get_property_at_index: cannot read unknown native type code",
		NULL
	};
	if ( ! p ) { js_native_throw_exception(ctx, e[0], exception); return NULL; }
	if ( ! p->location ) { js_native_throw_exception(ctx, e[1], exception); JSValueMakeNull(ctx); }
	if (index && index >= p->count || p->count == 0) { js_native_throw_exception(ctx, e[2], exception); JSValueMakeNull(ctx); }
	if (p->typeCode == JSNativeTypeVoid) { js_native_throw_exception(ctx, e[3], exception); JSValueMakeNull(ctx); }
	if (JSNativeUnsignedValue(p->typeCode)) {
		if (p->typeCode == JSNativeTypeChar + 1) return JSValueMakeNumber(ctx, (double) (unsigned char) *(unsigned char*)(p->location + index));
		else if (p->typeCode == JSNativeTypeShort + 1) return JSValueMakeNumber(ctx, (double) *(unsigned short*)(p->location + index));
		else if (p->typeCode == JSNativeTypeInt + 1) return JSValueMakeNumber(ctx, (double) *(unsigned int*)(p->location + index));
		else if (p->typeCode == JSNativeTypeLong + 1) return JSValueMakeNumber(ctx, (double) *(unsigned long*)(p->location + index));
		else if (p->typeCode == JSNativeTypeLongLong + 1) return JSValueMakeNumber(ctx, (double) *(unsigned long long*)(p->location + index));
		else { js_native_throw_exception(ctx, e[4], exception); return JSValueMakeNull(ctx); }
	} else {
		if (p->typeCode == JSNativeTypeBool) return JSValueMakeNumber(ctx, (double) *(bool*)(p->location + index));
		else if (p->typeCode == JSNativeTypeChar) return JSValueMakeNumber(ctx, (double) *(char*)(p->location + index));
		else if (p->typeCode == JSNativeTypeShort) return JSValueMakeNumber(ctx, (double) *(short*)(p->location + index));
		else if (p->typeCode == JSNativeTypeInt) return JSValueMakeNumber(ctx, (double) *(int*)(p->location + index));
		else if (p->typeCode == JSNativeTypeLong) return JSValueMakeNumber(ctx, (double) *(long*)(p->location + index));
		else if (p->typeCode == JSNativeTypeLongLong) return JSValueMakeNumber(ctx, (double) *(long long*)(p->location + index));
		else if (p->typeCode == JSNativeTypeFloat) return JSValueMakeNumber(ctx, (double) *(float*)(p->location + index));
		else if (p->typeCode == JSNativeTypeDouble) return JSValueMakeNumber(ctx, (double) *(double*)(p->location + index));
		else if (p->typeCode == JSNativeTypePointer) return JSValueMakeNumber(ctx, (double) *(unsigned long*)(p->location + index));
		else { js_native_throw_exception(ctx, e[4], exception); return JSValueMakeNull(ctx); }
	}
	return NULL;
}

static JSValueRef js_native_get_property (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) {
	JSNativePrivate * p = JSObjectGetPrivate(object); if (!p) return NULL;
	if ( ! js_native_is_native_object(ctx, object) ) {
		js_native_throw_exception(ctx, "js_native_get_property: not a JSNative object", exception);
		return NULL;
	}
	unsigned long index = 0;
	if (JSStringIsEqualToUTF8CString(propertyName, "pointer")) {
		return JSValueMakeNumber(ctx, (double) (unsigned long) p->location);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "value")) {
		return js_native_get_property_at_index(ctx, p, 0, exception);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "type")) {
		return JSValueMakeNumber(ctx, (double) p->typeCode);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "count")) {
		return JSValueMakeNumber(ctx, (double) (unsigned long) p->count);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "constant")) {
		return JSValueMakeBoolean(ctx, p->constant);
	} else if (js_native_jsstring_parse_int(ctx, propertyName, &index, exception)) {
		return js_native_get_property_at_index(ctx, p, index, exception);
	}
	return NULL;
}

bool js_native_set_property_at_index(JSContextRef ctx, JSNativePrivate * p, unsigned long index, JSValueRef value, JSValueRef * exception) {
	char * e[] = { // error strings
		"js_native_set_property_at_index: no native data available",
		"js_native_set_property_at_index: cannot write null base pointer",
		"js_native_set_property_at_index: cannot write range overflow",
		"js_native_set_property_at_index: cannot write void type",
		"js_native_set_property_at_index: cannot write constant value",
		"js_native_set_property_at_index: cannot write unknown native type code",
		NULL
	};
	if ( ! p ) { js_native_throw_exception(ctx, e[0], exception); return false; }
	if ( ! p->location ) { js_native_throw_exception(ctx, e[1], exception); return true; }
	if (index && index >= p->count || p->count == 0) { js_native_throw_exception(ctx, e[2], exception); return true; }
	if (p->typeCode == JSNativeTypeVoid) { js_native_throw_exception(ctx, e[3], exception); return true; }
	if ( p->constant ) { js_native_throw_exception(ctx, e[4], exception); return true; }
	if (JSNativeUnsignedValue(p->typeCode)) {
		if (p->typeCode == JSNativeTypeChar + 1) { *(unsigned char*)(p->location + index) = (unsigned char) JSValueToNumber(ctx, value, exception); return true; }
		else if (p->typeCode == JSNativeTypeShort + 1) { *(unsigned short*)(p->location + index) = (unsigned short) JSValueToNumber(ctx, value, exception); return true; }
		else if (p->typeCode == JSNativeTypeInt + 1) { *(unsigned int*)(p->location + index) = (unsigned int) JSValueToNumber(ctx, value, exception); return true; }
		else if (p->typeCode == JSNativeTypeLong + 1) { *(unsigned long*)(p->location + index) = (unsigned long) JSValueToNumber(ctx, value, exception); return true; }
		else if (p->typeCode == JSNativeTypeLongLong + 1) { *(unsigned long long*)(p->location + index) = (unsigned long long) JSValueToNumber(ctx, value, exception); return true; }
		else { js_native_throw_exception(ctx, e[5], exception); return true; }
	} else {
		if (p->typeCode == JSNativeTypeBool) { *(bool*)(p->location + index) = JSValueToBoolean(ctx, value); return true; }
		else if (p->typeCode == JSNativeTypeChar) { *(char*)(p->location + index) = (char) JSValueToNumber(ctx, value, exception); return true; }
		else if (p->typeCode == JSNativeTypeShort) { *(short*)(p->location + index) = (short) JSValueToNumber(ctx, value, exception); return true; }
		else if (p->typeCode == JSNativeTypeInt) { *(int*)(p->location + index) = (int) JSValueToNumber(ctx, value, exception); return true; }
		else if (p->typeCode == JSNativeTypeLong) { *(long*)(p->location + index) = (long) JSValueToNumber(ctx, value, exception); return true; } 
		else if (p->typeCode == JSNativeTypeLongLong) { *(long long*)(p->location + index) = (long long) JSValueToNumber(ctx, value, exception); return true; } 
		else if (p->typeCode == JSNativeTypeFloat) { *(float*)(p->location + index) = (float) JSValueToNumber(ctx, value, exception); return true; } 
		else if (p->typeCode == JSNativeTypeDouble) { *(double*)(p->location + index) = JSValueToNumber(ctx, value, exception); return true; } 
		else if (p->typeCode == JSNativeTypePointer) { *(unsigned long*)(p->location + index) = (unsigned long) JSValueToNumber(ctx, value, exception); return true; }
		else { js_native_throw_exception(ctx, e[5], exception); return true; }
	}
	return false;
}

static bool js_native_set_property (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
	JSNativePrivate * p = JSObjectGetPrivate(object); if (!p) { return false; }
	if ( ! js_native_is_native_object(ctx, object) ) {
		js_native_throw_exception(ctx, "js_native_set_property: not a JSNative object", exception);
		return false;
	}
	unsigned long index = 0;
	char * e[] = { // error strings
		"js_native_set_property: pointer is allocated, read only",
		"js_native_set_property: pointer is allocated, count is read only",
		NULL
	};
	if (JSStringIsEqualToUTF8CString(propertyName, "pointer")) {
		if (p->allocated) { js_native_throw_exception(ctx, e[0], exception); return true; }
		p->location = (void *) (unsigned long) JSValueToNumber(ctx, value, exception); return true;
	} else if (JSStringIsEqualToUTF8CString(propertyName, "value")) {
		return js_native_set_property_at_index(ctx, p, 0, value, exception);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "type")) {
		p->typeCode = (unsigned int) JSValueToNumber(ctx, value, exception); return true;
	} else if (JSStringIsEqualToUTF8CString(propertyName, "count")) {
		if (p->allocated) { js_native_throw_exception(ctx, e[1], exception); return true; }
		p->count = (unsigned long) JSValueToNumber(ctx, value, exception); return true;
	} else if (JSStringIsEqualToUTF8CString(propertyName, "constant")) {
		p->constant = JSValueToBoolean(ctx, value); return true;
	} else if (js_native_jsstring_parse_int(ctx, propertyName, &index, exception)) {
		return js_native_set_property_at_index(ctx, p, index, value, exception);
	}
	return false;
}

JSValueRef js_native_free(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	JSNativePrivate * p = JSObjectGetPrivate(thisObject);
	if (p) {
		if ( ! js_native_is_native_object(ctx, thisObject) ) {
			js_native_throw_exception(ctx, "js_native_free: not a JSNative object", exception);
			return JSValueMakeNull(ctx);
		}
		if (p->allocated && p->location) g_free(p->location);
		g_free(p); JSObjectSetPrivate(thisObject, NULL);
	}
	return JSValueMakeNull(ctx);
}

JSClassRef js_native_define_class(JSContextRef ctx, JSObjectRef object) {
	JSClassDefinition jsNative = kJSClassDefinitionEmpty;
	jsNative.className = "JSNative";
	jsNative.callAsConstructor = &js_native_construct_object;
	jsNative.setProperty = &js_native_set_property;
	jsNative.getProperty = &js_native_get_property;
	JSStaticFunction StaticFunctionArray[] = {
	    { "free", &js_native_free, kJSPropertyAttributeDontDelete  }, { 0, 0, 0 }
	};
	jsNative.staticFunctions = StaticFunctionArray;
	JSNative = JSClassRetain(JSClassCreate(&jsNative));
	JSStringRef jname = JSStringCreateWithUTF8CString("JSNative");
	JSObjectSetProperty(ctx, object, jname, JSObjectMake(ctx, JSNative, NULL), 0, NULL);
	JSStringRelease (jname);
	JSEvaluateScript(ctx, JSStringCreateWithUTF8CString(JSNativeSupport), NULL, NULL, 1, NULL);
	return JSNative;
}
