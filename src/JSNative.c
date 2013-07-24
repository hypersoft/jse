/* Brigadier */
#define JSNativeSource

#include "JSNative.inc"
#include "JSNativeClass.inc"

#include "JSNative.h"

JSObjectRef RtJSNativeAPI;
JSObjectRef RtJSNativeClassRegistry;

JSClassRef JSNativeGhost;

static JSStringRef RtCSPrototype, RtCSSetProperty, RtCSGetProperty, RtCSConstruct,
RtCSConvert;

static bool jsNativeApiSetProperty JSTNativePropertyWriter() {

	if (JSTCoreEqualsNative(property, "constructor")) return false;

	JSObjectRef set = (JSObjectRef)
		JSTEval("(this.constructor.__classFlags !== undefined)?this.constructor.setProperty:this.constructor.constructor.setPrototypeProperty", object);
	if (! JSTReference(set) ) return false;

	if (! JSTUndefined(JSTGetProperty(set, "passthrough")) ) return false;
	JSTSetProperty(set, "passthrough", JSTMakeBoolean(true), 0);

	bool result = JSTBoolean(JSTCall(set, object, JSTMakeString(property, NULL, false), value));
	JSTDeleteProperty(set, "passthrough");

	return result;

}

static JSValueRef jsNativeApiGetProperty JSTNativePropertyReader() {

	if (JSTCoreEqualsNative(property, "constructor")) return NULL;
	
	JSObjectRef get = (JSObjectRef)
		JSTEval("(this.constructor.__classFlags !== undefined)?this.constructor.getProperty:this.constructor.constructor.getPrototypeProperty", object);
	if (! JSTReference(get) ) return NULL;

	if (! JSTUndefined(JSTGetProperty(get, "passthrough")) ) return NULL;
	JSTSetProperty(get, "passthrough", JSTMakeBoolean(true), 0);

	JSValueRef result = JSTCall(get, object, JSTMakeString(property, NULL, false));
	JSTDeleteProperty(get, "passthrough"); if (JSTNull(result)) return NULL;

	return result;

}

static JSValueRef jsNativeApiConvertProperty JSTNativeConvertor() {
	JSValueRef conversion;
	JSObjectRef convert = JSTCoreGetPropertyObject(object, RtCSConvert);
	if (convert && JSTReference(convert)) {
		if (kJSTypeString == type) {
			conversion = JSTCall(convert, object, RtJS(String));
		} else
		if (kJSTypeNumber == type) {
			conversion = JSTCall(convert, object, RtJS(Number));
		} else
		if (kJSTypeBoolean == type) {
			conversion = JSTCall(convert, object, RtJS(Boolean));
		} else
		if (kJSTypeObject == type) {
			conversion = JSTCall(convert, object, RtJS(Object));
		} else
		if (kJSTypeNull == type) {
			conversion = JSTCall(convert, object, JSTMakeNull());
		} else
		if (kJSTypeUndefined == type) {
			conversion = JSTCall(convert, object, RtJS(undefined));
		}
		if (JSTBoolean(JSTEval("this === JSNative.api.conversionFailure", conversion))) return false;
		return conversion;
	}
	return false;
}

JSValueRef jsNativeApiCallAsFunction(JSContextRef ctx, JSObjectRef function, JSObjectRef this, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
	JSObjectRef classFunction = JSTGetPropertyObject(function, "function");
	if (! JSTReference(classFunction) ) classFunction = JSTGetPropertyObject(function, "constructor");
	return JSTRelayFunctionCall(classFunction);
}

JSObjectRef jsNativeApiCallAsConstructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	JSClassRef jsClassRef = JSTGetPrivate(constructor);
	JSObjectRef this = JSTCreateClassObject(jsClassRef, jsClassRef);
	JSObjectRef jsConstructor = JSTGetPropertyObject(constructor, "constructor");
	JSTSetPrototype(this, JSTGetProperty(jsConstructor, "prototype"));
	JSTSetProperty(this, "constructor", constructor, JSTPropertyProtected);
	JSValueRef deviation = JSObjectCallAsFunction(ctx, jsConstructor, this, argumentCount, arguments, exception);
	if (!JSTUndefined(deviation)) return (JSObjectRef)deviation;
	return this;
}

