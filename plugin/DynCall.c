#include "jse.h"

static int loadCount = 0;

#define LIBCLASS "SharedLibrary"
JSClass SharedLibraryClass = NULL;

typedef struct SharedLibraryData {
	void * handle;
	char path[];
} SharedLibraryData;

static void LibraryObjectFinalize(JSObject object)
{
	SharedLibraryData * p = JSObjectGetPrivate(object);
	if (p) {
		dlFreeLibrary(p->handle);
		g_free(p);
		JSObjectSetPrivate(object, NULL);
	}
}

static bool LibraryObjectHasProperty (JSContext ctx, JSObject object, JSString name)
{
	SharedLibraryData * p = JSObjectGetPrivate(object);
	if (!p) return false;

	char buffer[JSStringUtf8Size(name)];
	JSStringGetUTF8CString(name, buffer, sizeof(buffer));
	if (dlFindSymbol(p->handle, buffer)) return true;

	return false;
}

static JSValue LibraryObjectGetProperty(JSContext ctx, JSObject object, JSString name, JSValue * exception)
{
	SharedLibraryData * p = JSObjectGetPrivate(object);
	if (!p) return NULL;

	char buffer[JSStringUtf8Size(name)];
	JSStringGetUTF8CString(name, buffer, sizeof(buffer));
	void * pSymbol = dlFindSymbol(p->handle, buffer);
	if (!pSymbol) return JSValueMakeUndefined(ctx);

	return JSValueFromNumber(ctx, (uintptr_t)pSymbol);

}

static JSValue LibraryObjectConvertToType(JSContext ctx, JSObject object, JSType type, JSValue * exception)
{
	SharedLibraryData * p = JSObjectGetPrivate(object);
	if (type == kJSTypeString) {
		char buffer[1024];
		sprintf(buffer, "[SharedLibrary %s]", p->path);
		return JSValueFromUtf8(ctx, buffer);
	} else if (type == kJSTypeNumber) {
		return JSValueFromNumber(ctx, (uintptr_t)p->handle);
	}
	g_assert_not_reached();
}

static JSClassDefinition LibraryClassDefinition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	LIBCLASS,								/* Class Name */
	NULL,									/* Parent Class */
	NULL,									/* Static Values */
	NULL,									/* Static Functions */
	NULL,									/* Object Initializer */
	(void*) LibraryObjectFinalize,			/* Object Finalizer */
	(void*) LibraryObjectHasProperty,		/* Object Has Property */
	(void*) LibraryObjectGetProperty,		/* Object Get Property */
	NULL,									/* Object Set Property */
	NULL,									/* Object Delete Property */
	NULL,									/* Object Get Property Names */
	NULL,									/* new Object Call As Function */
	NULL,									/* new Object Call As Constructor */
	NULL,									/* Has Instance */
	(void*) LibraryObjectConvertToType		/* Object Convert To Type */
};

