/*
 * File:   JseMain.c
 * Author: triston
 *
 * Created on December 26, 2014, 5:06 AM
 */

#include "jse.h"

struct jse_s {
	char * command;
	JSUniverse universe;
	JSContext ctx;
	gboolean unloading, silent;
	GPtrArray * dlPath, * dlLib;
} jse;

#include "JseFunctions.c"

JSUniverse JSContextGetUniverse()
{
	return jse.universe;
}

void JSInitScriptArguments(JSContext ctx, int argc, char ** argv)
{
	unsigned i = 0;
	JSValue array[argc];
	for (i; i < argc; i++) array[i] = JSValueFromUtf8(ctx, argv[i]);
	JSObject arrayObject = JSObjectMakeArray(ctx, argc, array, NULL);
	JSObjectSetUtf8Property(
		ctx, JSContextGetGlobalObject(ctx), "argv", (JSValue) arrayObject,
		kJSPropertyAttributeDontDelete
	);
}

JSValue JSEvaluateUtf8(JSContext ctx, char * script, JSObject object, char * file, int line, JSValue * exception)
{
	JSString
		string = JSStringFromUtf8(script),
		url = JSStringFromUtf8(file);
	JSValue result = JSEvaluateScript(
		ctx, string, object, url, line, exception
	);
	JSStringRelease(string); JSStringRelease(url);
	return result;
}

static void jse_at_exit()
{
/*
	Reverse iterate through plug-ins, and call unload [if present] on each plug-in...
	Plug-ins loaded multiple times will have unload called multiple times...
	Plug-ins are advised to maintain a 'loaded' or 'loadCount' variable...
*/

	int i = jse.dlLib->len;
	while (i) {
		void * plugin = g_ptr_array_index(jse.dlLib, --i);
		void (*unload)(JSContext) = dlFindSymbol(plugin, "unload");
		if (unload) unload(jse.ctx);
	}

	JSGlobalContextRelease(jse.ctx);
	JSGarbageCollect((JSContext)jse.ctx);
	if (jse.universe) JSContextGroupRelease(jse.universe);

	g_ptr_array_free(jse.dlPath, TRUE);
	g_ptr_array_free(jse.dlLib, TRUE);

}

JSValue JSLoadPlugin(JSContext ctx, char * plugin, JSObject object, JSValue * error)
{
	if (jse.unloading) return JSValueMakeNull(ctx);
	if (! object) object = JSContextGetGlobalObject(ctx);
	else if (!JSValueIsObject(ctx, (JSValue)object)) {
		if (error) *error = JSExceptionFromUtf8(ctx, "ReferenceError", "Can't load plugin: target is not an object");
		return JSValueMakeNull (ctx);
	}
	int i; void * pLib = NULL; char path[4096];
	if (g_file_test(plugin, G_FILE_TEST_EXISTS)) {
		sprintf(path, "%s", plugin);
		pLib = dlLoadLibrary(plugin);
	} else for (i = 0; i < jse.dlPath->len; i++) {
		sprintf(path, "%s/%s", g_ptr_array_index(jse.dlPath, i), plugin);
		if (pLib = dlLoadLibrary(path)) break;
	}
	if (! pLib ) {
		if (error) *error = JSExceptionFromUtf8(ctx, "ReferenceError", "Couldn't load plugin %s", plugin);
		return JSValueMakeNull(ctx);
	}
	JSValue (*load)(JSContext, char *, JSObject, JSValue *) = dlFindSymbol(pLib, "load");
	if (! load ) {
		if (error) *error = JSExceptionFromUtf8(ctx, "ReferenceError", "Couldn't locate plugin initialize function `%s' in `%s'", "load", plugin);
		return JSValueMakeNull(ctx);
	}
	g_ptr_array_add(jse.dlLib, pLib);
	return load(jse.ctx, path, object, error);
}

void JSAddPluginPath(char * path) /* adds a copy of path to the search path */
{
	g_ptr_array_insert(jse.dlPath, 0, g_strdup(path));
}

