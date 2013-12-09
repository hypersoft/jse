const char * JSTReservedAddress =

	"Hypersoft Systems JSE Copyright 2013 Triston J. Taylor, All Rights Reserved.";

	const char * CODENAME = JSE_CODENAME;
	const char * VERSION = JSE_BUILDNO;
	const char * VENDOR = JSE_VENDOR;

#include "JSTools.inc"
#include "JSTInit.inc"

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]);
JSTObject JSTNativeInit_ JSTUtility(JSTObject js);
JSTValue JSTFunctionCall_(register JSTContext ctx, JSTValue *exception, JSTObject method, JSTObject object, ...);
JSTObject JSTFunctionCallback_ JSTUtility(char * p, void * f);
JSTValue JSTValueFromJSON_ JSTUtility(char * p);
static JSValueRef jsExecute JSTDeclareFunction ();
char * JSTConstructUTF8(char * format, ...);

#include "JSTools/Native.inc"

char * JSTConstructUTF8(char * format, ...) {
	va_list ap; va_start(ap, format); char * message = NULL;
	vasprintf(&message, format, ap); return message;
}

JSObjectRef jsToolsPasswdToObject(JSContextRef ctx, uid_t uid, JSValueRef * exception) {
	JSObjectRef result = JSTClassInstance(NULL, NULL);
	struct passwd * pws = getpwuid(uid);
	if (pws) {
		JSTObjectSetProperty(result, "name", JSTValueFromUTF8(pws->pw_name), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "password", JSTValueFromUTF8(pws->pw_passwd), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "id", JSTValueFromDouble(uid), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "gid", JSTValueFromDouble(pws->pw_gid), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "comment", JSTValueFromUTF8(pws->pw_gecos), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "home", JSTValueFromUTF8(pws->pw_dir), JSTObjectPropertyRequired);
		JSTObjectSetProperty(result, "shell", JSTValueFromUTF8(pws->pw_shell), JSTObjectPropertyRequired);
	} else {
		char out[PATH_MAX];
		sprintf(out, "%s: errno %lu", "JSE: unable to construct system user object", errno);
		JSTScriptReferenceError(out);
		return JSTObjectUndefined;
	}
	return result;
}

extern char *secure_getenv(const char *name);

static JSValueRef jsToolsEnvRead JSTDeclareFunction (String) {

	if (argc < 1) {
		JSTScriptSyntaxError("js.env.read: expected argument: string");
		return JSTValueUndefined;
	}

	if (!JSTValueIsString(argv[0])) {
		JSTScriptTypeError("js.env.read: expected string");
	} else {
		char * key = JSTStringToUTF8(JSTValueToString(argv[0]), true);
		char * value = secure_getenv(key); JSTStringFreeUTF8(key);
		return value?JSTValueFromUTF8(value):JSTValueUndefined;
	}

	return JSTValueUndefined;

}

static JSValueRef jsToolsEnvWrite JSTDeclareFunction (String) {

	if (argc < 2) {
		JSTScriptSyntaxError("js.env.write: expected string arguments: key, value");
		return JSTValueUndefined;
	}

	JSValueRef result = JSTValueUndefined;

	if (!JSTValueIsString(argv[0]) || !JSTValueIsString(argv[1])) {
		JSTScriptTypeError("js.env.write: expected string arguments");
	} else {
		char * key = JSTStringToUTF8(JSTValueToString(argv[0]), true);
		char * value = JSTStringToUTF8(JSTValueToString(argv[1]), true);
		result = JSTValueFromDouble(setenv(key, value, true));
		JSTStringFreeUTF8(key); JSTStringFreeUTF8(value);
	}

	return result;

}

static JSValueRef jsToolsEnvDelete JSTDeclareFunction (string) {
	if (argc < 1) {
		JSTScriptSyntaxError("js.env.delete: expected argument: string");
		return JSTValueUndefined;
	}
	JSValueRef result = JSTValueUndefined;
	if (!JSTValueIsString(argv[0])) {
		JSTScriptTypeError("js.env.delete: expected string");
	} else {
		char * key = JSTStringToUTF8(JSTValueToString(argv[0]), true);
		result = JSTValueFromDouble(unsetenv(key));
		JSTStringFreeUTF8(key);
	}
	return result;
}

