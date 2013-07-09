#define JSNativeTypeSource

#include "JSNative.h"
#include "JSNativeType.h"

#include <stdint.h>

JSClassRef JSNativeType = NULL;
JSObjectRef RtJSNativeType = NULL;
JSObjectRef RtJSNativeTypeAlias = NULL;

static JSValueRef GetProperty JSTNativePropertyReader() {
	return JSTCoreGetProperty(JSTGetIndexObject(RtJSNativeType, JSTGetPrivateInteger(object)), property);
}

static JSValueRef GetUnsigned JSTNativePropertyReader() {
	return JSTMakeBoolean(JSNativeTypeCodeIsUnsigned(JSTGetPrivateInteger(object)));
}

static JSObjectRef Alias JSTNativeConstructor(type, name) {
	JSObjectRef	type = JSNativeTypeDeref(JSTArgument(0));
	JSValueRef	name = JSTArgument(1);
	JSStringRef propertyName = NULL; JSObjectRef this = NULL;
	if (JSTReference(type)) {
		propertyName = JSTGetValueString(name, NULL);
		if ( ! JSTReference(JSNativeTypeDeref(name)) ) {
			this = JSTCreateClassObject(JSNativeType, JSTPointer(JSTGetProperty(type, "code")));
			JSTSetProperty(this, "alias", name, JSTPropertyConst);
			JSTCoreSetProperty(RtJSNativeTypeAlias, propertyName, (JSValueRef) this,
				JSTPropertyConst
			);
		}
	}
	if (propertyName) JSTFreeString(propertyName);
	if (this) return this;
	return RtJSObject(undefined);
}

static JSValueRef Type JSToolsFunction () {
	register JSValueRef code = RtJS(undefined);
	if (argc) {
		register JSStringRef propertyName = JSTGetValueString(JSTArgument(0), NULL);
		if ( ! JSTReference((code = JSTCoreGetProperty(RtJSNativeType, propertyName)))) {
			if ( ! JSTReference((code = JSTCoreGetProperty(RtJSNativeTypeAlias, propertyName))) ) { 
				// js_throw_exception(ctx, "JSNative.Type: unknown type", exception);
			}
		}
		JSTFreeString(propertyName);
	}
	return code;
}

static JSValueRef ClassTranslate JSTNativeConvertor() {
	if (type == JSTNumberType) return JSTGetProperty(object, "code");
	else if (type == JSTStringType)	return JSTGetProperty(object, "alias");
	return NULL;
}

static JSObjectRef ClassCreateConstantType(JSContextRef ctx, char * name, unsigned long code, bool integral, unsigned long int size, double min, double max) {

	JSValueRef * exception = NULL;

	JSObjectRef this = JSTCreateClassObject(NULL, NULL);

	JSTSetProperty(this, "name", 		JSTMakeBufferValue(name),	JSTPropertyConst);
	JSTSetProperty(this, "code",		JSTMakeNumber(code), 		JSTPropertyConst);
	JSTSetProperty(this, "size",		JSTMakeNumber(size), 		JSTPropertyConst);
	JSTSetProperty(this, "maximum",		JSTMakeNumber(max), 		JSTPropertyConst);
	JSTSetProperty(this, "minimum",		JSTMakeNumber(size), 		JSTPropertyConst);
	JSTSetProperty(this, "integral",	JSTMakeBoolean(integral), 	JSTPropertyConst);

	return this;
}

void js_native_type_init JSToolsProcedure (JSObjectRef jsObjJSNative) {

	JSClassDefinition jsNative = kJSClassDefinitionEmpty;
	jsNative.className = "JSNative.Type";
	jsNative.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsNative.convertToType = &ClassTranslate;

	JSStaticValue StaticValueArray[] = {
		{ "name",		&GetProperty, NULL, JSTPropertyConst },
		{ "code",		&GetProperty, NULL, JSTPropertyConst },
		{ "size",		&GetProperty, NULL, JSTPropertyConst },
		{ "maximum",	&GetProperty, NULL, JSTPropertyConst },
		{ "minimum",	&GetProperty, NULL, JSTPropertyConst },
		{ "integral",	&GetProperty, NULL, JSTPropertyConst },
		{ "unsigned",	&GetUnsigned, NULL, JSTPropertyConst },
		{ 0, 0, 0, 0 }
	};

	jsNative.staticValues = StaticValueArray;
	JSNativeType = JSClassRetain(JSClassCreate(&jsNative));
	RtJSNativeType = JSTSetPropertyFunction(jsObjJSNative, "Type", &Type);
	JSTSetProperty(RtJSNativeType, "Alias", (RtJSNativeTypeAlias = JSTMakeConstructor(JSNativeType, &Alias)), JSTPropertyConst);

	register JSValueRef 
	data = (JSValueRef) ClassCreateConstantType(ctx, "void", 0, false, 0, 0, 0);
	JSTSetProperty(RtJSNativeType, "0", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "bool", 10, false, sizeof(bool), INT8_MIN, INT8_MAX);
	JSTSetProperty(RtJSNativeType, "10", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "char", 20, true, sizeof(char), INT8_MIN, INT8_MAX);
	JSTSetProperty(RtJSNativeType, "20", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "unsigned char", 21, true, sizeof(char), 0, UINT8_MAX);
	JSTSetProperty(RtJSNativeType, "21", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "short", 30, true, sizeof(short), INT16_MIN, INT16_MAX);
	JSTSetProperty(RtJSNativeType, "30", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "unsigned short", 31, true, sizeof(short), 0, UINT16_MAX);
	JSTSetProperty(RtJSNativeType, "31", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "int", 40, true,  sizeof(int), INT32_MIN, INT32_MAX);
	JSTSetProperty(RtJSNativeType, "40", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "unsigned int", 41, true,  sizeof(int), 0, UINT32_MAX);
	JSTSetProperty(RtJSNativeType, "41", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "long", 50, true,  sizeof(long), INT32_MIN, INT32_MAX);
	JSTSetProperty(RtJSNativeType, "50", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "unsigned long", 51, true,  sizeof(long), 0, UINT32_MAX);
	JSTSetProperty(RtJSNativeType, "51", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "long long", 60, true,  sizeof(long long), INT64_MIN, INT64_MAX);
	JSTSetProperty(RtJSNativeType, "60", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "unsigned long long", 61, true,  sizeof(long long), 0, UINT64_MAX);
	JSTSetProperty(RtJSNativeType, "61", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "float", 70, false, sizeof(float), 0, 0);
	JSTSetProperty(RtJSNativeType, "70", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "double", 80, false, sizeof(double), 0, 0);
	JSTSetProperty(RtJSNativeType, "80", data, JSTPropertyConst);

	data = (JSValueRef) ClassCreateConstantType(ctx, "pointer", 90, true, sizeof(intptr_t), INTPTR_MIN, UINTPTR_MAX);
	JSTSetProperty(RtJSNativeType, "90", data, JSTPropertyConst);

}
