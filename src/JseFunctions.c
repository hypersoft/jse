
#include <bits/stat.h>
#include <time.h>
static char * JSE_ERROR_CTOR = "Error";
static char * JSE_SINGLE_ARGUMENT = "%s expected 1 argument, got %zd";
static char * JSE_MULTI_ARGUMENTS = "%s expected %i arguments, got %zd";

JSValue terminate(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{

	int status = (argc)?JSValueToNumber(ctx, argv[0], exception):0;
	if (*exception) return JSValueMakeNull(ctx);
	JSTerminate(status);

}

JSValue include(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc != 1) {
		if (exception)
			*exception = JSExceptionFromUtf8
			(ctx, JSE_ERROR_CTOR, JSE_SINGLE_ARGUMENT, __FUNCTION__, argc);
		return JSValueMakeNull (ctx);
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

JSValue lastError(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc != 0) {
		if (exception)
			*exception = JSExceptionFromUtf8
			(ctx, JSE_ERROR_CTOR, JSE_MULTI_ARGUMENTS, 0, __FUNCTION__, argc);
		return JSValueMakeNull (ctx);
	}
	int error = errno;
	JSObject detail = JSValueToObject(ctx, JSValueFromUtf8(ctx, strerror(error)), NULL);
	JSObjectSetUtf8Property(ctx, detail, "errno", JSValueFromNumber(ctx, error), 0);
	errno = 0;
	return detail;
}

JSValue addPluginPath(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc != 1) {
		if (exception)
			*exception = JSExceptionFromUtf8
			(ctx, JSE_ERROR_CTOR, JSE_SINGLE_ARGUMENT, __FUNCTION__, argc);
		return JSValueMakeNull (ctx);
	}
	if (JSUnloading()) return JSValueMakeNull(ctx);
	char * value = (argc)?JSValueToUtf8(ctx, argv[0]):NULL;
	if (g_file_test(value, G_FILE_TEST_IS_DIR))
		JSAddPluginPath(value);
	else {
		*exception = JSExceptionFromUtf8(ctx, "ReferenceError", "addPluginPath: `%s' is not a directory", value);
		g_free(value);
		return JSValueMakeNull (ctx);
	}
	g_free(value);
	return argv[0];
}

JSValue loadPlugin(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc != 1) {
		if (exception)
			*exception = JSExceptionFromUtf8
			(ctx, JSE_ERROR_CTOR, JSE_SINGLE_ARGUMENT, __FUNCTION__, argc);
		return JSValueMakeNull (ctx);
	}
	JSValue result;
	char * file = (argc)?JSValueToUtf8(ctx, argv[0]):NULL;
	if (*exception) goto fail;
	result = JSLoadPlugin(ctx, file, this, exception);
	g_free(file);
	fail:
	if (*exception) return JSValueMakeNull(ctx);
	else return result;
}

JSValue printErrorLine(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc != 1) {
		if (exception)
			*exception = JSExceptionFromUtf8
			(ctx, JSE_ERROR_CTOR, JSE_SINGLE_ARGUMENT, __FUNCTION__, argc);
		return JSValueMakeNull (ctx);
	}
	JSValue result;
	char * file = (argc)?JSValueToUtf8(ctx, argv[0]):NULL;
	g_printerr("%s\n", file);
	g_free(file);
	return JSValueMakeBoolean(ctx, true);
}

bool buffer_ends_with_newline(register char * buffer, int length) {
	if (buffer == NULL) return false;
	if (length == 0) length = strlen(buffer);
	if (length == 0) return false;
	register int check = length - 1;
	if (buffer[check] == 10) return true;
	if (buffer[check] == 0 && buffer[check - 1] == 10) return true;
	return false;
}

JSValue echo(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	uint i, argFinal = argc - 1;
	char *bytes;
	bool have_newline = false;
	for (i = 0; i < argc; i++) {
		bytes = JSValueToUtf8(ctx, argv[i]);
		if (! bytes) continue;
		g_print("%s%s", i ? " " : "", bytes);
		if (i == argFinal) have_newline = buffer_ends_with_newline(bytes, 0);
		free(bytes);
	}

	if (!have_newline) putc('\n', stdout); 
	
	fflush(stdout);

	return JSValueMakeBoolean(ctx, true);

}

JSValue checkSyntax(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{

	if (argc == 0) {
		*exception = JSExceptionFromUtf8(ctx, "Error", "checkSyntax expected 1-3 arguments, got %zd", argc);
		return JSValueMakeNull (ctx);
	}
	JSObject arguments = (JSObject)JSObjectGetUtf8Property(ctx, JSContextGetGlobalObject(ctx), "argv");
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
		JSValue v = JSObjectGetPropertyAtIndex(ctx, arguments, 0, exception);
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
		exec = JSValueToObject(ctx,JSInlineEval(ctx, "Object.create(this.prototype)", this, NULL), NULL);
		JSObjectSetUtf8Property(ctx, exec, "stdout", JSValueFromUtf8(ctx, exec_child_err), 0);
		g_free(exec_child_out);
		JSObjectSetUtf8Property(ctx, exec, "stderr", JSValueFromUtf8(ctx, exec_child_err), 0);
		g_free(exec_child_err);
		JSObjectSetUtf8Property(ctx, exec, "status", JSValueFromNumber(ctx, exec_child_status), 0);
	}

	while (index) g_free(argument[--index]);

	return (JSValue) exec;
}

JSValue machineTypeRead(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	// address, element?
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
