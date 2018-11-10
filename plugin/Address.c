/*

 This is a template file for a single class plug-in. Plug-ins can contain
 multiple classes.

 The provided loadCount variable is not necessary, but demonstrates reference
 counting by a plug-in.

 The associated function comments are exported from javascriptcoregtk-4.0.

 You should not expect this class to work 'out of the box,' it merely
 demonstrates how your class plug-in should be defined.

 */

#include <jse.h>

static int loadCount = 0;

#define CLASSNAME "Address"

JSClass Address = NULL;

JSString AddressPropertyVector;
JSString AddressPropertyType;
JSString AddressPropertyLength;
JSString AddressPropertyUnits;
JSString AddressPropertyWritable;

#define AddressGetResizable(a) (gboolean)(a->vector == 0 || a->allocated)
#define AddressFromValue(ctx, value, exception) (void*)(unsigned)JSValueToNumber(ctx, (JSValue) value, exception)

static void AddressObjectInitialize(JSContext ctx, JSObject object)
{

}

static void AddressObjectFinalize(JSObject object)
{
	g_free(JSObjectGetPrivate(object));
};

static JSValue AddressObjectGetProperty(JSContext ctx, JSObject object, JSString id, JSValue * exception)
{

	char name[JSStringUtf8Size(id)];
	JSStringGetUTF8CString (id, name, sizeof(name));
	//g_print("get %s\n", name);


	{
		int i; for (i = 0; name[i]; i++) {
			if (i == 0 && name[i] == '-') continue;
			else if (!isdigit(name[i])) goto notAnIndex;
		}

		void * address = (void*)(unsigned)JSValueToNumber(ctx, (JSValue) object, NULL);
		long long element; sscanf(name, "%lld", &element);
		long long length = JSValueToNumber(ctx, JSObjectGetProperty(ctx, object, AddressPropertyLength, NULL), NULL);

		if (length == 0) length = JSValueToNumber(ctx, JSObjectGetProperty(ctx, object, AddressPropertyUnits, NULL), NULL);
		if (element < 0) element = length + element;
		if (element < 0) return JSValueMakeUndefined(ctx);
		if (element >= length) return JSValueMakeUndefined(ctx);

		JSObject objectType = (JSObject) JSObjectGetProperty(ctx, object, AddressPropertyType, NULL);
		unsigned code = JSValueToNumber(ctx, (JSValue) objectType, NULL),
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
			return JSObjectCallAsFunction(ctx, objectType, objectType, 1, nargv, exception);
		} else {
			return JSValueMakeUndefined(ctx);
		}
	}	notAnIndex:

	if (!g_strcmp0(name, "allocated")) {
		void * p = AddressFromValue(ctx, object, NULL);
		return JSValueMakeBoolean(ctx, p && p == JSObjectGetPrivate(object));
	}

	return NULL;

}

