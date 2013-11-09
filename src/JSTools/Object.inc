#define JSTObject JSObjectRef

#define JSTObjectGetPrototype(o) (JSTObject)JSTAction(JSObjectGetPrototype, o)
#define JSTObjectGetConstructor(o) (JSTObject)JSTObjectGetProperty(JSTObjectGetPrototype(o), "constructor")
#define JSTObjectSetPrototype(o, p) JSTAction(JSObjectSetPrototype, o, p)
#define JSTObjectHasProperty(o, p) JST(JSTObjectHasProperty_, o, p)
#define JSTObjectGetProperty(o, p) JST(JSTObjectGetProperty_, (JSTObject)o, p)
#define JSTObjectSetProperty(o, p, v, a) JST(JSTObjectSetProperty_, o, p, v, a)
#define JSTObjectDeleteProperty(o, p) JST(JSTObjectDeleteProperty_, o, p)
#define JSTObjectGetPropertyAtIndex(o, i) JST(JSObjectGetPropertyAtIndex,(JSTObject) o, i)
#define JSTObjectSetPropertyAtIndex(o, i, v) JST(JSObjectSetPropertyAtIndex, o, i, v)
#define JSTObjectGetPrivate(o) JSObjectGetPrivate(o)
#define JSTObjectSetPrivate(o, p) JSObjectSetPrivate(o, p)

#define JSTObjectSetMethod(o, n, m, a)	if (JSTValueIsObject(o)) { JSTString name = JSTStringFromUTF8(n); \
JST(JSObjectSetProperty, o, name, JSTAction(JSObjectMakeFunctionWithCallback, name, m), a); \
JSTStringRelease(name); \
}

#define JSTObjectIsFunction(o) JSObjectIsFunction(ctx, o)
#define JSTObjectIsConstructor(o) JSTAction(JSObjectIsConstructor, o)

#define JSTObjectToValue(o) ((JSTValue) o)
#define JSTObjectUndefined ((JSTObject) JSTValueUndefined)
#define JSTObjectNull ((JSTObject) JSTValueNull)

#define JSTObjectPropertyHidden kJSPropertyAttributeDontEnum
#define JSTObjectPropertyReadOnly kJSPropertyAttributeReadOnly
#define JSTObjectPropertyRequired kJSPropertyAttributeDontDelete

#ifdef JSTools_h

extern bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p);
extern void JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a);
extern JSTValue JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p);
extern bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p);

#else

bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JST(JSObjectDeleteProperty, o, s);
		JSTStringRelease(s);
	}
	return result;
}

JSTValue JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p) {
	JSTValue result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JST(JSObjectGetProperty, o, s);
		JSTStringRelease(s);
	}
	return result;
}

void JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a) {
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		JST(JSObjectSetProperty, o, s, v, a);
		JSTStringRelease(s);
	}
}

bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSObjectHasProperty, o, s);
		JSTStringRelease(s);
	}
	return result;
}


#endif

