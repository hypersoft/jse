const char * JSTReservedAddress =

	"Hypersoft Systems JSE Copyright 2013 Triston J. Taylor, All Rights Reserved.";

	const char * CODENAME = JSE_CODENAME;
	const char * VERSION = JSE_BUILDNO;
	const char * VENDOR = JSE_VENDOR;

#define jse_type_const 1
#define jse_type_signed 2
#define jse_type_int 4
#define jse_type_struct 8
#define jse_type_union 16
#define jse_type_utf 32
#define jse_type_void 64
#define jse_type_bool 128
#define jse_type_char 256
#define jse_type_short 512
#define jse_type_long 1024
#define jse_type_size 2048
#define jse_type_pointer 4096
#define jse_type_int64 8192
#define jse_type_float 16384
#define jse_type_double 32768
#define jse_type_value 65536
#define jse_type_string 131072

#include "JSTools.inc"
#include "JSTInit.inc"

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]);
JSTObject JSTNativeInit_ JSTUtility(JSTObject js);
JSTValue JSTFunctionCall_(register JSTContext ctx, JSTValue *exception, JSTObject method, JSTObject object, ...);
JSTObject JSTFunctionCallback_ JSTUtility(char * p, void * f);
JSTValue JSTValueFromJSON_ JSTUtility(char * p);
static JSValueRef jst_sys_execute JSTDeclareFunction ();
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

static JSTValue jsToolsEnvUser JSTDeclareFunction () {
	if (JSTValueIsNumber(argv[0])) return (JSTValue) jsToolsPasswdToObject(ctx, JSTValueToDouble(argv[0]), exception);
	return JSTValueUndefined;
}

