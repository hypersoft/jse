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
#include <ctype.h>
#include <inttypes.h>

static int loadCount = 0;

#define CLASSNAME "Address"

JSClass Address = NULL;

#define AddressGetResizable(a) (gboolean)(a->vector == 0 || a->allocated)
#define AddressFromValue(ctx, value, exception) (void*)(uintptr_t)JSValueToNumber(ctx, (JSValue) value, exception)

typedef struct AddressContainer {
	void * data;
	unsigned length; bool allocated, readOnly;
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

		void * address = addressContainer->data;
		long long element; sscanf(name, "%lld", &element);
		long long length = addressContainer->length;

		if (length == 0) return JSValueMakeUndefined(ctx);
		if (element < 0) element = length + element;
		if (element < 0) return JSValueMakeUndefined(ctx);
		if (element >= length) return JSValueMakeUndefined(ctx);

		JSObject objectType = (JSObject) JSObjectGetUtf8Property(ctx, object, "type");
		unsigned code = JSValueToNumber(ctx, (JSValue) objectType, NULL),
				 width = (code & (1|2|4|8));

		bool sign = (code & 1024),
			floating = (code & 2048),
			pointer = (code & 512),
			utf = (code & 4096);

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
			JSObject out = JSValueToObject(ctx, JSValueFromNumber(ctx, value), NULL);
			JSObjectSetUtf8Property(ctx, out, "type", (JSValue)objectType, kJSPropertyAttributeDontEnum | kJSPropertyAttributeDontDelete);
			return (JSValue) out;
//			JSValue nargv[] = {JSValueFromNumber(ctx, value), 0};
//			return JSObjectCallAsFunction(ctx, objectType, objectType, 1, nargv, exception);
		} else {
			return JSValueMakeUndefined(ctx);
		}
	}	notAnIndex:

	if (!g_strcmp0(name, "vector")) {
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
		int i; for (i = 0; name[i]; i++) {
			if (i == 0 && name[i] == '-') continue;
			else if (!isdigit(name[i])) goto notAnIndex;
		}
		void * address = addressContainer->data;
		long long element; sscanf(name, "%lld", &element);
		long long length = addressContainer->length;

		// notice the silent fails. we should be throwing, but technically that's not valid javascript,
		// within a property accessor.
		if (length == 0) return true;
		if (element < 0) element = length + element;
		if (element < 0) return true;
		if (element >= length) return true;

		unsigned code = JSValueToNumber(ctx, JSObjectGetUtf8Property(ctx, object, "type"), NULL),
				 width = (code & (1|2|4|8));

		bool sign = (code & 1024),
			floating = (code & 2048),
			constant = (code & 256),
			pointer = (code & 512),
			utf = (code & 4096);

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
				JSExceptionThrowUtf8(ctx, "TypeError", exception, "trying to set address member %s to a multi-length-string", name);
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
		void * address = AddressFromValue(ctx, data, NULL);
		if (addressContainer->data && address != addressContainer->data && address != 0) {
			// access violation of the memory integrity.
			if (exception) *exception = JSExceptionFromUtf8 (ctx,
				"ReferenceError",
				"attempting to change the vector property of an internal pointer"
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
	if (!Address) {
		Address = JSClassCreate(&AddressDefinition);
	}

	JSClassRetain(Address);
	JSObject constructor;
	JSObjectSetUtf8Property(ctx, object, CLASSNAME,
		(JSValue) (constructor = JSConstructorCreate(
			ctx, CLASSNAME, Address, AddressObjectConstructor
		)), 0
	);
	
	loadCount++;
	return (JSValue) object;

}

void unload(JSContext ctx) {

	if (--loadCount) return;
	JSClassRelease(Address);

}