static bool AddressObjectSetProperty (JSContext ctx, JSObject object, JSString id, JSValue data, JSValue * exception)
{

	char name[JSStringUtf8Size(id)];
	JSStringGetUTF8CString (id, name, sizeof(name));
	//g_print("set %s\n", name);

	{
		int i; for (i = 0; name[i]; i++) {
			if (i == 0 && name[i] == '-') continue;
			else if (!isdigit(name[i])) goto notAnIndex;
		}
		void * address = (void*)(unsigned)JSValueToNumber(ctx, (JSValue) object, NULL);
		long long element; sscanf(name, "%lld", &element);
		long long length = JSValueToNumber(ctx, JSObjectGetProperty(ctx, object, AddressPropertyLength, NULL), NULL);

		if (length == 0) length = JSValueToNumber(ctx, JSObjectGetProperty(ctx, object, AddressPropertyUnits, NULL), NULL);
		if (element < 0) element = length + element;
		if (element < 0) return true;
		if (element >= length) return true;

		double value = JSValueToNumber(ctx, data, NULL);

		unsigned code = JSValueToNumber(ctx, JSObjectGetProperty(ctx, object, AddressPropertyType, NULL), NULL),
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
		}
		return true;
	}	notAnIndex:

	if (!g_strcmp0(name, "vector")) {
		void * address = AddressFromValue(ctx, object, NULL), * currentAddress = JSObjectGetPrivate(object);
		if (currentAddress && address != currentAddress ) {
			if (exception) *exception = JSExceptionFromUtf8 (
				ctx,
				"ReferenceError",
				"attempting to change the vector property of an internal pointer"
			);
			return true;
		}
	} else if (!g_strcmp0(name, "units")) {
		void * address = AddressFromValue(ctx, object, NULL);
		//if (exception && *exception) return NULL;
		if (address == 0) {
			int code = JSValueToNumber(ctx, JSObjectGetProperty(ctx, object, AddressPropertyType, NULL), NULL);
			if (code == 0) return true;
			int width = code & (1|2|4|8);
			unsigned length = JSValueToNumber(ctx, data, exception);
			if (length == 0) {
				g_free(address); JSObjectSetPrivate(object, NULL);
				JSObjectSetProperty(ctx, object, AddressPropertyVector, JSValueFromNumber(ctx, (unsigned) address), 0, NULL);
				return true;
			}
			unsigned current = JSValueToNumber(ctx, JSObjectGetProperty(ctx, object, AddressPropertyUnits, NULL), NULL);
			address = realloc(address, length * width);
			if (address) {
				JSObjectSetPrivate(object, address);
				JSObjectSetProperty(ctx, object, AddressPropertyVector, JSValueFromNumber(ctx, (unsigned) address), 0, NULL);
				if (length > current) {
					int overage = length - current, bytes = overage * width;
					memset(address + (current * width), 0, bytes);
				}
			} else {
				JSObjectSetPrivate(object, address);
			}
		} else {
			if (exception) *exception = JSExceptionFromUtf8 (
				ctx,
				"TypeError",
				"attempting to set the units property of an external pointer"
			);
			return true;
		}
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

#include <inttypes.h>

static JSValue
AddressConstructorMove(JSContext ctx,
	JSObject function,
	JSObject this,
	size_t argc,
	const JSValue argv[],
	JSValue * exception)
{
	void * dest = (void*)(uintptr_t)JSValueToNumber(ctx, argv[0], exception);
	void * source = (void*)(uintptr_t)JSValueToNumber(ctx, argv[1], exception);
	unsigned bytes = JSValueToNumber(ctx, argv[2], exception);
	void * result = memmove(dest, source, bytes);
	return JSValueFromNumber(ctx, (uintptr_t)result);
}

static JSValue
AddressMalloc(JSContext ctx,
	JSObject function,
	JSObject this,
	size_t argc,
	const JSValue argv[],
	JSValue * exception)
{
	long bytes = JSValueToNumber(ctx, argv[0], NULL);
	return JSValueFromNumber(ctx, (uintptr_t)malloc(bytes));
}

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{
	if (!Address) {
		Address = JSClassCreate(&AddressDefinition);
		AddressPropertyVector = JSStringFromUtf8("vector");
		AddressPropertyType = JSStringFromUtf8("type");
		AddressPropertyLength = JSStringFromUtf8("length");
		AddressPropertyUnits = JSStringFromUtf8("units");
		AddressPropertyWritable = JSStringFromUtf8("writable");
	}

	JSClassRetain(Address);
	JSObject constructor;
	JSObjectSetUtf8Property(ctx, object, CLASSNAME,
		(JSValue) (constructor = JSConstructorCreate(
			ctx, CLASSNAME, Address, AddressObjectConstructor
		)), 0
	);

	JSObjectCreateFunction(ctx, constructor, "move", AddressConstructorMove);
	JSObjectCreateFunction(ctx, constructor, "malloc", AddressMalloc);
	
	loadCount++;
	return (JSValue) object;

}

void unload(JSContext ctx)
{

	if (--loadCount) return;

	JSStringRelease(AddressPropertyVector);
	JSStringRelease(AddressPropertyType);
	JSStringRelease(AddressPropertyLength);
	JSStringRelease(AddressPropertyUnits);
	JSStringRelease(AddressPropertyWritable);

	JSClassRelease(Address);
}