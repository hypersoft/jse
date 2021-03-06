#include <jse.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <sys/wait.h>
#include <signal.h>

extern char * JSE_ERROR_CTOR;
extern char * JSE_SINGLE_ARGUMENT;
extern char * JSE_MULTI_ARGUMENTS;
extern char * JSE_RANGE_ARGUMENTS;
extern char * JSE_AT_LEAST_ARGUMENTS;
extern char * JSE_AT_LEAST_ONE_ARGUMENT;

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}

JSValue jsKill(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc == 2) {
		int pid = JSValueToNumber(ctx, argv[0], exception);
		int sig = JSValueToNumber(ctx, argv[1], exception);
		return JSValueMakeNumber(ctx, kill(pid, sig));
	} 
}

JSValue jsSleep(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc == 1) {
		int ms = JSValueToNumber(ctx, argv[0], exception);
		sleep_ms(ms);
		return JSValueMakeUndefined(ctx);
	} 
}

JSValue source(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{

	if (argc != 1) {
		return THROWING_EXCEPTION(WANT_SINGLE_PARAMETER());
	}

	JSValue result = NULL;
	char * file = JSValueToUtf8(ctx, argv[0]);
	char * contents = NULL;
	GError * error = NULL;
	g_file_get_contents(file, &contents, NULL, &error);

	if (! error) {
		char * data = contents;
		if (g_str_has_prefix(data, "#!")) {
			int c; while (c = data[0]) if (c != '\n') data++; else break;
		}
		JSString source = JSStringFromUtf8(data), url = JSStringFromUtf8(file);
		JSValue result = JSEvaluateScript(ctx, source, this, url, 1, exception);
		JSStringRelease(source); JSStringRelease(url);
		if (*exception) goto fail;
	} else {
		*exception = JSExceptionFromGError(ctx, error);
		g_error_free(error);
	}
	fail:
	free(file);	g_free(contents);
	return (*exception)?JSValueMakeNull(ctx):result;

}

JSValue printErrorLine(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc != 1) {
		return THROWING_EXCEPTION(WANT_SINGLE_PARAMETER());
	}

	JSValue result;
	char * file = (argc)?JSValueToUtf8(ctx, argv[0]):NULL;
	g_printerr("%s\n", file);
	g_free(file);
	return JSValueMakeBoolean(ctx, true);
}


JSValue echo(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	unsigned i, argFinal = argc - 1;
	char *bytes;
	bool have_newline = false;
	for (i = 0; i < argc; i++) {
		bytes = JSValueToUtf8(ctx, argv[i]);
		if (! bytes) continue;
		g_print("%s%s", i ? " " : "", bytes);
		free(bytes);
	}

	putc('\n', stdout); 
	fflush(stdout);

	return JSValueMakeBoolean(ctx, true);

}

JSValue checkSyntax(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{

	if (argc == 0) {
		return THROWING_EXCEPTION(WANT_AT_LEAST_ONE_PARAMETER());
	}

	JSObject parameters = (JSObject)JSObjectGetUtf8Property(ctx, JSContextGetGlobalObject(ctx), "parameter");
	JSString script, source;
	script = JSValueToString(ctx, argv[0], exception);
	int line = 1;
	if (exception && *exception) return NULL;
	if (argc > 1) {
		source = JSValueToStringCopy(ctx, argv[1], exception);
		if (exception && *exception) {
			JSStringRelease(script);
			return NULL;
		}
		if (argc > 2) line = JSValueToNumber(ctx, argv[2], exception);
		if (exception && *exception) {
			JSStringRelease(script);
			return NULL;
		}
	} else {
		JSValue v = JSObjectGetPropertyAtIndex(ctx, parameters, 0, exception);
		if (exception && *exception) {
			JSStringRelease(script);
			return NULL;
		}
		source = JSValueToStringCopy(ctx, v, exception);
		if (exception && *exception) {
			JSStringRelease(script);
			return NULL;
		}
	}

	bool value = JSCheckScriptSyntax(ctx, script, source, line, exception);
	JSStringRelease(script); JSStringRelease(source);
	return JSValueMakeBoolean(ctx, value);
}

JSValue run(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{

	if (argc < 1) {
		return THROWING_EXCEPTION(WANT_AT_LEAST_ONE_PARAMETER());
	}

	int allocated = 0, deallocated = 0;
	gchar *exec_child_out = NULL, *exec_child_err = NULL; gint exec_child_status = 0;

	JSObject exec = NULL;

	char * argument[argc + 1];
	int index = 0;
	while (index < argc) {
		argument[index] = JSValueToUtf8(ctx, argv[index]);
		index++;
	}
	argument[index] = NULL;
	if (g_spawn_sync(NULL, argument, NULL, G_SPAWN_LEAVE_DESCRIPTORS_OPEN | G_SPAWN_SEARCH_PATH | G_SPAWN_CHILD_INHERITS_STDIN, NULL, NULL, &exec_child_out, &exec_child_err, &exec_child_status, NULL)) {
		exec_child_status = WEXITSTATUS(exec_child_status);
		exec = JSValueToObject(ctx,JSInlineEval(ctx, "Object.create(this.prototype)", function, NULL), NULL);
		JSObjectSetUtf8Property(ctx, exec, "stdout", JSValueFromUtf8(ctx, exec_child_out), 0);
		g_free(exec_child_out);
		JSObjectSetUtf8Property(ctx, exec, "stderr", JSValueFromUtf8(ctx, exec_child_err), 0);
		g_free(exec_child_err);
		JSObjectSetUtf8Property(ctx, exec, "status", JSValueFromNumber(ctx, exec_child_status), 0);
	}

	while (index) g_free(argument[--index]);

	return (JSValue) exec;
}

JSValue jsLocalPath(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc == 0) {
		char buffer[8192]; getcwd(buffer, sizeof(buffer));
		return JSValueFromUtf8(ctx, buffer);
	} else if (argc == 1) {
		char * txt = JSValueToUtf8(ctx, argv[0]);
		if (!g_file_test(txt, G_FILE_TEST_IS_DIR)) {
			*exception = JSExceptionFromUtf8(ctx, "ReferenceError", "localPath: `%s' is not a directory", txt);
			g_free(txt);
			return JSValueMakeNull (ctx);
		}
		int result = chdir(txt);
		g_free(txt);
		return JSValueMakeBoolean(ctx, result != -1);
	} else {
		return THROWING_EXCEPTION(WANT_RANGE_PARAMETERS(0, 1));
	}
}

bool readline_has_initialized = false;

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
		*exception = JSExceptionFromUtf8(ctx, "Error", "readline expected 1 parameter, got %zd", argc);
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


JSValue load(JSContext ctx, char * path, JSObject global, JSValue * exception)
{

	JSObjectCreateFunction(ctx, global, "sleep", jsSleep);
	JSObjectCreateFunction(ctx, global, "kill", jsKill);

	JSObjectCreateFunction(ctx, global, "source", source);
	JSObjectCreateFunction(ctx, global, "printErrorLine", printErrorLine);
	JSObjectCreateFunction(ctx, global, "echo", echo);
	JSObjectCreateFunction(ctx, global, "checkSyntax", checkSyntax);
	JSObjectCreateFunction(ctx, global, "run", run);
	JSObjectCreateFunction(ctx, global, "localPath", jsLocalPath);
	JSObjectCreateFunction(ctx, global, "readLine", gnu_readline);

	return (JSValue) global;

}