void JSInit(char * command, JSContext ctx) {

	atexit(jse_at_exit);
	JSErrorQuark = g_quark_from_static_string ("Error");

	memset(&jse, 0, sizeof(jse));

	jse.command = command;

	if (ctx) {
		jse.ctx = ctx;
		jse.universe = JSContextGetGroup(ctx);
	} else {
		jse.ctx = JSGlobalContextCreateInGroup(
			jse.universe = JSContextGroupCreate(), NULL
		);
	}

	jse.dlPath = g_ptr_array_new_with_free_func (g_free);
	jse.dlLib = g_ptr_array_new_with_free_func ((GDestroyNotify)dlFreeLibrary);

	g_ptr_array_add(jse.dlPath, g_strdup("share/plugin"));
	g_ptr_array_add(jse.dlPath, g_strdup("~/.local/jse/plugin"));
	g_ptr_array_add(jse.dlPath, g_strdup("/usr/share/jse/plugin"));

	JSObject global = JSContextGetGlobalObject(jse.ctx);

	JSObjectCreateFunction(jse.ctx, global, "include", include);

	// ...
	JSDefineConstant(jse.ctx, global, O_RDONLY);
	JSDefineConstant(jse.ctx, global, O_RDWR);
	JSDefineConstant(jse.ctx, global, O_WRONLY);
	JSDefineConstant(jse.ctx, global, O_CREAT);
	JSDefineConstant(jse.ctx, global, O_APPEND);
	JSDefineConstant(jse.ctx, global, O_CLOEXEC);
	JSDefineConstant(jse.ctx, global, O_DIRECTORY);
	JSDefineConstant(jse.ctx, global, O_DSYNC);
	JSDefineConstant(jse.ctx, global, O_NOCTTY);
	JSDefineConstant(jse.ctx, global, O_NOFOLLOW);
	JSDefineConstant(jse.ctx, global, O_NONBLOCK);
	JSDefineConstant(jse.ctx, global, O_RSYNC);
	JSDefineConstant(jse.ctx, global, O_SYNC);
	JSDefineConstant(jse.ctx, global, O_TRUNC);

	JSDefineConstant(jse.ctx, global, S_IRUSR);
	JSDefineConstant(jse.ctx, global, S_IWUSR);
	JSDefineConstant(jse.ctx, global, S_IXUSR);
	JSDefineConstant(jse.ctx, global, S_IRWXU);

	JSDefineConstant(jse.ctx, global, S_IRGRP);
	JSDefineConstant(jse.ctx, global, S_IWGRP);
	JSDefineConstant(jse.ctx, global, S_IXGRP);
	JSDefineConstant(jse.ctx, global, S_IRWXG);

	JSDefineConstant(jse.ctx, global, S_IROTH);
	JSDefineConstant(jse.ctx, global, S_IWOTH);
	JSDefineConstant(jse.ctx, global, S_IXOTH);
	JSDefineConstant(jse.ctx, global, S_IRWXO);

	JSObjectCreateFunction(jse.ctx, global, "lastError", lastError);
	JSObjectCreateFunction(jse.ctx, global, "exit", terminate);
	JSObjectCreateFunction(jse.ctx, global, "printErrorLine", printErrorLine);
	JSObjectCreateFunction(jse.ctx, global, "echo", echo);
	JSObjectCreateFunction(jse.ctx, global, "loadPlugin", loadPlugin);
	JSObjectCreateFunction(jse.ctx, global, "addPluginPath", addPluginPath);
	JSObjectCreateFunction(jse.ctx, global, "checkSyntax", checkSyntax);
	JSObjectCreateFunction(jse.ctx, global, "run", run);

	JSObjectCreateFunction(jse.ctx, global, "machineTypeRead", machineTypeRead);
	JSObjectCreateFunction(jse.ctx, global, "machineTypeWrite", machineTypeWrite);

	JSLoadPlugin(jse.ctx, "GNUReadLine.jso", global, NULL);
	JSLoadPlugin(jse.ctx, "Environment.jso", global, NULL);

	JSValue jsError = NULL;
	JSInlineEval(jse.ctx, "include('/usr/share/jse/core.js')", NULL, &jsError);
	if (jsError) {
		JSReportException(jse.ctx, jse.command, jsError);
		JSTerminate(1);
	}

	errno = 0;

}

gboolean JSUnloading()
{
	return jse.unloading;
}

void JSTerminate(unsigned status)
{
	if (jse.unloading) return;
	jse.unloading = TRUE;
	exit(status);
}

JseOption jseOptions[] = {
	{OPT_WORD | OPT_ARG, 0, "--init"},
	{OPT_MINUS | OPT_LONG, 's', "silent"},
	{OPT_WORD | OPT_ARG, 0, "-L"},
	{OPT_WORD | OPT_ARG, 0, "-l"},
	{OPT_UNIFIED | OPT_ARG, 'e', "eval"},
	{OPT_UNIFIED | OPT_ARG, 'c', "command"},
	{0}
};