static JSValue LibraryObjectConstructor (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{

	if (! JSValueIsObjectOfClass(ctx, (void *) this, SharedLibraryClass)) {
		return JSObjectCallAsConstructor(ctx, function, argc, argv, exception);
	}

	char * libPath = (argc)?JSValueToUtf8(ctx, argv[0]):NULL;

	void * libHandle = dlLoadLibrary(libPath);
	if (! libHandle) {
		return JSExceptionThrowUtf8(ctx, "ReferenceError", exception, "Library: Can't find library `%s'", libPath);
	}

	unsigned bytes = ((libPath)?strlen(libPath):0) + 1;
	SharedLibraryData * p = g_malloc0(sizeof(SharedLibraryData)+bytes);
	p->handle = libHandle;
	if (libPath) memcpy(p->path, libPath, bytes);
	else memcpy(p->path, "jse", 3);
	JSObjectSetPrivate(this, p);

	return this;

}

#define FNCLASS "SharedFunction"
JSClass SharedFunctionClass = NULL;

static void SharedFunctionWriteSignature(JSContext ctx, void * vm, char signature, JSValue argument, bool ellipsisMode, JSValue * exception){
	double number;
	if (signature == 'p') {
		if (JSValueIsString(ctx, argument)) {
			argument = JSInlineEval(ctx, "[this, 0].toBuffer(this.type || UInt8)", (JSObject) argument, NULL);
		}
		number = JSValueToNumber(ctx, argument, exception);
		dcArgPointer(vm, (void*) (uintptr_t) number);
	} else if (signature == 'c' || signature == 's' || signature == 'i') {
		number = JSValueToNumber(ctx, argument, exception);
		dcArgPointer(vm, (void*)(uintptr_t)number);
	} else if (signature == 'C' || signature == 'S' || signature == 'I') {
		number = JSValueToNumber(ctx, argument, exception);
		dcArgInt(vm, number);
	} else if (signature == 'l' || signature == 'L') {
		number = JSValueToNumber(ctx, argument, exception);
		dcArgLongLong(vm, number);
	} else if (signature == 'f') {
		number = JSValueToNumber(ctx, argument, exception);
		if (ellipsisMode) dcArgDouble(vm, number);
		else dcArgFloat(vm, number);
	} else if (signature == 'd') {
		number = JSValueToNumber(ctx, argument, exception);
		dcArgDouble(vm, number);
	}
}

static JSValue SharedFunctionExec (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	char * protocol = JSValueToUtf8(ctx, JSObjectGetUtf8Property(ctx, function, "protocol"));
	int protocolLength = strlen(protocol);
	bool ellipsisMode = protocol[protocolLength - 1] == 'e';

	void * vm = dcNewCallVM((1 + argc) * sizeof(long long));

	if (ellipsisMode) {
		dcMode(vm, DC_CALL_C_ELLIPSIS);
		protocol[--protocolLength] = 0;
	}

	unsigned i; double number;

	for (i = 0; i < protocolLength; i++) {
		SharedFunctionWriteSignature(ctx, vm, protocol[i], argv[i], ellipsisMode, exception);
	}

	if (ellipsisMode && i != argc) {
		for (i; i < argc; i++) {
			JSValue argument = argv[i];
			if (JSValueIsString(ctx, argument)) {
				SharedFunctionWriteSignature(ctx, vm, 'p', argument, ellipsisMode, exception);
			} else if (JSValueIsObject(ctx, argument)){
				if (JSValueToNumber(ctx, JSInlineEval(ctx, "(this.constructor === Address)?1:0", (JSObject) argument, NULL), NULL))
					SharedFunctionWriteSignature(ctx, vm, 'p', argument, ellipsisMode, exception);
				else if (JSValueToNumber(ctx, JSInlineEval(ctx, "(this.constructor === Array)?1:0", (JSObject) argument, NULL), NULL)) {
					g_printerr("dyncall.vm: SharedFunction Fault: unsupported object type: Array");
					g_assert_not_reached();
				} else {
					char signature = JSValueToNumber(ctx, JSInlineEval(ctx, "(this.type)?String(this.type).charCodeAt(0):0", (JSObject) argument, NULL), NULL);
					if (signature) SharedFunctionWriteSignature(ctx, vm, signature, argument, ellipsisMode, exception);
					else {
						number = JSValueToNumber(ctx, argument, exception);
						dcArgInt(vm, (unsigned) number);
					}
				}
			} else if (JSValueIsNull(ctx, argument)) {
				dcArgInt(vm, (unsigned) 0);
			} else if (JSValueIsUndefined(ctx, argument)) {
				dcArgInt(vm, (unsigned) 0);
			} else {
				number = JSValueToNumber(ctx, argument, exception);
				dcArgPointer(vm, (void*)(uintptr_t) number);
			}
		}

	}

	JSValue result = JSValueMakeUndefined(ctx);
	JSValue returnType = JSObjectGetUtf8Property(ctx, function, "returns");
	char * returns = JSValueToUtf8(ctx, returnType);
	char signature = returns[0];
	void * handle = (void*)(uintptr_t)JSValueToNumber(ctx, JSObjectGetUtf8Property(ctx, function, "pointer"), NULL);

/*

’B’	Bool, bool
’c’	char
’C’	unsigned char
’s’	short
’S’	unsigned short
’i’ int
’I’	unsigned int
’j’	long
’J’	unsigned long
’l’	long long, int64 t
’L’	unsigned long long, uint64 t
’f’	float
’d’	double
’p’	void*
’Z’	const char* (pointing to C string)
’v’	void

*/
	if (signature == 'c' || signature == 's' || signature == 'i' || signature == 'j' || signature == 'p') {
		result = JSValueFromNumber(ctx, (uintptr_t)dcCallPointer(vm, handle));
	} else if (signature == 'C' || signature == 'S' || signature == 'I' || signature == 'J') {
		result = JSValueFromNumber(ctx, (signed long) dcCallLong(vm, handle));
	} else if (signature == 'l') {
		result = JSValueFromNumber(ctx, (unsigned long long) dcCallLongLong(vm, handle));
	} else if (signature == 'L') {
		result = JSValueFromNumber(ctx, (signed long long) dcCallLongLong(vm, handle));
	} else if (signature == 'f') {
		result = JSValueFromNumber(ctx, dcCallFloat(vm, handle));
	} else if (signature == 'd') {
		result = JSValueFromNumber(ctx, dcCallDouble(vm, handle));
	} else if (signature == 'v') {
		dcCallVoid(vm, handle);
	} else {
		g_printerr("dyncall.vm: unknown signature: `%c' in protocol `%s'\n", signature, protocol);
		g_assert_not_reached();
	}

	result = JSValueToObject(ctx, result, NULL);
	JSObjectSetUtf8Property(ctx, (JSObject) result, "type", returnType, 0);

	dcFree(vm);
	g_free(protocol); g_free(returns);

	return result;

}

static JSClassDefinition FunctionClassDefinition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	FNCLASS,								/* Class Name */
	NULL,									/* Parent Class */
	NULL,									/* Static Values */
	NULL,									/* Static Functions */
	NULL,									/* Object Initializer */
	NULL,									/* Object Finalizer */
	NULL,									/* Object Has Property */
	NULL,									/* Object Get Property */
	NULL,									/* Object Set Property */
	NULL,									/* Object Delete Property */
	NULL,									/* Object Get Property Names */
	(void*) SharedFunctionExec,				/* new Object Call As Function */
	NULL,									/* new Object Call As Constructor */
	NULL,									/* Has Instance */
	NULL									/* Object Convert To Type */
};