static JSValueRef jsToolsEnvKeys JSTDeclareFunction (string) {
	char **env = JSTValueToPointer(JSTScriptNativeEval("js.run.envp", NULL));
	size_t count = 0; while(env[count++]);
	size_t i = 0; char key[PATH_MAX];
	JSObjectRef result = JSTClassInstance(NULL, NULL);
	while(env[i]) {
		sscanf(env[i++], "%[^=]", key);
		JSTObjectSetProperty(result, key, JSTValueUndefined, 0);
	}
	return result;
}

static JSTValue jsToolsEnvUser JSTDeclareFunction () {
	if (JSTValueIsNumber(argv[0])) return (JSTValue) jsToolsPasswdToObject(ctx, JSTValueToDouble(argv[0]), exception);
	return JSTValueUndefined;
}

static JSValueRef jsToolsSource JSTDeclareFunction (file, [global object]) {
	JSTValue result;
	char *script = NULL, *src = NULL, *file = (argc)?JSTValueToUTF8(argv[0]):NULL;
	if (g_file_get_contents(file, &src, NULL, NULL)) {
		script = src; int c = 0;
		if (*script == '#' && *(script+1) == '!') {
			script+=2; while ((c = *script) && c != '\n') script++;
		}
		result = JSTScriptEval(script, (argc == 2)?(JSTObject)argv[1]:NULL, file, 1); g_free(src);
	} else {
		JSTScriptNativeError("unable to get file `%s' contents", file);
	}
	JSTStringFreeUTF8(file);
	return result;
}

static JSValueRef jst_io_path JSTDeclareFunction () {
	if (argc == 0) {
		char buffer[PATH_MAX]; getcwd(buffer, PATH_MAX);
		return JSTValueFromUTF8(buffer);
	}
	char * val; JSValueRef
	result = JSTValueFromDouble(chdir(val = JSTStringToUTF8(JSTStringFromValue(argv[0]), true)));
	JSTStringFreeUTF8(val);
	return result;
}

static JSValueRef jst_io_stream_print JSTDeclareFunction () {
	size_t i; char * output; double count = 0;
	FILE * stream = JSTValueToPointer(argv[0]);
	for (i = 1; i < argc; i++) {
		output = JSTStringToUTF8(JSTValueToString(argv[i]), true);
		if (output) count += fprintf(stream, "%s", output);
		JSTStringFreeUTF8(output);
	}
	return JSTValueFromDouble(count);
}

static JSValueRef jst_io_stream_pointer JSTDeclareFunction () {
	if (argc > 0) {
		int stream = JSTValueToDouble(argv[0]);
		if (stream == 0) return JSTValueFromPointer(stdin);
		if (stream == 1) return JSTValueFromPointer(stdout);
		if (stream == 2) return JSTValueFromPointer(stderr);
	}
	return JSTValueUndefined;
}

static JSValueRef jst_error_number JSTDeclareFunction () {
	if (argc == 0) return JSTValueFromDouble(errno);
	else errno = JSTValueToDouble(argv[0]); return argv[0];
}

static JSValueRef jst_error_message JSTDeclareFunction () {
	if (argc == 1) return JSTValueFromUTF8(strerror(JSTValueToDouble(argv[0])));
	return JSTValueUndefined;
}

void * jst_object_class = NULL;
char * jst_object_class_value = "jse://sys/object/value";
char * jst_object_class_interface = "jse://sys/object/interface";

static JSTDeclareDeleteProperty(jst_object_class_delete) {
	JSTObject interface = (JSTObject) JSTObjectGetPrivate(object);
	char *ss = "delete"; if (JSTObjectHasProperty(interface, ss)) return false;
	JSTObject data = (JSTObject) JSTObjectGetProperty(interface, jst_object_class_value);
	JSTObject delete = (JSTObject) JSTObjectGetProperty(interface, ss);
	JSTObjectDeleteProperty(interface, ss);
	JSTValue result = JSTFunctionCall(delete, object, data, JSTStringToValue(propertyName, false));
	JSTObjectSetProperty(interface, ss, delete, 0);
	return JSTValueToBoolean(result);
}

