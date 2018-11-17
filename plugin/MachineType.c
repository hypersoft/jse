#define MACHINE_TYPE_C

#include <MachineType.h>

#define MTCLASS "MachineType"

static int loadCount = 0;

static JSClass MachineTypeClass = NULL;

static JSClassDefinition MachineTypeClassDefinition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	MTCLASS,								/* Class Name */
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
	NULL,									/* new Object Call As Function */
	NULL,									/* new Object Call As Constructor */
	NULL,									/* Has Instance */
	NULL									/* Object Convert To Type */
};

static JSValue JSMachineTypeConstructor (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	JSObject machineTypeConstructor = (JSObject) JSObjectGetUtf8Property(ctx, function, "create");
	JSObjectCallAsFunction(ctx, machineTypeConstructor, this, argc, argv, exception);
	return this;
}

double MachineTypeRead(JSContext ctx, void * address, size_t element, unsigned code, JSValue * exception) {

	unsigned
		width = (code & (1|2|4|8));
	bool 
		sign = (code & 1024),
			floating = (code & 2048),
				pointer = (code & 512),
					utf = (code & 4096); // <-- this won't be used, as utfN is just an alias for get charcode

	if (pointer) {
		return ((uintptr_t *)address)[element];
	} else if (floating) {
		if (width == 4) return ((float*)address)[element];
		else if (width == 8) return ((double*)address)[element];
		else goto bad_float_request;
	}

	if (address && width) {
		if (sign) {
			if (width == 1) return ((signed char *)address)[element];
			else if (width == 2) return ((signed short *)address)[element];
			else if (width == 4) return ((signed long *)address)[element];
			else if (width == 8) return ((signed long long *)address)[element];
		} else {
			if (width == 1) return ((unsigned char *)address)[element];
			else if (width == 2) return ((unsigned short *)address)[element];
			else if (width == 4) return ((unsigned long *)address)[element];
			else if (width == 8) return ((unsigned long long *)address)[element];
		}
	}

bad_type_request:

	THROWING_EXCEPTION(JSExceptionFromUtf8(ctx, JSE_ERROR_CTOR, "failed to read from address %p with a width of %i", address, width));
	return 0;

bad_float_request:

	THROWING_EXCEPTION(JSExceptionFromUtf8(ctx, JSE_ERROR_CTOR, "failed to read floating value from address %p with a width of %i", address, width));
	return 0;

}

static JSValue JSMachineTypeRead(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	// address, element?
	if (argc < 1 || argc > 2) {
		return THROWING_EXCEPTION(WANT_RANGE_PARAMETERS(1, 2));
	}

	void * address = (void*)(uintptr_t)JSValueToNumber(ctx, argv[0], NULL);
	size_t element = (argc > 1)?JSValueToNumber(ctx, argv[1], NULL):0;
	unsigned code = JSValueToNumber(ctx, (JSValue) this, NULL);

	JSValue result = JSValueFromNumber(ctx, MachineTypeRead(ctx, address, element, code, exception));

	if (exception && *exception) return JSValueMakeNull(ctx);

	result = (JSValue) JSValueToObject(ctx, result, NULL);
	JSObjectSetUtf8Property(ctx, (JSObject) result, "type", (JSValue)this, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly);

	return result;

}

bool MachineTypeWrite(JSContext ctx, void * address, size_t element, JSValue data, unsigned code, JSValue * exception) {

	unsigned
		width = (code & (1|2|4|8));
	bool 
		sign = (code & 1024),
			floating = (code & 2048),
				constant = (code & 256),
					pointer = (code & 512),
						utf = (code & 4096);

	if (constant) return true;

	double value = 0;

	if (JSValueIsNumber(ctx, data)) {
		value = JSValueToNumber(ctx, data, exception);
	} else if (JSValueIsString(ctx, data)) {
		JSStringRef jsStr = JSValueToString(ctx, data, exception);
		short * str = (void*) JSStringGetCharactersPtr(jsStr);
		int len = JSStringGetLength(jsStr);
		if (len == 1) {
			value = str[0];
		} else {
			JSExceptionThrowUtf8(ctx, "TypeError", exception, "trying to set address member %i to a multi-character-string", element);
		}
	} else {
		value = JSValueToNumber(ctx, data, exception);
	}

	if (exception && *exception) return true;

	if (pointer) sign = floating = 0, width = sizeof(void*);

	if (address && width && constant == 0) {
		if (floating) {
			if (width == 4) ((float *)address)[element] = value;
			else if (width == 8) ((double *)address)[element] = value;
			else goto oops;
		} else {
			if (width == 1) ((unsigned char *)address)[element] = value;
			else if (width == 2) ((unsigned short *)address)[element] = value;
			else if (width == 4) ((unsigned long *)address)[element] = value;
			else if (width == 8) ((unsigned long long *)address)[element] = value;
			else goto oops;
		}
	}

	return true;

oops:

	THROWING_EXCEPTION(JSExceptionFromUtf8(ctx, JSE_ERROR_CTOR, "failed to write to address %p with a width of %i", address, width));
	return false;

}