GError * jse_parse_options (
	JseOptionType flag,
	JseOption * option,
	const char * value
) {
	static int evalNumber = 0;
	if (option == &jseOptions[0]) {
		int argc; char ** argv;
		char buffer[strlen(value)+7]; g_sprintf(buffer, "shell %s", value);
		g_shell_parse_argv(buffer, &argc, &argv, NULL);
		JseOptionParseResult * parseResult = jse_option_parse_main (
			argc, argv, jseOptions, jse_parse_options
		);
		if (parseResult->error) {
			JSReportError(jse.command, parseResult->error);
			jse_option_parse_result_free(parseResult);
			g_strfreev(argv);
			JSTerminate(1);
		} else {
			jse_option_parse_result_free(parseResult);
			g_strfreev(argv);
			return NULL;
		}
	} else if (option == &jseOptions[1]) {
		jse.silent = TRUE;
		return NULL;
	} else if (option == &jseOptions[2]) {
		if (!g_file_test(value, G_FILE_TEST_IS_DIR)) return NULL;
		g_ptr_array_insert(jse.dlPath, 0, g_strdup(value));
		return NULL;
	} else if (option == &jseOptions[3]) {
		JSValue result, jsError = NULL;
		(void) JSLoadPlugin(jse.ctx, (char*)value, NULL, &jsError);
		if (jsError) return JSExceptionToGError(jse.ctx, jsError);
		return NULL;
	} else if (option == &jseOptions[4]) {
		char evalNo[512]; sprintf(evalNo, "eval[%d]", evalNumber++);
		JSValue jsError = NULL, result = JSEvaluateUtf8(
			jse.ctx, (char*)value, NULL, evalNo, 1, &jsError
		);
		if (jsError) return JSExceptionToGError(jse.ctx, jsError);
		if (flag == OPT_PLUS || jse.silent) return NULL;
		value = JSValueToUtf8(jse.ctx, result);
		g_print("%s\n", value);
		g_free((char*)value);
		return NULL;
	} else if (option == &jseOptions[5]) {
		char evalNo[512]; sprintf(evalNo, "eval[%d]", evalNumber++);
		JSValue jsError = NULL, result = JSEvaluateUtf8(
			jse.ctx, (char*)value, NULL, evalNo, 1, &jsError
		);
		if (jsError) return JSExceptionToGError(jse.ctx, jsError);
		if (flag == OPT_PLUS || jse.silent) JSTerminate(0);
		value = JSValueToUtf8(jse.ctx, result);
		g_print("%s\n", value);
		g_free((char*)value);
		JSTerminate(0);
	}
	g_assert_not_reached();
}

int jse_file_mode(char * file)
{
	char * fileContents = NULL, * fileData;
	{
		GError * error = NULL;
		if (!g_file_get_contents(file, &fileContents, NULL, &error)) {
			int final = error->code;
			JSReportError(jse.command, error);
			g_error_free(error);
			JSTerminate(final);
		}
	}

	JSValue jsError = NULL, result = NULL;

	// skip any shebang line..
	fileData = fileContents;
	if (g_str_has_prefix(fileData, "#!")) {
		int c; while (c = fileData[0]) if (c != '\n') fileData++; else break;
	}
	JSString
		script = JSStringFromUtf8(fileData),
		url = JSStringFromUtf8(file);
		result = JSEvaluateScript(
		jse.ctx, script, NULL, url, 1, &jsError
	);

	JSStringRelease(script); JSStringRelease(url); g_free(fileContents);

	if (jsError) {
		JSReportException(jse.ctx, file, jsError);
		JSTerminate(1);
	}

	if (! jse.silent ) {
		char * value = JSValueToUtf8(jse.ctx, result);
		g_print("%s\n", value);
		g_free(value);
	}

	JSTerminate(0);

}

void jse_tty_mode()
{
	jse.silent = true;
	jse_file_mode("/usr/share/jse/interactive.js");
}

int main(int argc, char** argv)
{
	JSInit(argv[0], NULL);

	// parse arguments...
	if (argc > 1) {
		JseOptionParseResult * parseResult = jse_option_parse_main (
			argc, argv, jseOptions, jse_parse_options
		);
		if (parseResult->error && parseResult->error->code) {
			JSReportError(jse.command, parseResult->error);
			int final = parseResult->error->code;
			jse_option_parse_result_free(parseResult);
			JSTerminate(final);
		} else {
			argc -= parseResult->argi;
			argv += parseResult->argi;
			jse_option_parse_result_free(parseResult);
		}
	}
	/*

		File and script arguments should be remaining..
		If file is - then file is standard input.

		JSE Has no "interactive" mode. Any such requirement should be provided
		via plug-in, or script.

	*/

	char * devStdin = "/dev/stdin";
	char * file = NULL;

	if (argv[0] == jse.command) argv[0] = devStdin;
	else {
		if (!g_file_test(argv[0], G_FILE_TEST_EXISTS)) argv[0] = devStdin;
		else if (!g_strcmp0(argv[0], "-")) argv[0] = devStdin;
	}

	file = argv[0];

	/*
		Initialize script arguments
	*/

	JSInitScriptArguments(jse.ctx, argc, argv);

	if (file == devStdin && isatty(0) ) jse_tty_mode();
	else jse_file_mode(file);

	g_assert_not_reached();

	return 0;

}
