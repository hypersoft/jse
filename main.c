#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <getopt.h>
#include <string.h>

#include "glib.inc"
#include "JavaScriptCore.inc"
#include "dyncall.inc"

#include "src/JSNative.c"

JSContextGroupRef MainContextGroup;
JSGlobalContextRef	MainContext;

JSClassRef JSNativeString;


JSGlobalContextRef jse_create_context(JSClassRef globalObjectClass) {
	return JSGlobalContextRetain(JSGlobalContextCreateInGroup(MainContextGroup, globalObjectClass));
}

void jse_destroy_context(JSGlobalContextRef context) {
	JSGlobalContextRelease(context);
}

JSObjectRef jse_get_context_global_object(JSGlobalContextRef context) {
	return JSContextGetGlobalObject(context);
}

typedef struct {
	JSGlobalContextRef context;
	JSObjectRef this;
	JSStringRef content;
	JSValueRef exception;
	JSStringRef url;
} JSContextScript;

JSContextScript * jse_create_script (JSGlobalContextRef context, const char * js, const char * url, JSObjectRef this) {

	register char * seek = (char *) js;

	if ( ! seek ) seek = "";
	// pass over all shebang data (keeping linefeed)
	if (*seek == '#' && *(seek + 1) == '!') {
		seek = (seek+1); do seek++; while (*seek && *seek != 10);
	}

	JSContextScript *ret = g_new0 (JSContextScript, 1);
	ret->content = JSStringRetain(JSStringCreateWithUTF8CString(seek));
	if (url) ret->url = JSStringRetain(JSStringCreateWithUTF8CString(url));

	// initialize parameters
	ret->context = context;
	ret->this = this;

	JSCheckScriptSyntax(ret->context, ret->content, ret->url, 1, &ret->exception);

	return ret;

}

void jse_destroy_script (JSContextScript * obj) {
	JSStringRelease (obj->content); obj->content = NULL;
	JSStringRelease (obj->url); obj->url = NULL;
	g_free (obj);
}

JSValueRef jse_run_script(JSContextScript * script) {
	script->exception = 0;
	return JSEvaluateScript(script->context, script->content, script->this,
		script->url, 1, &script->exception
	);
}

bool jse_set_object_property (JSContextRef ctx, JSObjectRef object, const gchar * name, JSValueRef value) {
	JSStringRef jname = JSStringCreateWithUTF8CString (name); JSValueRef exception = NULL;
	if (value) JSObjectSetProperty (
		ctx, (JSObjectRef) object, jname, value, 0, &exception
	); JSStringRelease (jname);
	return TRUE;
}

extern bool jse_create_function (JSContextRef context, char * name, void * func, JSObjectRef obj) {
  JSObjectRef oref;
  oref = JSObjectMakeFunctionWithCallback (context, NULL, func);
  return jse_set_object_property (context, obj, name, oref);
}

gchar * jse_value_to_string (JSContextRef ctx, JSValueRef val, JSValueRef * exception)
{
  JSStringRef jsstr = NULL;
  JSValueRef func, str;
  gchar *buf = NULL;
  gint length;

  if (val == NULL)
    return NULL;
  else if (JSValueIsUndefined (ctx, val))
    {
      buf = g_strdup ("[undefined]");
    }
  else if (JSValueIsNull (ctx, val))
    {
      buf = g_strdup ("[null]");
    }
  else
    {      
      jsstr = JSValueToStringCopy (ctx, val, NULL);
      length = JSStringGetMaximumUTF8CStringSize (jsstr);
    	if (length > 0) {
		  buf = g_malloc (length * sizeof (gchar));
		  JSStringGetUTF8CString (jsstr, buf, length);
		}
      if (jsstr) JSStringRelease (jsstr);
    }

  return buf;
}

JSObjectRef jse_convert_argv(JSContextRef ctx, int argc, char *argv[]) {
	void * jsargv = JSObjectMakeArray(MainContext, 0, NULL, NULL);
	register i = 0; while (i < argc) {
		JSObjectSetPropertyAtIndex(
			MainContext, jsargv, i, JSValueMakeString(
				MainContext, JSStringCreateWithUTF8CString(argv[i])
			), NULL
		); i++;
	};
	return jsargv;
}

static JSValueRef jse_fn_puts (JSContextRef ctx, JSObjectRef function, JSObjectRef this_object, size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
	char * dummy = jse_value_to_string(ctx,arguments[0],exception);
	int value = puts(dummy); g_free(dummy);
	return JSValueMakeNumber(ctx, (double) value);
}

static JSValueRef jse_fn_getenv (JSContextRef ctx, JSObjectRef function, JSObjectRef this_object, size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
	char * dummy = jse_value_to_string(ctx,arguments[0],exception);
	char * dummy2 = (char*) g_getenv(dummy);
	g_free(dummy);
	return JSValueMakeString(ctx, JSStringCreateWithUTF8CString(dummy2));;
}

// NOT THREAD SAFE *******************
static JSValueRef jse_fn_setenv (JSContextRef ctx, JSObjectRef function, JSObjectRef this_object, size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
	char * dummy = jse_value_to_string(ctx,arguments[0],exception);
	char * dummy2 = jse_value_to_string(ctx,arguments[1],exception);
	JSValueRef ret = JSValueMakeBoolean(ctx, g_setenv(dummy, dummy2, JSValueToBoolean(ctx, arguments[2])));
	g_free(dummy); g_free(dummy2);
	return ret;
}
// ***********************************
static JSValueRef jse_fn_unsetenv (JSContextRef ctx, JSObjectRef function, JSObjectRef this_object, size_t argumentCount, const JSValueRef arguments[], JSValueRef * exception)
{
	char * dummy = jse_value_to_string(ctx,arguments[0],exception);
	g_unsetenv(dummy);
	g_free(dummy);
	return JSValueMakeUndefined(ctx);
}
// ***********************************



int main(int argc, char *argv[], char *envp[]) {

	// we will be using main context group for the duration of runtime.
	MainContextGroup = JSContextGroupRetain(JSContextGroupCreate());
	MainContext = jse_create_context(NULL);

	void * cxgobj = jse_get_context_global_object(MainContext);

	js_native_define_class(MainContext, cxgobj);

	jse_set_object_property(
		MainContext, cxgobj, "argv", jse_convert_argv(MainContext, argc, argv)
	);

	jse_create_function(MainContext, "puts", &jse_fn_puts, cxgobj);
	jse_create_function(MainContext, "getenv", &jse_fn_getenv, cxgobj);
	jse_create_function(MainContext, "setenv", &jse_fn_setenv, cxgobj);

	char * content = NULL; g_file_get_contents(argv[1], &content, NULL, NULL);
	JSContextScript * script = jse_create_script(MainContext, content, argv[1], NULL);
	g_free(content);

	jse_run_script(script);

	JSGarbageCollect(MainContext);
}