static JSTDeclareGetProperty(jst_object_class_get) {
	JSTObject interface = (JSTObject) JSTObjectGetPrivate(object);
	char *gg = "get"; if (! JSTObjectHasProperty(interface, gg)) return NULL;
	JSTObject data = (JSTObject) JSTObjectGetProperty(interface, jst_object_class_value);
	JSTObject getter = (JSTObject) JSTObjectGetProperty(interface, gg);
	JSTObjectDeleteProperty(interface, gg);
	JSTValue result = JSTFunctionCall(getter, object, data, JSTStringToValue(propertyName, false));
	JSTObjectSetProperty(interface, gg, getter, 0);
	return JSTValueIsNull(result) ? NULL : result;
}

static JSTDeclareSetProperty(jst_object_class_set) {
	JSTObject interface = (JSTObject) JSTObjectGetPrivate(object);
	char *ss = "set"; if (! JSTObjectHasProperty(interface, ss)) return false;
	JSTObject data = (JSTObject) JSTObjectGetProperty(interface, jst_object_class_value);
	JSTObject setter = (JSTObject) JSTObjectGetProperty(interface, ss);
	JSTObjectDeleteProperty(interface, ss);
	JSTValue result = JSTFunctionCall(setter, object, data, JSTStringToValue(propertyName, false), value);
	JSTObjectSetProperty(interface, ss, setter, 0);
	return JSTValueToBoolean(result);
}

static JSTDeclareGetPropertyNames(jst_object_class_enumerate) {
	JSStringRef name; size_t i, length; void * exception = NULL;
	JSTObject keys = (JSTObject) JSTScriptNativeEval("Object.keys(this)", (JSTObject)
		JSTObjectGetProperty((JSTObject) JSTObjectGetPrivate(object), jst_object_class_value)
	);
	length = JSTValueToDouble(JSTObjectGetProperty(keys, "length"));
	for (i = 0; i < length; i++) { JSPropertyNameAccumulatorAddName(
		propertyNames, (name = JSTValueToString(JSTObjectGetPropertyAtIndex(keys, i)))
		);  JSTStringRelease(name);
	}
}

static JSTValue jst_object_exec JSTDeclareFunction(JSTObject prototype, JSTObject interface, JSTObject data) {
	JSTObject interface = (JSTObject) JSTObjectGetPrivate(function);
	return JSObjectCallAsFunction(ctx,
		(JSTObject) JSTObjectGetProperty(interface, "exec"),
		(JSTObject) JSTObjectGetProperty(interface, jst_object_class_value),
		argc, argv, exception
	);
}

static JSTDeclareConstructor(jst_object_new) {
	char *ii = "instance", *pp = "prototype"; JSTObject this,
	interface = JSTObjectGetPrivate(constructor),
	new = (JSTObject) JSTObjectGetProperty(interface, "new");
	if (JSTObjectHasProperty(constructor, ii))
		this = JSTClassInstance(NULL, NULL),
		JSTObjectSetPrototype(this, JSTObjectGetProperty(interface, ii));
	else if (JSTObjectHasProperty(constructor, pp))
		this = JSTClassInstance(NULL, NULL),
		JSTObjectSetPrototype(this, JSTObjectGetProperty(constructor, pp));
	else this = (JSTObject) JSTObjectGetProperty(interface, jst_object_class_value);
	return (JSTObject) JSObjectCallAsFunction(ctx, new, this, argumentCount, arguments, exception);
}

static JSTDeclareHasInstance(jst_object_is_product) {
	JSTObject interface = (JSTObject) JSTObjectGetPrivate(constructor);
	if (! JSTObjectHasProperty(interface, "product")) return false;
	return JSTValueToBoolean(JSTFunctionCall(
		(JSTObject) JSTObjectGetProperty(interface, "product"), constructor,
		possibleInstance
	));
}

static JSTDeclareHasProperty(jst_object_query) {
	void *exception = NULL;
	return JSObjectHasProperty(ctx, (JSTObject)
		(JSTObject) JSTObjectGetProperty(JSTObjectGetPrivate(object), jst_object_class_value),
		propertyName
	);
}