static JSValueRef jsNativeApiCreateClass JSToolsFunction () {

	JSClassDefinition jsClass = kJSClassDefinitionEmpty;
	JSClassDefinition jsPrototype = kJSClassDefinitionEmpty;

	JSValueRef constructor = argv[0];
	JSValueRef className = argv[1];

	int flags = JSTInteger(argv[2]);
	int oflags = JSTInteger(argv[3]);

	char * nativeClassName = JSTGetValueBuffer(className, NULL);

	int fConstructor = 2, fFunction = 4, fGet = 8, fSet = 16, fConvert = 32, fManual = 64;

	if (flags & fGet) jsClass.getProperty = &jsNativeApiGetProperty;
	if (flags & fSet) jsClass.setProperty = &jsNativeApiSetProperty;
	if (flags & fConvert) jsClass.convertToType = &jsNativeApiConvertProperty;
	if (flags & fFunction) jsClass.callAsFunction = &jsNativeApiCallAsFunction;
	if (flags & fConstructor) jsClass.callAsConstructor = &jsNativeApiCallAsConstructor;

	if (flags & fManual) jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;

	jsClass.className = nativeClassName;
	JSClassRef jsClassRef = JSClassRetain(JSClassCreate(&jsClass));

	if (oflags & fGet) jsPrototype.getProperty = &jsNativeApiGetProperty;
	if (oflags & fSet) jsPrototype.setProperty = &jsNativeApiSetProperty;
	if (oflags & fConvert) jsPrototype.convertToType = &jsNativeApiConvertProperty;
	if (oflags & fFunction) jsPrototype.callAsFunction = &jsNativeApiCallAsFunction;
	if (oflags & fConstructor) jsPrototype.callAsConstructor = &jsNativeApiCallAsConstructor;

	if (oflags & fManual) jsPrototype.attributes = kJSClassAttributeNoAutomaticPrototype;

	jsPrototype.className = nativeClassName;
	JSClassRef jsPrototypeRef = JSClassRetain(JSClassCreate(&jsPrototype));

	JSTFreeBuffer(nativeClassName);

	JSValueRef thisPrototype = (JSValueRef) JSTCreateClassObject(jsPrototypeRef, jsClassRef);
	JSTSetProperty((JSObjectRef)thisPrototype,"name", className, JSTPropertyProtected);
	JSTSetProperty((JSObjectRef)thisPrototype,"__classFlags", argv[3], JSTPropertyProtected);

	JSValueRef thisClass = (JSValueRef) JSTCreateClassObject(jsClassRef, jsPrototypeRef);
	JSTSetProperty((JSObjectRef)thisClass,"name", className, JSTPropertyProtected);
	JSTSetProperty((JSObjectRef)thisClass,"__prototypeFlags", argv[2], JSTPropertyProtected);

	JSTSetProperty((JSObjectRef)constructor, "__classConstructor", thisClass, JSTPropertyProtected);
	JSTSetProperty((JSObjectRef)constructor, "__classPrototype", thisPrototype, JSTPropertyProtected);
	JSTSetProperty((JSObjectRef)thisClass,"constructor", constructor, JSTPropertyProtected);
	JSTSetProperty((JSObjectRef)thisPrototype,"constructor", constructor, JSTPropertyProtected);

	return thisClass;

}

void js_native_init JSToolsProcedure (int argc, char *argv[], char *envp[]) {

	JSObjectRef global = JSTGetGlobalObject();

	JSTLoadRuntime(global, argc, argv, envp);

	RtCSPrototype = JSTCreateStaticString("prototype", NULL);
	RtCSSetProperty = JSTCreateStaticString("setProperty", NULL);
	RtCSGetProperty = JSTCreateStaticString("getProperty", NULL);
	RtCSConstruct = JSTCreateStaticString("constructor", NULL);
	RtCSConvert = JSTCreateStaticString("convert", NULL);

	JSClassDefinition jsClass = kJSClassDefinitionEmpty;
	jsClass.attributes = kJSClassAttributeNoAutomaticPrototype;
	jsClass.className = "JSNative.Ghost";
	JSNativeGhost = JSClassRetain(JSClassCreate(&jsClass));
	
	RtJSNativeAPI = (JSObjectRef) JSTCreateClassObject(NULL,NULL);//JSTEval("Object.defineProperty(this, 'api', {value:{},writeable:true,configurable:true});",global);

	JSTSetPropertyFunction(RtJSNativeAPI, "createClass", &jsNativeApiCreateClass);

	JSTSetProperty(global, "api", RtJSNativeAPI, 0);

	JSTEvalScript(JSNativeClass, global, "JSNativeClass.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);


	JSTEvalScript(JSNativeSupport, global, "JSNative.js"); 
	if (JSTCaughtException) JSTReportFatalException(1);

	
}

