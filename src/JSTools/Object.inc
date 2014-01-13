#define JSTObject JSObjectRef

#define JSTObjectGetPrototype(o) (JSTObject)JSTAction(JSObjectGetPrototype, (JSTObject) o)
#define JSTObjectGetConstructor(o) (JSTObject)JSTObjectGetProperty(JSTObjectGetPrototype(o), "constructor")
#define JSTObjectSetPrototype(o, p) JSTAction(JSObjectSetPrototype, (JSTObject) o, (JSTObject) p)
#define JSTObjectHasProperty(o, p) JST(JSTObjectHasProperty_, o, p)
#define JSTObjectGetProperty(o, p) JST(JSTObjectGetProperty_, (JSTObject) o, p)
#define JSTObjectGetObject(o, p) ((JSTObject) JSTObjectGetProperty((JSTObject)o, p))
#define JSTObjectSetProperty(o, p, v, a) JST(JSTObjectSetProperty_, o, p, v, a)
#define JSTObjectDeleteProperty(o, p) JST(JSTObjectDeleteProperty_, o, p)
#define JSTObjectGetPropertyAtIndex(o, i) JST(JSObjectGetPropertyAtIndex,(JSTObject) o, i)
#define JSTObjectSetPropertyAtIndex(o, i, v) JST(JSObjectSetPropertyAtIndex, o, i, v)
#define JSTObjectGetPrivate(o) JSObjectGetPrivate(o)
#define JSTObjectSetPrivate(o, p) JSObjectSetPrivate(o, p)

#define JSTObjectSetMethod(o, n, m, a) JST(JSTObjectSetMethod_, o, n, m, a)

#define JSTObjectSetConstructor(o, n, c, m, a) JST(JSTObjectSetConstructor_, o, n, c, m, a)

#define JSTObjectIsFunction(o) JSObjectIsFunction(ctx, o)
#define JSTObjectIsConstructor(o) JSObjectIsConstructor(ctx, o)

#define JSTObjectToValue(o) ((JSTValue) o)
#define JSTObjectUndefined ((JSTObject) JSTValueUndefined)
#define JSTObjectNull ((JSTObject) JSTValueNull)

#define JSTObjectArray(argc, argv) JST(JSObjectMakeArray, argc, argv)

#define JSTObjectPropertyHidden kJSPropertyAttributeDontEnum
#define JSTObjectPropertyReadOnly kJSPropertyAttributeReadOnly
#define JSTObjectPropertyRequired kJSPropertyAttributeDontDelete
#define JSTObjectPropertyAPI (JSTObjectPropertyHidden | JSTObjectPropertyReadOnly | JSTObjectPropertyRequired)

#ifdef JSTools_h

extern JSTObject JSTObjectSetMethod_ JSTUtility(JSTObject o, JSTString n, void * m, int a);
extern JSTObject JSTObjectSetConstructor_ JSTUtility(JSTObject o, JSTClass c, void * m, int a);
extern bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p);
extern void JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a);
extern void * JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p);
extern bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p);

#else

#define JSTValueIsObject(v) JSTAction(JSValueIsObject, v)

JSTObject JSTObjectSetMethod_ JSTUtility(JSTObject o, char * n, void * m, int a) {
	JSTObject method = NULL;
	if (JSTValueIsObject(o)) {
		JSTString name = JSTStringFromUTF8(n); 
		JST(JSObjectSetProperty, (o)?o:JSContextGetGlobalObject(ctx), name, (method = JSTAction(JSObjectMakeFunctionWithCallback, name, m)), a); 
		JSTStringRelease(name); 
	}
	return method;
}

JSTObject JSTObjectSetConstructor_ JSTUtility(JSTObject o, char * n, JSTClass c, void * m, int a) {
	JSTObject constructor = NULL;
	if (JSTValueIsObject(o)) {
		JSTString name = JSTStringFromUTF8(n);
		JST(JSObjectSetProperty, (o)?o:JSContextGetGlobalObject(ctx), name, (constructor = JSTAction(JSObjectMakeConstructor, c, m)), a);
		JSTStringRelease(name);
	}
	return constructor;
}

bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JST(JSObjectDeleteProperty, (o)?o:JSContextGetGlobalObject(ctx), s);
		JSTStringRelease(s);
	}
	return result;
}

void * JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p) {
	void * result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = (void*) JST(JSObjectGetProperty, (o)?o:JSContextGetGlobalObject(ctx), s);
		JSTStringRelease(s);
	}
	return result;
}

void JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a) {
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		JST(JSObjectSetProperty, (o)?o:JSContextGetGlobalObject(ctx), s, v, a);
		JSTStringRelease(s);
	}
}

bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSObjectHasProperty, (o)?o:JSContextGetGlobalObject(ctx), s);
		JSTStringRelease(s);
	}
	return result;
}

#endif

