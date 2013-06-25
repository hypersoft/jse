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

static JSValueRef js_native_call_function(JSContextRef ctx, char * function, JSObjectRef object, int argc, JSValueRef arguments[], JSValueRef * exception) {
	JSObjectRef call = (void*) js_native_eval_scriptlet(ctx, function, object, exception); // check exception!
	return JSObjectCallAsFunction(ctx, call, object, argc, arguments, exception); // forward exception
}

static JSObjectRef js_native_construct_object (JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	JSNativePrivate * p = g_new0(JSNativePrivate, 1);
	p->allocated = true;
	JSValueRef arg[] = { arguments[0], NULL };
	JSValueRef jsTypeCode = js_native_call_function (
		ctx, "JSNative.typeCode", NULL, 1, arg, exception
	); // check exception!
	p->typeCode = (int) JSValueToNumber(ctx, jsTypeCode, exception); // check exception!
	if (argumentCount >= 2) {
		p->count = JSValueToNumber(ctx, arguments[1], exception); // check exception!
	} else p->count = 1;
	bool isUnsigned = JSNativeUnsignedValue(p->typeCode);
	if (isUnsigned) p->typeCode--;
	if (p->typeCode == JSNativeTypeVoid) {
		p->allocated = false; p->typeCode = JSNativeTypeVoid;
		p->location = NULL; p->count = p->count;
	} else if (p->typeCode == JSNativeTypeBool) p->location = g_new0(bool, p->count);
	else if (p->typeCode == JSNativeTypeChar) p->location = g_new0(char, p->count);
	else if (p->typeCode == JSNativeTypeShort) p->location = g_new0(short, p->count);
	else if (p->typeCode == JSNativeTypeInt) p->location = g_new0(int, p->count);
	else if (p->typeCode == JSNativeTypeLong) p->location = g_new0(long, p->count);
	else if (p->typeCode == JSNativeTypeLongLong) p->location = g_new0(long long, p->count);
	else if (p->typeCode == JSNativeTypeFloat) p->location = g_new0(float, p->count);
	else if (p->typeCode == JSNativeTypeDouble) p->location = g_new0(double, p->count);
	else if (p->typeCode == JSNativeTypePointer) p->location = g_new0(void *, p->count);
	if (isUnsigned) p->typeCode++;
	return JSObjectMake(ctx, JSNative, p);
}

JSValueRef js_native_get_property_at_index(JSContextRef ctx, JSNativePrivate * p, unsigned long index) {
	if ( ! p || ! p->location ) return NULL;
	if (index && index >= p->count) return NULL;
	if (p->typeCode == JSNativeTypeVoid) return JSValueMakeNull(ctx);
	if (JSNativeUnsignedValue(p->typeCode)) {
		if (p->typeCode == JSNativeTypeChar + 1) return JSValueMakeNumber(ctx, (double) (unsigned char) *(unsigned char*)(p->location + index));
		else if (p->typeCode == JSNativeTypeShort + 1) return JSValueMakeNumber(ctx, (double) *(unsigned short*)(p->location + index));
		else if (p->typeCode == JSNativeTypeInt + 1) return JSValueMakeNumber(ctx, (double) *(unsigned int*)(p->location + index));
		else if (p->typeCode == JSNativeTypeLong + 1) return JSValueMakeNumber(ctx, (double) *(unsigned long*)(p->location + index));
		else if (p->typeCode == JSNativeTypeLongLong + 1) return JSValueMakeNumber(ctx, (double) *(unsigned long long*)(p->location + index));
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
	}
	return NULL;
}

static JSValueRef js_native_get_property (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) {
	JSNativePrivate * p = JSObjectGetPrivate(object);
	if (!p) return NULL;
	if (!p->location) return NULL;
	if (JSStringIsEqualToUTF8CString(propertyName, "pointer")) {
		return JSValueMakeNumber(ctx, (double) (unsigned long) p->location);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "value")) {
		return js_native_get_property_at_index(ctx, p, 0);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "type")) {
		return JSValueMakeNumber(ctx, (double) p->typeCode);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "count")) {
		return JSValueMakeNumber(ctx, (double) (unsigned long) p->count);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "constant")) {
		return JSValueMakeBoolean(ctx, p->constant);
	}
	unsigned long length = JSStringGetMaximumUTF8CStringSize (propertyName);
	char * buf = g_malloc (length * sizeof (gchar));
	JSStringGetUTF8CString (propertyName, buf, length);
	if (! isdigit(*buf) ) {
		g_free(buf); return NULL;
	} else {
		unsigned long index = atol(buf); g_free(buf);
		return js_native_get_property_at_index(ctx, p, index);
	}
}