static JSValue FunctionObjectConstructor (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	JSObject addressObject = (JSObject) JSObjectGetUtf8Property(ctx, function, "create");
	return JSObjectCallAsFunction(ctx, addressObject, this, argc, argv, exception);
}

/* it is disappointing that we do not have a callback class 
	 however, there is some logic to this. Anything that requires a callback,
	 should have a definite native translation unit. This helps with debugging,
	 and voids production of sloppy code that may not be accountable for execution
	 in operative, theory.

	 so this is a forced limitation, because its a simple thing to do.
	 if you wan't call back access, you can create your own, but it would,
	 be far easier to write a plugin, that will always work independent of your
	 script.

	 And all the Unix Gurus Say: Seperate your interface from your implementation.
	 
*/

#define CBCLASS "CallBack"
JSClass CallBackClass = NULL;

#define VACLASS "varargs"
JSClass VarArgsClass = NULL;

/* it is dissappointing that we do not have struct and union classes,
   however, the core has data alignment and casting facitilities, built-in.
	 so, one can construct any kind of struct they wish with address binding,
	 and whatever else they can come up with.
*/

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{
	if (!loadCount) {
		SharedLibraryClass = JSClassCreate(&LibraryClassDefinition);
		SharedFunctionClass = JSClassCreate(&FunctionClassDefinition);
		JSClassRetain(SharedLibraryClass);
		JSClassRetain(SharedFunctionClass);
	}

	JSObject SharedLibrary;
	JSObjectSetUtf8Property(ctx, object, LIBCLASS,
		(JSValue) (SharedLibrary = JSConstructorCreate(
			ctx, LIBCLASS, SharedLibraryClass, LibraryObjectConstructor
		)), 0
	);

	JSObject SharedFunction;
	JSObjectSetUtf8Property(ctx, object, FNCLASS,
		(JSValue) (SharedFunction = JSConstructorCreate(
			ctx, FNCLASS, SharedFunctionClass, FunctionObjectConstructor
		)), 0
	);

	loadCount++;
	return (JSValue) object;

}

void unload(JSContext ctx)
{

	if (--loadCount) return;

	JSClassRelease(SharedLibraryClass);
	JSClassRelease(SharedFunctionClass);

}