static JSValueRef jst_sys_run JSTDeclareFunction (file, [global object]) {
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

static JSValueRef jst_sys_eval JSTDeclareFunction(source, file, line) {
	char * source = JSTValueToUTF8(argv[0]), * file = JSTValueToUTF8(argv[1]);
	JSTValue result = JSTScriptEval(source, this, file, JSTValueToDouble(argv[2]));
	free(file); free(source); return result;
}

static JSValueRef jst_io_flush JSTDeclareFunction(void) { sync(); return NULL;}

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

static JSValueRef jst_io_stream_open JSTDeclareFunction(void) {
	char * file = JSTValueToUTF8(argv[0]), * mode = JSTValueToUTF8(argv[1]);
	JSTValue result = JSTValueFromPointer(fopen(file, mode));
	free(file); free(mode); return result;
}

static JSValueRef jst_io_stream_reopen JSTDeclareFunction(void) {
	char * file = JSTValueToUTF8(argv[0]), * mode = JSTValueToUTF8(argv[1]);
	JSTValue result = JSTValueFromPointer(freopen(file, mode, JSTValueToPointer(argv[2])));
	free(file); free(mode); return result;
}

static JSValueRef jst_io_stream_close JSTDeclareFunction () {
	if (argc > 0) return JSTValueFromDouble(fclose(JSTValueToPointer(argv[0])));
	return JSTValueUndefined;
}

static JSValueRef jst_io_stream_flush JSTDeclareFunction () {
	if (argc > 0) return JSTValueFromDouble(fflush(JSTValueToPointer(argv[0])));
	return JSTValueUndefined;
}

static JSValueRef jst_io_stream_eof JSTDeclareFunction () {
	long result = feof(JSTValueToPointer(argv[0]));
	return JSTValueFromDouble(result);
}

static JSValueRef jst_io_stream_seek JSTDeclareFunction () {
	return JSTValueFromDouble(fseek(JSTValueToPointer(argv[0]), JSTValueToDouble(argv[1]), SEEK_SET));
}

static JSValueRef jst_io_stream_rewind JSTDeclareFunction () {
	if (argc > 0) rewind(JSTValueToPointer(argv[0]));
	return JSTValueUndefined;
}

static JSValueRef jst_io_stream_descriptor JSTDeclareFunction () {
	long result = fileno(JSTValueToPointer(argv[0]));
	return JSTValueFromDouble(result);
}

static JSValueRef jst_io_stream_position JSTDeclareFunction () {
	long result = ftell(JSTValueToPointer(argv[0]));
	return JSTValueFromDouble(result);
}

static JSValueRef jst_io_stream_error JSTDeclareFunction () {
	long result = ferror(JSTValueToPointer(argv[0]));
	return JSTValueFromDouble(result);
}

static JSValueRef jst_io_stream_error_clear JSTDeclareFunction () {
	if (argc > 0) clearerr(JSTValueToPointer(argv[0]));
	return JSTValueUndefined;
}

static JSValueRef jst_io_stream_wide JSTDeclareFunction () {
	long result = fwide(JSTValueToPointer(argv[0]), 0);
	return JSTValueFromDouble(result);
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

static JSValueRef jst_io_stream_read_line JSTDeclareFunction () {
	char *buffer = NULL; ssize_t length = 0;
	ssize_t result = getline(&buffer, &length, JSTValueToPointer(argv[0]));
	if (result == -1) return JSTValueNull;
	JSTValue string = JSTValueFromUTF8(buffer); free(buffer);
	return string;
}

static JSValueRef jst_io_stream_read_field JSTDeclareFunction (FILE *, int char) {
	char *buffer = NULL; ssize_t length = 0;
	ssize_t result = getdelim(&buffer, &length, JSTValueToDouble(argv[1]), JSTValueToPointer(argv[0]));
	if (result == -1) return JSTValueNull;
	JSTValue string = JSTValueFromUTF8(buffer); free(buffer);
	return string;
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
	char *pp = "prototype"; JSTObject this, product,
	interface = JSTObjectGetPrivate(constructor),
	new = (JSTObject) JSTObjectGetProperty(interface, "new");
	if (JSTObjectHasProperty(new, pp)) this = JSTClassInstance(NULL, NULL),
		JSTObjectSetPrototype(this, JSTObjectGetProperty(new, pp));
	else this = (JSTObject) JSTObjectGetProperty(interface, jst_object_class_value);
	product = (JSTObject) JSObjectCallAsFunction(ctx, new, this, argumentCount, arguments, exception);
	return (JSTValueIsVoid(product)) ? this : product;
}

static JSTDeclareHasInstance(jst_object_is_product) {
	JSTObject interface = (JSTObject) JSTObjectGetPrivate(constructor);
	if (! JSTObjectHasProperty(interface, "product")) return false;
	return JSTValueToBoolean(JSTFunctionCall(
		(JSTObject) JSTObjectGetProperty(interface, "product"),
		(JSTObject) JSTObjectGetProperty(interface, "new"),
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
		"Object.create(this)", (JSTObject) argv[0]
	), prototype, value;

	void * _object_class = jst_object_class;

	char *v = "value", *ii = "init", *nn = "name", *in = "new", *pp = "prototype",
	*cc = "class", *ccc = "constructor", *fc = "function";

	bool exec = JSTObjectHasProperty(interface, "exec"),
	construct = JSTObjectHasProperty(interface, in),
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

	if (JSTObjectHasProperty(interface, pp))
	prototype = (JSTObject) JSTScriptNativeEval("Object.create(this.prototype)", interface);
	else prototype = JSTClassInstance(NULL, NULL);
	if (JSTObjectHasProperty(interface, v))
	value = (JSTObject) JSTScriptNativeEval("Object.create(Object(this))", (JSTObject) JSTObjectGetProperty(interface, v));
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
		if (construct) { char *ii = "instance";
			if (JSTObjectHasProperty(interface, ii)){ // link new prototype
				JSTObject new = (JSTObject) JSTObjectGetProperty(interface, in),
				prototype = (JSTObject) JSTObjectGetProperty(interface, ii);
				JSTObjectSetProperty(new, pp, prototype, 0),
				JSTObjectSetProperty(prototype, "constructor", new, 0);
			};  jsClass.callAsConstructor = &jst_object_new;
		}
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

static JSValueRef jst_exit JSTDeclareFunction () {
	if (argc == 1) exit(JSTValueToDouble(argv[0]));
	else exit(0);
	return JSTValueNull;
}

static JSValueRef jst_object_prototype JSTDeclareFunction () {
	if (argc == 1) return JSTObjectGetPrototype(argv[0]);
	else JSTObjectSetPrototype(argv[0], argv[1]);
	return argv[0];
}

extern char *secure_getenv(const char *name);

static JSValueRef jst_env_read JSTDeclareFunction (String) {

	if (argc < 1) {
		JSTScriptSyntaxError("sys.env read: expected argument: string");
		return JSTValueUndefined;
	}

	if (!JSTValueIsString(argv[0])) {
		JSTScriptTypeError("sys.env read: expected string");
	} else {
		char * key = JSTStringToUTF8(JSTValueToString(argv[0]), true);
		char * value = secure_getenv(key); JSTStringFreeUTF8(key);
		return value?JSTValueFromUTF8(value):JSTValueUndefined;
	}

	return JSTValueUndefined;

}

static JSValueRef jst_env_write JSTDeclareFunction (String) {

	if (argc < 2) {
		JSTScriptSyntaxError("sys.env write: expected string arguments: key, value");
		return JSTValueUndefined;
	}

	JSValueRef result = JSTValueUndefined;

	if (!JSTValueIsString(argv[0]) || !JSTValueIsString(argv[1])) {
		JSTScriptTypeError("sys.env write: expected string arguments");
	} else {
		char * key = JSTStringToUTF8(JSTValueToString(argv[0]), true);
		char * value = JSTStringToUTF8(JSTValueToString(argv[1]), true);
		result = JSTValueFromDouble(setenv(key, value, true));
		JSTStringFreeUTF8(key); JSTStringFreeUTF8(value);
	}

	return result;

}

static JSValueRef jst_env_delete JSTDeclareFunction (string) {
	if (argc < 1) {
		JSTScriptSyntaxError("sys.env delete: expected argument: string");
		return JSTValueUndefined;
	}
	JSValueRef result = JSTValueUndefined;
	if (!JSTValueIsString(argv[0])) {
		JSTScriptTypeError("sys.env delete: expected string");
	} else {
		char * key = JSTStringToUTF8(JSTValueToString(argv[0]), true);
		result = JSTValueFromDouble(unsetenv(key));
		JSTStringFreeUTF8(key);
	}
	return result;
}

static JSValueRef jst_env_keys JSTDeclareFunction (string) {
	char **env = JSTValueToPointer(JSTScriptNativeEval("sys.envp", NULL));
	size_t count = 0; while(env[count++]);
	size_t i = 0; char key[PATH_MAX];
	JSObjectRef result = JSTClassInstance(NULL, NULL);
	while(env[i]) {
		sscanf(env[i++], "%[^=]", key);
		JSTObjectSetProperty(result, key, JSTValueUndefined, 0);
	}
	return result;
}

static JSTValue jst_to_utf8 JSTDeclareFunction(Value target) {
	// must have one arg of type String
	return JSTValueFromPointer(JSTValueToUTF8(argv[0]));
}

static JSTValue jst_from_utf8 JSTDeclareFunction(Pointer target) {
	// must have one arg of type char *
	return JSTValueFromUTF8(JSTValueToPointer(argv[0]));
}

static JSTValue jst_to_utf32 JSTDeclareFunction(Value target) {
	JSTString jss = JSTValueToString(argv[0]);
	size_t len = JSTValueToDouble(JSTObjectGetProperty(argv[0], "length"));
	return JSTValueFromPointer(JSTStringToUTF32(jss, len, true));
}

static JSTValue jst_from_utf32 JSTDeclareFunction(Value target) {
	char * utf32 = JSTValueToPointer(argv[0]);
	JSTString source = JSTStringFromUTF32(utf32, -1, false);
	JSTValue result = JSTValueFromString(source, true);
	return result;
}

static JSTValue jst_free JSTDeclareFunction(Pointer target) {
	free(JSTValueToPointer(argv[0]));
	return JSTValueUndefined;
}

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]) {

	JSTObject sys, object, engine, io, stream, read, memory;

	sys = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(global, "sys", sys, 0);

	JSTObjectSetMethod(sys, "eval", jst_sys_eval, 0);
	JSTObjectSetMethod(sys, "run", jst_sys_run, 0);
	JSTObjectSetMethod(sys, "exec", jst_sys_execute, 0);

	JSTObjectSetMethod(sys, "toUTF8", jst_to_utf8, 0);
	JSTObjectSetMethod(sys, "fromUTF8", jst_from_utf8, 0);
	JSTObjectSetMethod(sys, "toUTF32", jst_to_utf32, 0);
	JSTObjectSetMethod(sys, "fromUTF32", jst_from_utf32, 0);

	JSTObjectSetMethod(sys, "_env_read", jst_env_read, 0);
	JSTObjectSetMethod(sys, "_env_write", jst_env_write, 0);
	JSTObjectSetMethod(sys, "_env_delete", jst_env_delete, 0);
	JSTObjectSetMethod(sys, "_env_keys", jst_env_keys, 0);

	object = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "object", object, 0);
	JSTObjectSetMethod(object, "prototype", jst_object_prototype, 0);

	memory = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(sys, "memory", memory, 0);
	JSTObjectSetMethod(memory, "free", jst_free, 0);

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
	JSTObjectSetProperty(sys, "date", JSTScriptNativeEval("Date.now()", global), JSTObjectPropertyReadOnly);
	JSTObjectSetMethod(sys, "exit", jst_exit, 0);

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
	JSTObjectSetMethod(io, "flush", jst_io_flush, 0);

	JSTScriptEval(JSTInitScript, global, "jse.init.js", 1);
	if (JSTScriptHasError) JSTScriptReportException(), exit(1);

	char * script = JSTConstructUTF8(
		"sys.type.width.bool = %i, sys.type.width.char = %i, "
		"sys.type.width.short = %i, sys.type.width.long = %i, "
		"sys.type.width.size = %i, sys.type.width.pointer = %i, "
		"sys.type.width.int64 = %i, sys.type.width.float = %i, "
		"sys.type.width.double = %i, sys.type.width.value = %i, "
		"sys.type.width.string = %i; Object.freeze(sys.type.width);"
		/* other convenient data */
		"sys.byteOrder = %i",
		sizeof(bool), sizeof(char),
		sizeof(short), sizeof(long),
		sizeof(size_t), sizeof(intptr_t),
		sizeof(long long), sizeof(float),
		sizeof(double), sizeof(intptr_t),
		sizeof(intptr_t),
		/* other convenient data */
		G_BYTE_ORDER
	);

	JSTScriptNativeEval(script, global); free(script);
	if (JSTScriptHasError) JSTScriptReportException(), exit(1);

	read = JSTClassInstance(NULL, NULL);
	JSTObjectSetMethod(read, "line", jst_io_stream_read_line, 0);
	JSTObjectSetMethod(read, "field", jst_io_stream_read_field, 0);

	stream = (JSTObject) JSTObjectGetProperty(io, "stream");
	JSTObjectSetProperty(stream, "read", read, 0);

	JSTObjectSetMethod(stream, "open", jst_io_stream_open, 0);
	JSTObjectSetMethod(stream, "reopen", jst_io_stream_reopen, 0);
	JSTObjectSetMethod(stream, "close", jst_io_stream_close, 0);
	JSTObjectSetMethod(stream, "flush", jst_io_stream_flush, 0);
	JSTObjectSetMethod(stream, "rewind", jst_io_stream_rewind, 0);
	JSTObjectSetMethod(stream, "seek", jst_io_stream_seek, 0);
	JSTObjectSetMethod(stream, "position", jst_io_stream_position, 0);
	JSTObjectSetMethod(stream, "eof", jst_io_stream_eof, 0);
	JSTObjectSetMethod(stream, "descriptor", jst_io_stream_descriptor, 0);

	JSTObjectSetMethod(stream, "error", jst_io_stream_error, 0);
	JSTObject error = (JSTObject) JSTObjectGetProperty(stream, "error");
	JSTObjectSetMethod(error, "clear", jst_io_stream_error_clear, 0);

	JSTObjectSetMethod(error, "wide", jst_io_stream_wide, 0);

	JSTObjectSetProperty(global, "read", read, 0);

	JSTNativeInit(global);

/*	static bool initialized;*/
/*	char buffer[PATH_MAX];*/

/*	if (! initialized )	initialized++;*/

/*	JSTObject js = JSTValueToObject();*/
/*	if (JSTScriptHasError) JSTScriptReportException(), exit(1);*/
/*	*/
/*	JSTObjectSetProperty(global, "js", js, JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);*/

/*	JSTObjectSetMethod(js, "source", jsToolsSource, JSTObjectPropertyReadOnly);*/
/*	JSTObjectSetMethod(js, "exec", jst_sys_execute, JSTObjectPropertyReadOnly);*/
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

static JSValueRef jst_sys_execute JSTDeclareFunction () {

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
		JSTObjectSetProperty(exec, "status", JSTValueFromDouble(exec_child_status), 0);
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
	return (JSValueRef) exec;
}

