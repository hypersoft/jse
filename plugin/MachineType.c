#include <jse.h>
#define MTCLASS "MachineType"
static int loadCount = 0;
JSClass MachineTypeClass = NULL;

static JSValue MachineTypeExec (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	JSObject valueObject = (JSObject) JSObjectGetUtf8Property(ctx, function, "value");
	return JSObjectCallAsFunction(ctx, valueObject, function, argc, argv, exception);
}

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
	(void*) MachineTypeExec,				/* new Object Call As Function */
	NULL,									/* new Object Call As Constructor */
	NULL,									/* Has Instance */
	NULL									/* Object Convert To Type */
};

static JSValue MachineTypeObjectConstructor (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	JSObject addressObject = (JSObject) JSObjectGetUtf8Property(ctx, function, "create");
	return JSObjectCallAsFunction(ctx, addressObject, this, argc, argv, exception);
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

	loadCount++;
	return (JSValue) object;

}

void unload(JSContext ctx)
{
	if (--loadCount) return;
	JSClassRelease(MachineTypeClass);
}