static JSTValue jst_object_create JSTDeclareFunction(JSTObject prototype, JSTObject interface, JSTObject data) {

	JSTObject class, interface = (JSTObject) JSTScriptNativeEval(
		"Object(this)", (JSTObject) argv[0]
	), prototype, value;

	void * _object_class = jst_object_class;

	char *v = "value", *ii = "init", *nn = "name",
	*cc = "class", *ccc = "constructor", *fc = "function";

	bool exec = JSTObjectHasProperty(interface, "exec"),
	construct = JSTObjectHasProperty(interface, "new"),
	hasName = JSTObjectHasProperty(interface, nn);

	char *classType = JSTCND(
		exec || construct, JSTCND(construct, ccc, fc), cc
	), *nameOf;

	if (hasName) {
		char *className = JSTValueToUTF8(JSTObjectGetProperty(interface, nn));
		nameOf = JSTConstructUTF8("%s %s", classType, className);
		free(className);
	} else {
		nameOf = JSTConstructUTF8("%s", classType);
	}

	if (JSTObjectHasProperty(interface, "prototype"))
	prototype = (JSTObject) JSTScriptNativeEval("Object(this.prototype)", interface);
	else prototype = JSTClassInstance(NULL, NULL);
	if (JSTObjectHasProperty(interface, v))
	value = (JSTObject) JSTScriptNativeEval("Object(this)", (JSTObject) JSTObjectGetProperty(interface, v));
	else value = JSTClassInstance(NULL, NULL);

	JSTObjectSetProperty(interface, jst_object_class_value, value, 0);
		
	if (hasName || exec || construct){
		JSTClassDefinition jsClass = JSTClassEmptyDefinition;
		jsClass.attributes = JSTClassPropertyManualPrototype,
		jsClass.className = nameOf,
		jsClass.setProperty = &jst_object_class_set,
		jsClass.getProperty = &jst_object_class_get,
		jsClass.deleteProperty = &jst_object_class_delete,
		jsClass.hasProperty = &jst_object_query,
		jsClass.hasInstance = &jst_object_is_product,
		jsClass.getPropertyNames = &jst_object_class_enumerate;
		if (exec) jsClass.callAsFunction = &jst_object_exec;
		if (construct) jsClass.callAsConstructor = &jst_object_new;
		_object_class = JSClassCreate(&jsClass);
	}

	class = JSTClassInstance(_object_class, interface); free(nameOf);
	JSTObjectSetProperty(prototype, jst_object_class_interface, interface, JSTObjectPropertyHidden);
	JSTObjectSetPrototype(class, prototype);

	if (JSTObjectHasProperty(interface, ii)) {
		JSTObject _init = (JSTObject) JSTObjectGetProperty(interface, ii);
		JSTFunctionCall(_init, class, value, prototype);
	}

	return (JSTValue) class;

}

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]) {

	JSTObject sys, object, engine, io, error;

	sys = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(global, "sys", sys, 0);

	object = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "object", object, 0);

	{
		char * name = "class";
		JSTClassDefinition jsClass = JSTClassEmptyDefinition;
		jsClass.attributes = JSTClassPropertyManualPrototype, jsClass.className = name;
		jsClass.setProperty = &jst_object_class_set,
		jsClass.getProperty = &jst_object_class_get,
		jsClass.deleteProperty = &jst_object_class_delete,
		jsClass.hasProperty = &jst_object_query,
		jsClass.hasInstance = &jst_object_is_product,
		jsClass.getPropertyNames = &jst_object_class_enumerate;
		jst_object_class = JSClassRetain(JSClassCreate(&jsClass));

	}

	JSTObjectSetMethod(object, "create", jst_object_create, 0);

	JSTObjectSetProperty(sys, "main", JSTValueFromUTF8(argv[1]), JSTObjectPropertyReadOnly);
	JSTObjectSetProperty(sys, "date", JSTScriptNativeEval("Object.freeze(new Date())", global), JSTObjectPropertyReadOnly);

	JSTObjectSetProperty(sys, "global", global, 0);
	JSTObjectSetProperty(sys, "context", JSTValueFromPointer(ctx), 0);
	JSTObjectSetProperty(sys, "exception", JSTValueFromPointer(exception), 0);

	{	char buffer[PATH_MAX]; getcwd(buffer, PATH_MAX);
		JSTObjectSetProperty(sys, "path", JSTValueFromUTF8(buffer), JSTObjectPropertyReadOnly);
	}

	JSTObjectSetProperty(sys, "argc", JSTValueFromDouble(argc), 0);
	JSTObjectSetProperty(sys, "argv", JSTValueFromPointer(argv), 0);
	JSTObjectSetProperty(sys, "envp", JSTValueFromPointer(envp), 0);
	
	JSTObjectSetMethod(sys, "_io_path", jst_io_path, 0);
	JSTObjectSetMethod(sys, "_io_stream_print", jst_io_stream_print, 0);
	JSTObjectSetMethod(sys, "_io_stream_pointer", jst_io_stream_pointer, 0);

	JSTObjectSetMethod(sys, "_error_number", jst_error_number, 0);
	JSTObjectSetMethod(sys, "_error_message", jst_error_message, 0);

	engine = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "engine", engine, 0);

	JSTObjectSetProperty(engine, "copyright", JSTValueFromUTF8(JSTReservedAddress), JSTObjectPropertyReadOnly);
	JSTObjectSetProperty(engine, "vendor", JSTValueFromUTF8(VENDOR), JSTObjectPropertyReadOnly);
	JSTObjectSetProperty(engine, "version", JSTValueFromUTF8(VERSION), JSTObjectPropertyReadOnly);
	JSTObjectSetProperty(engine, "codename", JSTValueFromUTF8(CODENAME), JSTObjectPropertyReadOnly);
	JSTObjectSetProperty(engine, "path", JSTValueFromUTF8(argv[0]), JSTObjectPropertyReadOnly);

	io = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "io", io, 0);

	error = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "error", error, 0);

	JSTScriptEval(JSTInitScript, global, "jse.init.js", 1);
	if (JSTScriptHasError) JSTScriptReportException(), exit(1);

	JSTNativeInit(global);

