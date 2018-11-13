
#include <bits/stat.h>
#include <time.h>

char * JSE_ERROR_CTOR = "Error";
char * JSE_SINGLE_ARGUMENT = "%s expected 1 parameter, have: %zd";
char * JSE_MULTI_ARGUMENTS = "%s expected %i parameters, have: %zd";
char * JSE_RANGE_ARGUMENTS = "%s expected %i-%i parameters, have: %zd";
char * JSE_AT_LEAST_ARGUMENTS = "%s requires at least %i parameters, have: %zd";
char * JSE_AT_LEAST_ONE_ARGUMENT = "%s requires at least 1 parameter, have: %zd";

JSValue terminate(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc > 1) {
		return THROWING_EXCEPTION(WANT_RANGE_PARAMETERS(0, 1));
	}
	int status = (argc)?JSValueToNumber(ctx, argv[0], exception):0;
	if (*exception) return NULL_VALUE;
	JSTerminate(status);

}


JSValue lastError(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	if (argc != 0) {
		return THROWING_EXCEPTION(WANT_NO_PARAMETERS());
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
		return THROWING_EXCEPTION(WANT_SINGLE_PARAMETER());
	}

	if (JSUnloading()) return NULL_VALUE;
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
	if (argc != 1 && argc != 2) {
		return THROWING_EXCEPTION(WANT_RANGE_PARAMETERS(1, 2));
	}

	JSValue result;
	char * file = (argc)?JSValueToUtf8(ctx, argv[0]):NULL;
	if (*exception) goto fail;
	result = JSLoadPlugin(ctx, file, ((argc > 1)?JSValueToObject(ctx, argv[1], NULL):JSContextGetGlobalObject(ctx)), exception);
	g_free(file);
	fail:
	if (*exception) return NULL_VALUE;
	else return result;
}


JSValue machineTypeRead(JSContext ctx, JSObject function, JSObject this, size_t argc, const JSValue argv[], JSValue * exception)
{
	// address, element?
	if (argc < 1 || argc > 2) {
		return THROWING_EXCEPTION(WANT_RANGE_PARAMETERS(1, 2));
	}

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
	if (argc < 1 || argc > 3) {
		return THROWING_EXCEPTION(WANT_RANGE_PARAMETERS(1, 3));
	}

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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
extern char **environ;

int startCommand(const char* command, char* const parameters[], char* const envrionment[], int ioc, int * iov[][2], int *status) {

  // where int iov[N] == destination, source
  // where int ioc == iov[MAX]
  // where destination == child-process-file-descriptor
  // where source == parent-process-file-descriptor

  // null command is an error.
	// negative ioc is an error.
	// positive ioc with null iov is an error.
	// null status is a nop, otherwise return status destination.

  int nChild;
  int nResult;

	if (envrionment == NULL) envrionment = environ;

  nChild = fork();

  if (0 == nChild) {
    // child continues here
		// replace all io channels in iov with their accompanying substitutes
		for (int i = 0; i < ioc; i++) {
			if (dup2(*iov[i][1], *iov[i][0]) == -1) {
				g_printerr("failed to redirect child process file descriptor (%i) from parent process file descriptor (%i)\n", *iov[i][0], *iov[i][1]);
				if (status) * status = errno;
				exit(errno);
			}
		}
    // run child process image
    // replace this with any exec* function find easier to use ("man exec")
		nResult = execve(command, parameters, envrionment);
		if (status) * status = nResult;

    exit(nResult);

  }

	return nChild;

}