JSValue JSMachineTypeWrite(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	// address, value
	// address, element, value
	if (argc < 1 || argc > 3) {
		return THROWING_EXCEPTION(WANT_RANGE_PARAMETERS(1, 3));
	}

	void * address = (void*)(uintptr_t)JSValueToNumber(ctx, argv[0], NULL);
	long long element = 0;
	JSValue data;
	if (argc == 2) data = argv[1];
	else if (argc > 2)
		element = JSValueToNumber(ctx, argv[1], NULL),
			data = argv[2];
	unsigned code = JSValueToNumber(ctx, (JSValue) this, NULL);
	
	if (MachineTypeWrite(ctx, address, element, data, code, exception)) {
		return data;
	}

	return JSValueMakeUndefined(ctx);
	
}

#define CLASSNAME "Address"

JSClass Address = NULL;

#define AddressGetResizable(a) (gboolean)(a->pointer == 0 || a->allocated)
#define AddressFromValue(ctx, value, exception) (void*)(uintptr_t)JSValueToNumber(ctx, (JSValue) value, exception)

typedef struct AddressContainer {
	void * data;
	size_t length; bool allocated, readOnly;
} AddressContainer;

static void AddressObjectInitialize(JSContext ctx, JSObject object)
{
	JSObjectSetPrivate(object, g_malloc0(sizeof(AddressContainer)));
}

static void AddressObjectFinalize(JSObject object)
{
	AddressContainer * address = JSObjectGetPrivate(object);
	if (address) {
		if (address->data) free(address->data);
		free(address);
	}
};

static JSValue AddressObjectGetProperty(JSContext ctx, JSObject object, JSString id, JSValue * exception)
{

	AddressContainer * addressContainer = JSObjectGetPrivate(object);

	char name[JSStringUtf8Size(id)];
	JSStringGetUTF8CString (id, name, sizeof(name));

	{
		int i; for (i = 0; name[i]; i++) {
			if (i == 0 && name[i] == '-') continue;
			else if (!isdigit(name[i])) goto notAnIndex;
		}

		size_t element; sscanf(name, "%lld", &element);
		size_t length = addressContainer->length;

		if (length == 0) return JSValueMakeUndefined(ctx);
		if (element < 0) return JSValueMakeUndefined(ctx);
		if (element >= length) return JSValueMakeUndefined(ctx);

		JSObject objectType = (JSObject) JSObjectGetUtf8Property(ctx, object, "type");
		unsigned code = JSValueToNumber(ctx, (JSValue) objectType, NULL);

		int argc = 2; JSValue argv[] = {object, JSValueFromNumber(ctx, element), NULL};
		return JSMachineTypeRead(ctx, NULL, objectType, argc, argv, exception);

	}	notAnIndex:

	if (!g_strcmp0(name, "pointer")) {
		return JSValueFromNumber(ctx, (uintptr_t) addressContainer->data);
	} else if (!g_strcmp0(name, "allocated")) {
		return JSValueMakeBoolean(ctx, addressContainer->allocated);
	} else if (!g_strcmp0(name, "length")) {
		return JSValueFromNumber(ctx, addressContainer->length);
	} else if (!g_strcmp0(name, "readOnly")) {
		return JSValueMakeBoolean(ctx, addressContainer->readOnly);
	}

	return NULL;

}