/*	static bool initialized;*/
/*	char buffer[PATH_MAX];*/

/*	if (! initialized )	initialized++;*/

/*	JSTObject js = JSTValueToObject();*/
/*	if (JSTScriptHasError) JSTScriptReportException(), exit(1);*/
/*	*/
/*	JSTObjectSetProperty(global, "js", js, JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);*/

/*	JSTObjectSetMethod(js, "source", jsToolsSource, JSTObjectPropertyReadOnly);*/
/*	JSTObjectSetMethod(js, "exec", jsExecute, JSTObjectPropertyReadOnly);*/
/*	JSTObjectSetProperty(js, "user", jsToolsPasswdToObject(ctx, getuid(), exception), JSTObjectPropertyReadOnly);*/

/*	JSObjectRef env = JSTClassInstance(NULL, NULL); JSTObjectSetProperty(js, "env", env, 0);*/

/*	JSTObjectSetMethod(env, "read", jsToolsEnvRead, 0);*/
/*	JSTObjectSetMethod(env, "write", jsToolsEnvWrite, 0);*/
/*	JSTObjectSetMethod(env, "keys", jsToolsEnvKeys, 0);*/
/*	JSTObjectSetMethod(env, "delete", jsToolsEnvDelete, 0);*/
/*	JSTObjectSetMethod(env, "cwd", jsToolsEnvCWD, 0);*/
/*	JSTObjectSetMethod(env, "chdir", jsToolsEnvChDir, 0);*/
/*	JSTObjectSetMethod(env, "user", jsToolsEnvUser, 0);*/

/*	JSTObject jsRun = JSTClassInstance(NULL, NULL);*/
/*	JSTObjectSetProperty(js, "run", jsRun, JSTObjectPropertyRequired);*/
/*	JSTObjectSetProperty(jsRun, "argc", JSTValueFromDouble(argc), 0);*/
/*	JSTObjectSetProperty(jsRun, "argv", JSTValueFromPointer(argv), 0);*/
/*	JSTObjectSetProperty(jsRun, "envp", JSTValueFromPointer(envp), 0);*/
/*	JSTObjectSetProperty(jsRun, "uid", JSTValueFromDouble(getuid()), 0);*/
/*	JSTObjectSetProperty(jsRun, "euid", JSTValueFromDouble(geteuid()), 0);*/
/*	JSTObjectSetProperty(jsRun, "gid", JSTValueFromDouble(getgid()), 0);*/
/*	JSTObjectSetProperty(jsRun, "pid", JSTValueFromDouble(getpid()), 0);*/
/*	JSTObjectSetProperty(jsRun, "path", JSTValueFromString(JSTStringFromUTF8(getcwd(buffer, PATH_MAX)),true), 0);*/
/*	JSTObjectSetProperty(jsRun, "date", JSTScriptNativeEval("Object.freeze(new Date())", global), 0);*/
/*	JSTScriptNativeEval("Object.freeze(js.run)", global);*/

	return global;

}

