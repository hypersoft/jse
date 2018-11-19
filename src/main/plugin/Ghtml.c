#include <jse.h>
#define GhtmlClassName "Ghtml"
static int loadCount = 0;
JSClass GhtmlClass = NULL;

static JSClassDefinition GhtmlClassClassDefinition = {
	0,										/* Version, always 0 */
											/* ClassAttributes */
	kJSClassAttributeNoAutomaticPrototype,
	GhtmlClassName,								/* Class Name */
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

static JSValue GhtmlClassObjectConstructor (JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	JSObject machineTypeConstructor = (JSObject) JSObjectGetUtf8Property(ctx, function, "create");
	JSObjectCallAsFunction(ctx, machineTypeConstructor, this, argc, argv, exception);
	return this;
}

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{

	if (!loadCount) {
		GhtmlClass = JSClassCreate(&GhtmlClassClassDefinition);
		JSClassRetain(GhtmlClass);
	}

	JSObject ghtmlClass;
	JSObjectSetUtf8Property(ctx, object, GhtmlClassName,
		(JSValue) (ghtmlClass = JSConstructorCreate(
			ctx, GhtmlClassName, GhtmlClass, GhtmlClassObjectConstructor
		)), 0
	);

	loadCount++;
	return (JSValue) object;

}

void unload(JSContext ctx)
{
	if (--loadCount) return;
	JSClassRelease(GhtmlClass);
}