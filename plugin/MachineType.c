#include <jse.h>
#define MTCLASS "MachineType"
static int loadCount = 0;
JSClass MachineTypeClass = NULL;

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

static JSValue MachineTypeObjectConstructor (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	JSObject machineTypeConstructor = (JSObject) JSObjectGetUtf8Property(ctx, function, "create");
	JSObjectCallAsFunction(ctx, machineTypeConstructor, this, argc, argv, exception);
	return this;
}


JSValue machineTypeRead(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	// address, element?
	if (argc < 1 || argc > 2) {
		return THROWING_EXCEPTION(WANT_RANGE_PARAMETERS(1, 2));
	}

	void * address = (void*)(uintptr_t)JSValueToNumber(ctx, argv[0], NULL);
	long long element = (argc > 1)?JSValueToNumber(ctx, argv[1], NULL):0;

	unsigned code = JSValueToNumber(ctx, (JSValue) this, NULL),
			 width = (code & (1|2|4|8));

	bool sign = (code & 1024),
		floating = (code & 2048),
		pointer = (code & 512);

	if (pointer) sign = floating = 0, width = sizeof(void*);

	double value = 0;

	if (address && width) {
		if (sign) {
			if (width == 1) value = ((signed char *)address)[element];
			else if (width == 2) value = ((signed short *)address)[element];
			else if (width == 4) value = ((signed long *)address)[element];
			else if (width == 8) value = ((signed long long *)address)[element];
			else g_assert_not_reached();
		} else {
			if (floating) {
				if (width == 4) value = ((float *)address)[element];
				else if (width == 8) value = ((double *)address)[element];
				else g_assert_not_reached();
			} else {
				if (width == 1) value = ((unsigned char *)address)[element];
				else if (width == 2) value = ((unsigned short *)address)[element];
				else if (width == 4) value = ((unsigned long *)address)[element];
				else if (width == 8) value = ((unsigned long long *)address)[element];
				else g_assert_not_reached();
			}
		}
		JSValue nargv[] = {JSValueFromNumber(ctx, value), 0};
		return JSObjectCallAsFunction(ctx, this, this, 1, nargv, exception);
	} else {
		return JSValueMakeUndefined(ctx);
	}
}

JSValue machineTypeWrite(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	// address, value
	// address, element, value
	if (argc < 1 || argc > 3) {
		return THROWING_EXCEPTION(WANT_RANGE_PARAMETERS(1, 3));
	}

	void * address = (void*)(uintptr_t)JSValueToNumber(ctx, argv[0], NULL);
	double value = 0; long long element = 0;
	if (argc == 2) value = JSValueToNumber(ctx, argv[1], NULL);
	else if (argc > 2)
		element = JSValueToNumber(ctx, argv[1], NULL),
		value = JSValueToNumber(ctx, argv[2], NULL);
	unsigned code = JSValueToNumber(ctx, (JSValue) this, NULL),
			 width = (code & (1|2|4|8));

	bool sign = (code & 1024),
		floating = (code & 2048),
		constant = (code & 256),
		pointer = (code & 512);

	if (pointer) sign = floating = 0, width = sizeof(void*);

	if (address && width && constant == 0) {
		if (floating) {
			if (width == 4) ((float *)address)[element] = value;
			else if (width == 8) ((double *)address)[element] = value;
			else g_assert_not_reached();
		} else {
			if (width == 1) ((unsigned char *)address)[element] = value;
			else if (width == 2) ((unsigned short *)address)[element] = value;
			else if (width == 4) ((unsigned long *)address)[element] = value;
			else if (width == 8) ((unsigned long long *)address)[element] = value;
			else g_assert_not_reached();
		}
		return argv[1];
	} else {
		return JSValueMakeUndefined(ctx);
	}
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
			ctx, MTCLASS, MachineTypeClass, MachineTypeObjectConstructor
		)), 0
	);

	JSObjectCreateFunction(ctx, MachineType, "read", machineTypeRead);
	JSObjectCreateFunction(ctx, MachineType, "write", machineTypeWrite);

	// make it easy to define the platform pointer type with this [hidden] static ref.
	JSObjectSetUtf8Property(ctx, MachineType, "ptrSize", JSValueFromNumber(ctx, sizeof(uintptr_t)), 0);
	JSObjectSetUtf8Property(ctx, MachineType, "intSize", JSValueFromNumber(ctx, sizeof(size_t)), 0);

#if (BYTE_ORDER == LITTLE_ENDIAN)
	JSObjectSetUtf8Property(ctx, MachineType, "littleEndian", JSValueMakeBoolean(ctx, true), kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete);
#elif (BYTE_ORDER == BIG_ENDIAN)
	JSObjectSetUtf8Property(ctx, MachineType, "littleEndian", JSValueMakeBoolean(ctx, false), kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete);
#endif

	loadCount++;
	return (JSValue) object;

}

void unload(JSContext ctx)
{
	if (--loadCount) return;
	JSClassRelease(MachineTypeClass);
}