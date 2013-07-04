#include "JSTools.h"
#include "JSNative.h"

JSClassRef JSNativeType = NULL;

JSObjectRef RtJSNativeType = NULL;
JSObjectRef RtJSNativeTypeAlias = NULL;

static JSStringRef * resource = NULL;

typedef unsigned int JSNativeTypeCode;

#define JSNativeTypeVoid 0
#define JSNativeTypeBool 10	
#define JSNativeTypeChar 20	
#define JSNativeTypeShort 30
#define JSNativeTypeInt 40
#define JSNativeTypeLong 50
#define JSNativeTypeLongLong 60
#define JSNativeTypeFloat 70
#define JSNativeTypeDouble 80
#define JSNativeTypePointer 90

#define JSNativeTypeCodeIsInteger(V) (V < 70 && V > 10)
#define JSNativeTypeCodeIsUnsigned(V) (JSNativeTypeCodeIsInteger(V) && (V % 10))

#define NativeTypeLookup(VAL)  ( (JSObjectRef) JSTCallFunction(RtJSNative, RtJSNativeType, VAL, NULL))

static JSValueRef GetProperty JSTNativePropertyReader() {
	return JSTGetCoreProperty(
		(JSObjectRef) JSTGetPropertyIndex(RtJSNativeType,
			JSTGetPrivateInteger(object)
		), propertyName
	);
}

static JSValueRef GetUnsigned JSTNativePropertyReader() {
	return JSTMakeBoolean(JSNativeTypeCodeIsUnsigned(JSTGetPrivateInteger(object)));
}

static JSObjectRef Alias JSTNativeConstructor(type, name) {
	JSObjectRef	type = NativeTypeLookup(JSTArgument(0));
	JSValueRef	name = JSTArgument(1);
	JSStringRef propertyName = NULL; JSObjectRef this = NULL;
	if (JSTReference(type)) {
		propertyName = JSTCoreString(name);
		if ( ! JSTReference(NativeTypeLookup(name)) ) {
			this = JSTCreateObject(JSNativeType, JSTPointer(JSTGetProperty(type, "code")));
			JSTSetProperty(this, "alias", name, JSTPropertyConst);
			JSTSetCoreProperty(RtJSNativeTypeAlias, propertyName, (JSValueRef) this,
				JSTPropertyConst
			);
		}
	}
	if (propertyName) JSTCoreStringDead(propertyName);
	if (this) return this;
	return JSTKnownObject(JSTMakeUndefined());
}

static JSValueRef Type JSTNativeFunction(code/type object/name) {
	register JSValueRef this = JSTMakeUndefined();
	if (argumentCount) {
		register JSStringRef propertyName = JSTCoreString(JSTArgument(0));
		if ( ! JSTReference((this = JSTGetCoreProperty(RtJSNativeType, propertyName)))) {
			if ( ! JSTReference((this = JSTGetCoreProperty(RtJSNativeTypeAlias, propertyName))) ) { 
				// js_throw_exception(ctx, "JSNative.Type: unknown type", exception);
			}
		}
		JSTCoreStringDead(propertyName);
	}
	return this;
}

static JSValueRef ClassTranslate JSTNativeClassConvertor() {
	if (type == JSTNumberType) return JSTGetProperty(object, "code");
	else if (type == JSTStringType)	return JSTGetProperty(object, "alias");
	return NULL;
}

static JSObjectRef ClassCreateConstantType(JSContextRef ctx, char * name, unsigned long code, bool integral, unsigned long int size, double min, double max) {

	JSValueRef * exception = NULL;

	JSObjectRef this = JSTCreateObject(NULL, NULL);
	JSStringRef coreName = JSTMakeNativeString(name);
	JSTSetProperty(this, "name", JSTMakeCoreString(coreName), JSTPropertyConst);
	JSTCoreStringDead(coreName);

	JSTSetProperty(this, "code",		JSTMakeNumber(code), 		JSTPropertyConst);
	JSTSetProperty(this, "size",		JSTMakeNumber(size), 		JSTPropertyConst);
	JSTSetProperty(this, "maximum",		JSTMakeNumber(max), 		JSTPropertyConst);
	JSTSetProperty(this, "minimum",		JSTMakeNumber(size), 		JSTPropertyConst);
	JSTSetProperty(this, "integral",	JSTMakeBoolean(integral), 	JSTPropertyConst);

	return this;
}

void js_native_type_class_init JSTProcedure (JSObjectRef object) {

	/* The ever ubiquitous string table, provides optimized property operations */

	resource = JSTCoreStringTable(NULL,
		/*  0 */ "name",
		/*  1 */ "code",
		/*  2 */ "size",
		/*  3 */ "maximum",
		/*  4 */ "minimum",
		/*  5 */ "integral",
		/*  6 */ "unsigned",
		/*  7 */ "Type",
		/*  8 */ "Alias",
		/*  9 */ "JSNative",
		/* 10 */ "JSNative.Type",
		/* 11 */ "this.Type",
		NULL
	);

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
/*
	JSStaticFunction StaticFunctionArray[] = {
		{ "", &js_native_address_is_access_aligned, kJSPropertyAttributeDontDelete },
		{ "seekAlignForward", &js_native_address_seek_align_forward, kJSPropertyAttributeDontDelete },
		{ "seekAlignReverse", &js_native_address_seek_align_reverse, kJSPropertyAttributeDontDelete },
		{ NULL, 0, 0 }
	};
*/
	JSClassDefinition jsNative = kJSClassDefinitionEmpty;
	jsNative.className = "JSNative.Type";
	jsNative.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsNative.convertToType = &ClassTranslate;
	jsNative.staticValues = StaticValueArray;
/*
	jsNative.getProperty = &js_native_address_get_index;
	jsNative.staticFunctions = StaticFunctionArray; 
*/
	JSNativeType = JSClassRetain(JSClassCreate(&jsNative));

	RtJSNativeType = JSTMakeCorePropertyFunction(object, resource[7], &Type);
	RtJSNativeTypeAlias = JSTMakeCoreConstructorProperty(RtJSNativeType, resource[8], JSNativeType, &Alias);

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