bool js_native_set_property_at_index(JSContextRef ctx, JSNativePrivate * p, unsigned long index, JSValueRef value) {
	if (! p || ! p->location || ! p->count || p->constant ) return false;
	if (index && index >= p->count) return false;
	if (JSNativeUnsignedValue(p->typeCode)) {
		if (p->typeCode == JSNativeTypeChar + 1) { *(unsigned char*)(p->location + index) = (unsigned char) JSValueToNumber(ctx, value, NULL); return true; }
		else if (p->typeCode == JSNativeTypeShort + 1) { *(unsigned short*)(p->location + index) = (unsigned short) JSValueToNumber(ctx, value, NULL); return true; }
		else if (p->typeCode == JSNativeTypeInt + 1) { *(unsigned int*)(p->location + index) = (unsigned int) JSValueToNumber(ctx, value, NULL); return true; }
		else if (p->typeCode == JSNativeTypeLong + 1) { *(unsigned long*)(p->location + index) = (unsigned long) JSValueToNumber(ctx, value, NULL); return true; }
		else if (p->typeCode == JSNativeTypeLongLong + 1) { *(unsigned long long*)(p->location + index) = (unsigned long long) JSValueToNumber(ctx, value, NULL); return true; }
	} else {
		if (p->typeCode == JSNativeTypeBool) { *(bool*)(p->location + index) = JSValueToBoolean(ctx, value); return true; }
		else if (p->typeCode == JSNativeTypeChar) { *(char*)(p->location + index) = (char) JSValueToNumber(ctx, value, NULL); return true; }
		else if (p->typeCode == JSNativeTypeShort) { *(short*)(p->location + index) = (short) JSValueToNumber(ctx, value, NULL); return true; }
		else if (p->typeCode == JSNativeTypeInt) { *(int*)(p->location + index) = (int) JSValueToNumber(ctx, value, NULL); return true; }
		else if (p->typeCode == JSNativeTypeLong) { *(long*)(p->location + index) = (long) JSValueToNumber(ctx, value, NULL); return true; } 
		else if (p->typeCode == JSNativeTypeLongLong) { *(long long*)(p->location + index) = (long long) JSValueToNumber(ctx, value, NULL); return true; } 
		else if (p->typeCode == JSNativeTypeFloat) { *(float*)(p->location + index) = (float) JSValueToNumber(ctx, value, NULL); return true; } 
		else if (p->typeCode == JSNativeTypeDouble) { *(double*)(p->location + index) = JSValueToNumber(ctx, value, NULL); return true; } 
		else if (p->typeCode == JSNativeTypePointer) { *(unsigned long*)(p->location + index) = (unsigned long) JSValueToNumber(ctx, value, NULL); return true; }
	}
	return false;
}

static bool js_native_set_property (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
	JSNativePrivate * p = JSObjectGetPrivate(object);
	if (!p) return false;
	if (JSStringIsEqualToUTF8CString(propertyName, "pointer")) {
		if (p->allocated) return false;
		p->location = (void *) (unsigned long) JSValueToNumber(ctx, value, NULL);
		return true;
	} else if (JSStringIsEqualToUTF8CString(propertyName, "value")) {
		return js_native_set_property_at_index(ctx, p, 0, value);
	} else if (JSStringIsEqualToUTF8CString(propertyName, "type")) {
		p->typeCode = (unsigned int) JSValueToNumber(ctx, value, NULL);
		return true;
	} else if (JSStringIsEqualToUTF8CString(propertyName, "count")) {
		if (p->allocated) return false;
		p->count = (unsigned long) JSValueToNumber(ctx, value, NULL);
		return true;
	} else if (JSStringIsEqualToUTF8CString(propertyName, "constant")) {
		p->constant = JSValueToBoolean(ctx, value, NULL);
		return true;
	}
	if (p->count > 1) {
	unsigned long length = JSStringGetMaximumUTF8CStringSize (propertyName);
	void * buf = g_malloc (length * sizeof (gchar));
	JSStringGetUTF8CString (propertyName, buf, length);
	unsigned long index = atol(buf); g_free(buf);
	return js_native_set_property_at_index(ctx, p, index, value);
	}
	return false;
}

// The callback invoked when determining whether an object has a property.
static bool js_native_has_property(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName) {
	JSNativePrivate * p = JSObjectGetPrivate(object);
	if (!p) return false;
	if (JSStringIsEqualToUTF8CString(propertyName, "pointer")) return true;
	if (JSStringIsEqualToUTF8CString(propertyName, "value")) return true;
	if (JSStringIsEqualToUTF8CString(propertyName, "type")) return true;
	if (JSStringIsEqualToUTF8CString(propertyName, "count")) return true;
	if (JSStringIsEqualToUTF8CString(propertyName, "constant")) return true;
	return false;
}

JSValueRef js_native_free(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	JSNativePrivate * p = JSObjectGetPrivate(thisObject);
	if (p) {
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
