
#include <jse.h>

#include <readline/readline.h>
#include <readline/history.h>

JSClass ReadLine = NULL;
JSUniverse universe = NULL;
gboolean readline_has_initialized = FALSE;


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
		g_free(buf);

	if (str == NULL) {
		putc('\n', stderr);
		return JSValueMakeNumber(ctx, -1);
	}

  if (str && *str)
    {
      add_history(str);
      valstr = JSValueFromUtf8(ctx, str);
      g_free(str);
			write_history(path);
			history_truncate_file(path, 1000);
    }

  g_free(path);

  if (valstr == 0)
    valstr = JSValueMakeNull(ctx);

  return valstr;
}

JSValue load(JSContext ctx, char * path, JSObject object, JSValue * exception)
{
	if (! universe ) {
		JSClassDefinition definition = JSClassDefinitionEmpty;
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
