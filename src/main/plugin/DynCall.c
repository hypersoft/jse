#include "jse.h"

static int loadCount = 0;

#define SHARED_LIBRARY_CLASS_NAME "SharedLibrary"
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
	SHARED_LIBRARY_CLASS_NAME,								/* Class Name */
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

#define SHARED_FUNCTION_CLASS_NAME "SharedFunction"
JSClass SharedFunctionClass = NULL;

static void SharedFunctionWriteSignature(register JSContext ctx, void * vm, register char signature, JSValue argument, bool ellipsisMode, JSValue * exception){
	register double number;
	if (signature == 'p') {
		if (JSValueIsString(ctx, argument)) {
			argument = JSInlineEval(ctx, "[this, 0].toBuffer(this.type || UInt8)", (JSObject) argument, NULL);
		}
		number = JSValueToNumber(ctx, argument, exception);
		dcArgPointer(vm, (void*) (uintptr_t) number);
	} else if (signature == 'c' || signature == 's' || signature == 'i' || signature == 'j') {
		number = JSValueToNumber(ctx, argument, exception);
		dcArgPointer(vm, (void*)(uintptr_t)number);
	} else if (signature == 'C' || signature == 'S' || signature == 'I' || signature == 'J') {
		number = JSValueToNumber(ctx, argument, exception);
		dcArgPointer(vm, (void*)(uintptr_t)number);
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

static JSValue SharedFunctionExec (register JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	char * protocol = JSValueToUtf8(ctx, JSObjectGetUtf8Property(ctx, function, "protocol"));
	int protocolLength = strlen(protocol);
	bool ellipsisMode = protocol[protocolLength - 1] == 'e';

	void * vm = dcNewCallVM((1 + argc) * sizeof(long long));

	if (ellipsisMode) {
		dcMode(vm, DC_CALL_C_ELLIPSIS_VARARGS);
		protocol[--protocolLength] = 0;
	} else {
    if (JSObjectHasUtf8Property(ctx, this, "mode")) {
      JSValue mode = JSObjectGetUtf8Property(ctx, this, "mode");
      dcMode(vm, JSValueToNumber(ctx, mode, NULL));
    }
  }

	register unsigned i; register double number;

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
					char signature = JSValueToNumber(ctx, JSInlineEval(ctx, "(this.type)?type.format:0", (JSObject) argument, NULL), NULL);
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
  
  g_free(protocol);

	JSValue result = JSValueMakeUndefined(ctx);
	JSValue returnType = JSObjectGetUtf8Property(ctx, function, "returns");
	char signature = JSValueToNumber(ctx, JSObjectGetUtf8Property(ctx, (JSObjectRef) returnType, "format"), NULL);
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
	g_free(protocol);

	return result;

}

static JSClassDefinition SharedFunctionClassDefinition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	SHARED_FUNCTION_CLASS_NAME,								/* Class Name */
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

static JSValue SharedFunctionConstructor (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	JSObject addressObject = (JSObject) JSObjectGetUtf8Property(ctx, function, "create");
	return JSObjectCallAsFunction(ctx, addressObject, this, argc, argv, exception);
}

#define FUNCTION_CALLBACK_CLASS_NAME "FunctionCallback"
JSClass FunctionCallbackClass;

typedef struct _FunctionCallbackData {
	DCCallback * pointer; 
  JSContextGroupRef group;
  JSObject object;
  int protocolLength;
  char * protocol;
  JSObject function;
} FunctionCallbackData;

static void FunctionCallbackInitialize(JSContext ctx, JSObject object)
{
  JSObjectSetPrivate(object, g_malloc0(sizeof(FunctionCallbackData)));
}

static void FunctionCallbackFinalize(JSObject object)
{	
  FunctionCallbackData * data = JSObjectGetPrivate(object);
  if (data->pointer) dcbFreeCallback(data->pointer);
  if (data->protocol) g_free(data->protocol);
  g_free(data);
}

static JSValue FunctionCallbackConvertToType(JSContext ctx, JSObject object, JSType type, JSValue * exception)
{
  if (type == kJSTypeNumber) {
    FunctionCallbackData * data = JSObjectGetPrivate(object);
    return JSValueFromNumber(ctx, (uintptr_t)data->pointer);
  }
	return NULL;
}

static JSClassDefinition FunctionCallbackClassDefinition = {
	0,                                      /* Version, always 0 */
	kJSClassAttributeNoAutomaticPrototype,  /* ClassAttributes */
	FUNCTION_CALLBACK_CLASS_NAME,						/* Class Name */
	NULL,                                   /* Parent Class */
	NULL,                                   /* Static Values */
	NULL,                                   /* Static Functions */
	(void*) FunctionCallbackInitialize,			/* Object Initializer */
	(void*) FunctionCallbackFinalize,				/* Object Finalizer */
	(void*) NULL,                           /* Object Has Property */
	(void*) NULL,                           /* Object Get Property */
	(void*) NULL,                           /* Object Set Property */
	(void*) NULL,                           /* Object Delete Property */
	(void*) NULL,                           /* Object Get Property Names */
	(void*) NULL,                           /* new Object Call As Function */
	(void*) NULL,                           /* new Object Call As Constructor */
	(void*) NULL,                           /* Has Instance */
	(void*) FunctionCallbackConvertToType		/* Object Convert To Type */
};

static char FunctionCallbackHandler(DCCallback* cb, DCArgs* args, DCValue* result, FunctionCallbackData * callbackData)
{
	JSContext ctx = JSGlobalContextCreateInGroup(JSContextGetUniverse(), NULL);
	JSObject e = NULL; JSValue *exception = (void*)&e;
  int max = callbackData->protocolLength - 2;
 
  char returnType = callbackData->protocol[max + 1];
  
  // not currently supported for callback...
  bool ellipsisMode = (callbackData->protocol[max - 1] == 'e');

  if (ellipsisMode) max -= 1;
  
  JSValue argv[max];
  
  for (int i = 0; i < max; i++) {
    switch (callbackData->protocol[i]) {
      case 'B': { argv[i] = JSValueMakeBoolean(ctx, dcbArgBool(args)); break; }
      case 's': case 'i': case 'j': case 'c': { argv[i] = JSValueFromNumber(ctx, dcbArgLong(args)); break; }
      case 'C': case 'S': case 'I': case 'J': { argv[i] = JSValueFromNumber(ctx, dcbArgULong(args)); break; }
      case 'Z': case 'p': { argv[i] = JSValueFromNumber(ctx, (uintptr_t)dcbArgPointer(args)); break; }
      case 'l': { argv[i] = JSValueFromNumber(ctx, dcbArgLongLong(args)); break; }
      case 'L': { argv[i] = JSValueFromNumber(ctx, dcbArgULongLong(args)); break; }
      case 'd': { argv[i] = JSValueFromNumber(ctx, dcbArgDouble(args)); break; }
      case 'f': { argv[i] = JSValueFromNumber(ctx, dcbArgFloat(args)); break; }
      case 'v': { argv[i] = JSValueMakeUndefined(ctx); break; }
    }
  }

	JSValue value = JSObjectCallAsFunction(ctx, (JSObjectRef) callbackData->function, (JSObjectRef) callbackData->object, max, argv, exception);

  if (e) {
    JSReportException(ctx, jse.command, e);
    return 'v';
  }
  
  switch (returnType) {
    case 'B': result->B = JSValueToBoolean(ctx, value); break;
    case 'c': case 's': case 'i': case 'j': case 'l': result->l = JSValueToNumber(ctx, value, NULL); break;
    case 'C': case 'S': case 'I': case 'J': case 'L': result->L = JSValueToNumber(ctx, value, NULL); break;
    case 'p': case 'Z': result->p = (void *)(uintptr_t)JSValueToNumber(ctx, value, NULL); break;
    case 'f': result->f = JSValueToNumber(ctx, value, NULL); break;
    case 'd': result->d = JSValueToNumber(ctx, value, NULL); break;
  }

  return returnType;

}

static JSValue FunctionCallbackConstructor (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
  
  FunctionCallbackData * data = JSObjectGetPrivate(this);
	JSObject constructor = (JSObject) JSObjectGetUtf8Property(ctx, function, "create");

  JSObject cbo = (JSObject) JSObjectCallAsFunction(ctx, constructor, this, argc, argv, exception);
  if (exception && *exception) return NULL;

  data->object = (JSObject) argv[0]; // use fastest get
  data->function = (JSObject) argv[2]; // use fastest get
  data->protocol = JSValueToUtf8(ctx, JSObjectGetUtf8Property(ctx, cbo, "protocol"));

  data->protocolLength = (data->protocol)?strlen(data->protocol):0;

  data->pointer = dcbNewCallback(data->protocol, (void*) &FunctionCallbackHandler, data);
  
	return cbo;

}

/* it is disappointing that we do not have struct and union classes,
   however, the core has data alignment and casting facilities, built-in.
	 so, one can construct any kind of struct they wish with address binding,
	 and whatever else they can come up with.
*/

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{
	if (!loadCount) {
		SharedLibraryClass = JSClassCreate(&LibraryClassDefinition);
		JSClassRetain(SharedLibraryClass);
		SharedFunctionClass = JSClassCreate(&SharedFunctionClassDefinition);
		JSClassRetain(SharedFunctionClass);
    FunctionCallbackClass = JSClassCreate(&FunctionCallbackClassDefinition);
		JSClassRetain(FunctionCallbackClass);
	}

	JSObject SharedLibrary;
	JSObjectSetUtf8Property(ctx, object, SHARED_LIBRARY_CLASS_NAME,
		(JSValue) (SharedLibrary = JSConstructorCreate(
			ctx, SHARED_LIBRARY_CLASS_NAME, SharedLibraryClass, LibraryObjectConstructor
		)), 0
	);

	JSObject SharedFunction;
	JSObjectSetUtf8Property(ctx, object, SHARED_FUNCTION_CLASS_NAME,
		(JSValue) (SharedFunction = JSConstructorCreate(
			ctx, SHARED_FUNCTION_CLASS_NAME, SharedFunctionClass, SharedFunctionConstructor
		)), 0
	);

  int DATA_FLAGS = kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete;
  
  JSObjectSetUtf8Property(ctx, SharedFunction, "TYPE_DEFAULT", JSValueFromNumber(ctx, DC_CALL_C_DEFAULT), DATA_FLAGS);
  JSObjectSetUtf8Property(ctx, SharedFunction, "TYPE_VARARGS", JSValueFromNumber(ctx, DC_CALL_C_VARARGS), DATA_FLAGS);
  JSObjectSetUtf8Property(ctx, SharedFunction, "TYPE_SYSTEM", JSValueFromNumber(ctx, DC_CALL_SYS_DEFAULT), DATA_FLAGS);

	JSObject FunctionCallback;
	JSObjectSetUtf8Property(ctx, object, FUNCTION_CALLBACK_CLASS_NAME,
		(JSValue) (FunctionCallback = JSConstructorCreate(
			ctx, FUNCTION_CALLBACK_CLASS_NAME, FunctionCallbackClass, FunctionCallbackConstructor
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
	JSClassRelease(FunctionCallbackClass);

}
