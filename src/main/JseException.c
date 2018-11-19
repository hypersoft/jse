#include "jse.h"

GQuark JSErrorQuark = 0;

JSValue JSExceptionFromUtf8(JSContext ctx, char * globalConstructorName, char * fmt, ...)
{
	int size = 0;
	va_list ap;
	va_start(ap, fmt);
	size = vsnprintf(NULL, 0, fmt, ap) + 1;
	va_end(ap);
	char buffer[size];
	va_start(ap, fmt);
	vsnprintf(buffer, size, fmt, ap);
	va_end(ap);
	JSValue message[2] = {JSValueFromUtf8(ctx, buffer), NULL};
	JSObject global = JSContextGetGlobalObject(ctx);
	JSObject ctor = (JSObject) JSObjectGetUtf8Property(ctx, global, globalConstructorName);
	if (! ctor) {
		g_printerr("Cannot find error constructor named `%s'\n", globalConstructorName);
		abort();
	}
	if (JSValueIsObject(ctx, ctor)) return (JSValue)JSObjectCallAsConstructor(ctx, ctor, 1, message, NULL);
	return ctor;
}

JSValue JSExceptionThrowUtf8(JSContext ctx, char * globalConstructorName, JSValue * exception, char * fmt, ...)
{
	if (exception) {
		int size = 0;
		va_list ap;
		va_start(ap, fmt);
		size = vsnprintf(NULL, 0, fmt, ap) + 1;
		va_end(ap);
		char buffer[size];
		va_start(ap, fmt);
		vsnprintf(buffer, size, fmt, ap);
		va_end(ap);
		JSValue message[2] = {JSValueFromUtf8(ctx, buffer), NULL};
		JSObject global = JSContextGetGlobalObject(ctx);
		JSObject ctor = (JSObject) JSObjectGetUtf8Property(ctx, global, globalConstructorName);
		if (JSValueIsObject(ctx, ctor)) *exception = (JSValue)JSObjectCallAsConstructor(ctx, ctor, 1, message, NULL);
		else {
			g_printerr("Cannot find error constructor named `%s'\n", globalConstructorName);
			abort();
		}
	}
	return JSValueMakeNull(ctx);
}

JSValue JSExceptionFromGError(JSContext ctx, GError * error)
{
  JSValue errorValue = JSExceptionFromUtf8(ctx, "Error", "%s", error->message);
  return errorValue;

}

gchar * JSExceptionToUtf8 (JSContext ctx, JSValue e)
{
	gchar *message, *name, *value, *stack;
	JSValue stackObject = JSObjectGetUtf8Property(ctx, (JSObject)e, "stack");
	message = JSValueToUtf8(ctx, JSObjectGetUtf8Property(ctx, (JSObject)e, "message"));
	name = JSValueToUtf8(ctx, JSObjectGetUtf8Property(ctx, (JSObject)e, "name"));
	int line = JSValueToNumber(ctx, JSObjectGetUtf8Property(ctx, (JSObject)e, "line"), NULL);
	char * sourceUrl = JSValueToUtf8(ctx, JSObjectGetUtf8Property(ctx, (JSObject)e, "sourceURL"));
	stack = (JSValueIsUndefined(ctx, stackObject))?NULL:JSValueToUtf8(ctx, stackObject);

	value = (stack)?
		g_strdup_printf ("%s: %s\nstack: %s", name, message, stack)
	:	g_strdup_printf ("%s:%i: %s: %s", sourceUrl, line, name, message);

	g_free (message);
	g_free (sourceUrl);
	g_free (name);
	g_free (stack);

	return value;
}

GError * JSExceptionToGError(JSContext ctx, JSValue exception) {
	if (!ctx || !exception) return NULL;
	char * message =  JSExceptionToUtf8(ctx, exception);
	GError * error = g_error_new (JSErrorQuark, 1024,
		"%s", message
	);
	g_free(message);
	return error;
}

void JSReportException(JSContext ctx, char * command, JSValue exception)
{
	char * message = JSExceptionToUtf8(ctx, exception);
	g_printerr("%s: %s\n", command, message);
	g_free(message);
}

void JSReportError(char * command, GError * error)
{
	g_printerr("%s: %s\n", command, error->message);
	//g_error_free(error);
}