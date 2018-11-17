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

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{

	if (!loadCount) {
		MachineTypeClass = JSClassCreate(&MachineTypeClassDefinition);
		JSClassRetain(MachineTypeClass);
	}

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
}