JSTValue JSTValueFromJSON_ JSTUtility(char * p) {
	JSTValue result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSValueMakeFromJSONString, s);
		JSTStringRelease(s);
	}
	return result;
}

JSTObject JSTFunctionCallback_ JSTUtility(char * p, void * f) {
	JSTObject result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSObjectMakeFunctionWithCallback, s, f);
		JSTStringRelease(s);
	}
	return result;
}

JSTValue JSTFunctionCall_(register JSTContext ctx, JSTValue *exception, JSTObject method, JSTObject object, ...) {
	va_list ap; register size_t argc = 0, count = 0; va_start(ap, object); 
	while(va_arg(ap, void*) != JSTReservedAddress) argc++; va_start(ap, object);
	if (argc > 32) return JSTValueUndefined; JSTValue argv[argc+1];
	while (count < argc) argv[count++] = va_arg(ap, JSTValue);
	argv[count] = NULL; return JST(JSObjectCallAsFunction, method, object, argc, argv);
}

static JSValueRef jsExecute JSTDeclareFunction () {

	int child_status = 0, allocated = 0, deallocated = 0;
	gchar *exec_child_out = NULL, *exec_child_err = NULL; gint exec_child_status = 0;

	JSObjectRef exec = JSTObjectUndefined;

	bool captureOut = false, captureError = false, captureTime = false;

	captureTime = JSTObjectHasProperty(this, "time");
	captureOut = JSTObjectHasProperty(this, "output");
	captureError = JSTObjectHasProperty(this, "error");

	JSObjectRef oargv = (JSTObject) JSTObjectGetProperty(this, "argv");
	int oargc = JSTValueToDouble(JSTObjectGetProperty(oargv, "length"));

	if (JSTScriptHasError) { return JSTValueUndefined; }

	char * argument[oargc + argc + 1];
	char ** dest = argument;

	while (allocated < oargc) {
		JSTString tmp = JSTValueToString(JSTObjectGetPropertyAtIndex(oargv, allocated));
		argument[allocated] = JSTStringToUTF8(tmp, true);
		allocated++;
	}

	dest = argument + oargc; int secondary=0;

	while (secondary < argc) {
		if (JSTValueIsNull(argv[secondary])) return JSTScriptReportException();
		JSTString tmp = JSTValueToString(argv[secondary]);
		dest[secondary] = JSTStringToUTF8(tmp, true);
		secondary++;
	}

	dest[secondary] = NULL; allocated += secondary;
	if (JSTScriptHasError) { goto leaving; }

	exec = JSTClassInstance(NULL, NULL);
	if (captureTime) JSTScriptNativeEval("this.startTime = Object.freeze(new Date())", exec);
	if (g_spawn_sync(NULL, argument, NULL, G_SPAWN_LEAVE_DESCRIPTORS_OPEN | G_SPAWN_SEARCH_PATH | G_SPAWN_CHILD_INHERITS_STDIN, NULL, NULL, (captureOut) ? &exec_child_out : NULL, (captureError) ? &exec_child_err : NULL, &exec_child_status, NULL)) {
		exec_child_status = WEXITSTATUS(exec_child_status);
		bool success = (exec_child_status == 0);

		if (success) {
			JSTObjectSetProperty(exec, "status", JSTValueFromBoolean(success), 0);
		} else {
			JSTObjectSetProperty(exec, "status", JSTValueFromDouble(exec_child_status), 0);
		}

		if (captureOut && exec_child_out) {
			JSTObjectSetProperty(exec, "stdout", JSTValueFromString(JSTStringFromUTF8(exec_child_out), 0), true);
			JSTStringFreeUTF8(exec_child_out);
		}
		if (captureError && exec_child_err) {
			JSTObjectSetProperty(exec, "stderr", JSTValueFromString(JSTStringFromUTF8(exec_child_err), 0), true);
			JSTStringFreeUTF8(exec_child_err);
		} 
	}
	if (captureTime) JSTScriptNativeEval("this.endTime = Object.freeze(new Date())", exec);

leaving:
	while (deallocated < allocated) JSTStringFreeUTF8(argument[deallocated++]);

	JSTObjectSetPrototype(exec, JSTScriptNativeEval("js.exec.prototype", NULL));

	return (JSValueRef) exec;
}