static bool AddressObjectSetProperty (JSContext ctx, JSObject object, JSString id, JSValue data, JSValue * exception)
{

	AddressContainer * addressContainer = JSObjectGetPrivate(object);

	if (addressContainer->readOnly) return true;

	char name[JSStringUtf8Size(id)];
	JSStringGetUTF8CString (id, name, sizeof(name));

	{
		int i;
		for (i = 0; name[i]; i++) {
			if (i == 0 && name[i] == '-') continue;
			else if (!isdigit(name[i])) goto notAnIndex;
		}
		
		size_t element; sscanf(name, "%lld", &element);
		size_t length = addressContainer->length;

		// notice the silent fails. we should be throwing, but technically that's not valid javascript,
		// within a property accessor.
		if (length == 0) return true;
		if (element < 0) return true;
		if (element >= length) return true;

		int argc = 3;
		JSValue argv[] = {object, JSValueFromNumber(ctx, element), data, NULL};
		return JSMachineTypeWrite(ctx, NULL, (JSObject)JSObjectGetUtf8Property(ctx, object, "type"), argc, argv, exception);

	}	notAnIndex:

	if (!g_strcmp0(name, "pointer")) {
		void * address = AddressFromValue(ctx, data, NULL);
		if (addressContainer->data && address != addressContainer->data && address != 0) {
			// access violation of the memory integrity.
			if (exception) *exception = JSExceptionFromUtf8 (ctx,
				"ReferenceError",
				"attempting to change the pointer property of an internal address"
			);
			return true;
		}
		if (address == 0 && addressContainer-> data != 0 && addressContainer->allocated) {
			free(addressContainer->data);
			addressContainer->length = 0;
			addressContainer->allocated = false;
		}
		addressContainer->data = address;
		return true;
	} else if (!g_strcmp0(name, "length")) {
		void * address = addressContainer->data;
		int code = JSValueToNumber(ctx, JSObjectGetUtf8Property(ctx, object, "type"), NULL);
		if (code == 0) return true;
		int width = code & (1|2|4|8);
		unsigned current = addressContainer->length;
		unsigned length = JSValueToNumber(ctx, data, exception);
		addressContainer->length = length;
		unsigned bytes = length * width;
		if (bytes) {
			if (address == 0) { addressContainer->allocated = true; }
			if (addressContainer->allocated) address = realloc(address, bytes);
			if (address == 0) { addressContainer->allocated = false; }
		} else if (address) {
			free(address);
			address = 0;
		}
		addressContainer->data = address;
		if (length > current && address) {
			int overage = length - current, bytes = overage * width;
			memset(address + (current * width), 0, bytes);
		}
		return true;
	} else if (!g_strcmp0(name, "allocated")) {
		return true;
	} else if (!g_strcmp0(name, "readOnly")) {
		addressContainer->readOnly = JSValueToBoolean(ctx, data);
		return true;
	}

	return false;

}

static JSClassDefinition AddressDefinition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	CLASSNAME,								/* Class Name */
	NULL,									/* Parent Class */
	NULL,									/* Static Values */
	NULL,									/* Static Functions */
	(void*) AddressObjectInitialize,		/* Object Initializer */
	(void*) AddressObjectFinalize,			/* Object Finalizer */
	NULL,									/* Object Has Property */
	(void*) AddressObjectGetProperty,		/* Object Get Property */
	(void*) AddressObjectSetProperty,		/* Object Set Property */
	NULL,									/* Object Get Property Names */
	NULL,									/* new Object Call As Function */
	NULL,									/* new Object Call As Constructor */
	NULL,									/* Has Instance */
	NULL									/* Object Convert To Type */
};

static JSValue
AddressObjectConstructor(JSContext ctx,
	JSObject function,
	JSObject this,
	size_t argc,
	const JSValue argv[],
	JSValue * exception)
{
	JSObject addressObject = (JSObject) JSObjectGetUtf8Property(ctx, function, "create");
	JSObjectCallAsFunction(ctx, addressObject, this, argc, argv, exception);
	return this;
}

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{

	if (!loadCount) {
		MachineTypeClass = JSClassCreate(&MachineTypeClassDefinition);
		Address = JSClassCreate(&AddressDefinition);
		JSClassRetain(MachineTypeClass);
		JSClassRetain(Address);
	}

	JSObject constructor;
	JSObjectSetUtf8Property(ctx, object, CLASSNAME,
		(JSValue) (constructor = JSConstructorCreate(
			ctx, CLASSNAME, Address, AddressObjectConstructor
		)), 0
	);

	JSObject MachineType;
	JSObjectSetUtf8Property(ctx, object, MTCLASS,
		(JSValue) (MachineType = JSConstructorCreate(
			ctx, MTCLASS, MachineTypeClass, JSMachineTypeConstructor
		)), 0
	);

	JSObjectCreateFunction(ctx, MachineType, "read", JSMachineTypeRead);
	JSObjectCreateFunction(ctx, MachineType, "write", JSMachineTypeWrite);

	// make it easy to define the platform pointer type with this [hidden] static ref.
	JSObjectSetUtf8Property(ctx, MachineType, "ptrSize", JSValueFromNumber(ctx, sizeof(uintptr_t)), 0);
	JSObjectSetUtf8Property(ctx, MachineType, "intSize", JSValueFromNumber(ctx, sizeof(size_t)), 0);

#if (BYTE_ORDER == LITTLE_ENDIAN)
	JSObjectSetUtf8Property(ctx, MachineType, "bigEndian", JSValueMakeBoolean(ctx, false), kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete);
#elif (BYTE_ORDER == BIG_ENDIAN)
	JSObjectSetUtf8Property(ctx, MachineType, "bigEndian", JSValueMakeBoolean(ctx, true), kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete);
#endif

	loadCount++;
	return (JSValue) object;

}

void unload(JSContext ctx)
{
	if (--loadCount) return;
	JSClassRelease(MachineTypeClass);
	JSClassRelease(Address);
}