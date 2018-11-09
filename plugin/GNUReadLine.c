
#include <jse.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <ffi.h>
#include <sys/mman.h>

JSClass ReadLine = NULL;
JSUniverse universe = NULL;
gboolean readline_has_initialized = FALSE;

static void
gnu_handle_readline_closure(ffi_cif * cif, void *result, void **args, void *userdata)
{
  JSContext ctx = JSGlobalContextCreateInGroup(universe, NULL);
  JSValue exception = 0;
  JSObject function = (JSObject) userdata;

  JSObjectCallAsFunction(ctx, function, 0, 0, 0, &exception);
  if (exception)
    {
      gchar *mes = JSExceptionToUtf8(ctx,
					    exception);
      g_warning("Exception in readline bind key closure. %s \n", mes);
	  g_free(mes);
    }
  JSGlobalContextRelease(ctx);
}

// exists for lifetime of program
static ffi_closure *gnu_make_readline_closure(JSObject function)
{
  ffi_cif *cif;
  ffi_closure *closure;

  cif = g_new0(ffi_cif, 1);
  closure = mmap(0, sizeof(ffi_closure), PROT_READ | PROT_WRITE |
		 PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
  ffi_prep_cif(cif, FFI_DEFAULT_ABI, 0, &ffi_type_sint, 0);
  ffi_prep_closure(closure, cif, gnu_handle_readline_closure, function);

  return closure;
}

static JSValue
gnu_readline_bind(JSContext ctx,
		   JSObject function,
		   JSObject this_object,
		   size_t argc,
		   const JSValue arguments[], JSValue * exception)
{
  gchar *key;
  ffi_closure *c;

	if (argc != 2) {
		*exception = JSExceptionFromUtf8(ctx, "Error", "bind expected 2 arguments, got %zd", argc);
		return JSValueMakeNull (ctx);
	}

  key = JSValueToUtf8(ctx, arguments[0]);
  c = gnu_make_readline_closure((JSObject) arguments[1]);

  rl_bind_key(*key, (void *) c);

  g_free(key);

  return JSValueMakeNull(ctx);
}

static JSValue
gnu_readline_done(JSContext ctx,
	     JSObject function,
	     JSObject this_object,
	     size_t argc,
	     const JSValue arguments[],
	     JSValue * exception)
{
	if (argc != 0) {
		*exception = JSExceptionFromUtf8(ctx, "Error", "done expected 0 arguments, got %zd", argc);
		return JSValueMakeNull (ctx);
	}
  rl_done = 1;
  return JSValueMakeNull (ctx);
}

static JSValue
gnu_readline_buffer(JSContext ctx,
	     JSObject function,
	     JSObject this_object,
	     size_t argc,
	     const JSValue arguments[],
	     JSValue * exception)
{
  return JSValueFromUtf8 (ctx, rl_line_buffer);
}

static JSValue
gnu_readline_insert(JSContext ctx,
	       JSObject function,
	       JSObject this_object,
	       size_t argc,
	       const JSValue arguments[],
	       JSValue * exception)
{
  gchar *ins;
  gint ret;

	if (argc != 1) {
		*exception = JSExceptionFromUtf8(ctx, "Error", "insert expected 1 argument, got %zd", argc);
		return JSValueMakeNull (ctx);
	}

  ins = JSValueToUtf8(ctx, arguments[0]);
  ret = rl_insert_text (ins);
  g_free (ins);

  return JSValueFromNumber (ctx, ret);
}

static JSValue
gnu_readline(JSContext ctx,
	      JSObject function,
	      JSObject this_object,
	      size_t argc,
	      const JSValue arguments[], JSValue * exception)
{
  JSValue valstr = 0;
  gchar *str = NULL;
  gchar *buf;
  const gchar *histfname = g_get_home_dir();
  gchar *path = g_build_filename(histfname, "history.jse", NULL);

  if (!readline_has_initialized)
    {
      read_history(path);
      readline_has_initialized = TRUE;
    }

 	if (argc != 1) {
		*exception = JSExceptionFromUtf8(ctx, "Error", "readline expected 1 argument, got %zd", argc);
		return JSValueMakeNull (ctx);
	}

  buf = JSValueToUtf8(ctx, arguments[0]);

  str = readline(buf);
  if (str && *str)
    {
      add_history(str);
      valstr = JSValueFromUtf8(ctx, str);
      g_free(str);
    }

  write_history(path);
  history_truncate_file(path, 1000);

  g_free(buf);
  g_free(path);

  if (valstr == 0)
    valstr = JSValueMakeNull(ctx);

  return valstr;
}

JSStaticFunction ReadLineFunctions[] = {
	{"bind", (void*)gnu_readline_bind, 0},
	{"done", (void*)gnu_readline_done, 0},
	{"buffer", (void*)gnu_readline_buffer, 0},
	{"insert", (void*)gnu_readline_insert, 0},
	{NULL, NULL, 0}
};

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{
	if (! universe ) {
		JSClassDefinition definition = JSClassDefinitionEmpty;
		definition.staticFunctions = ReadLineFunctions;
		definition.callAsFunction = (void*) gnu_readline;

		ReadLine = JSClassCreate(&definition);
		universe = JSContextGetUniverse();
	}

	JSObject ReadLineObject = JSObjectMake(ctx, ReadLine, 0);

	JSObjectSetUtf8Property(ctx, object, "readline", (JSValue) ReadLineObject, 0);
	return (JSValue) object;

}

void unload(JSContext ctx)
{
	JSClassRelease(ReadLine);
}
