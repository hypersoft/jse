//extern char *secure_getenv(const char *name);

void * jst_envp_class = NULL;
char ** jst_envp_default = NULL;

static char * jst_envp_key_empty = "environment key is empty";
static char * jst_envp_key_illegal = "illegal character '=' detected in environment key";

static char * jst_envp_validate(char * key) {
	char * seek;
	if (!*key) return jst_envp_key_empty;
	for (seek = key; *seek; seek++) if (*seek == '=') {
		return jst_envp_key_illegal;
	}
	return NULL;
}

static JSTDeclareSetProperty(jst_envp_set) {

	char ** envp = JSTObjectGetPrivate(object);
	char * key = JSTStringToUTF8(propertyName, false);
	char * error = jst_envp_validate(key);
	bool result = false;

	if (error) JSTScriptSetError(JSTScriptSyntaxError(error));
	else {
		char * data = JSTValueToUTF8(value);
		if (jst_envp_default == envp) result = g_setenv(key, data, true);
		else {
			char ** envp2 = g_environ_setenv(envp, key, data, true);
			if (envp2 != envp) JSTObjectSetPrivate(object, envp2);
			result = true;
		}
	}

	free(key);

	return result;

}

static JSTDeclareGetProperty(jst_envp_get) {

	JSTValue result = JSTValueUndefined;

	char ** envp = JSTObjectGetPrivate(object);
	char * key = JSTStringToUTF8(propertyName, false);
	char * error = jst_envp_validate(key);
	const char * value = NULL;

	if (error) JSTScriptSetError(JSTScriptSyntaxError(error));
	else {
		if (jst_envp_default == envp) value = g_getenv(key);
		else value = g_environ_getenv(envp, key);
		if (value) result = JSTValueFromUTF8(value);
	}

	free(key);

	return result;

}

static JSTDeclareDeleteProperty(jst_envp_delete) {

	char ** envp = JSTObjectGetPrivate(object);
	char * key = JSTStringToUTF8(propertyName, false);
	char * error = jst_envp_validate(key);
	bool result = false;

	if (error) JSTScriptSetError(JSTScriptSyntaxError(error));
	else {
		if (jst_envp_default == envp) {
			g_unsetenv(key);
		} else {
			char ** envp2 = g_environ_unsetenv(envp, key);
			if (envp2 != envp) JSTObjectSetPrivate(object, envp2);
		}
		result = true;
	}

	free(key);

	return result;

}

static JSTDeclareGetPropertyNames(jst_envp_enumerate) {

	char ** envp = JSTObjectGetPrivate(object);
	size_t count = 0, i, l;
	char key[PATH_MAX], * data;
	JSTString copy;

	while(envp[count++]); count--;

	for (i = 0; i < count; i++) {
		data = envp[i];
		for (l = 0; data[l] && data[l] != '='; l++) key[l] = data[l];
		key[l] = 0, copy = JSTStringFromUTF8(key);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	}

}

static JSTDeclareHasProperty(jst_envp_query) {

	char ** envp = JSTObjectGetPrivate(object);
	size_t count = 0, i;
	char * seek = JSTStringToUTF8(propertyName, false);
	bool result = false;

	char * error = jst_envp_validate(seek);

	if (!error) {
		while(envp[count++]); count--;
		for (i = 0; i < count; i++) {
			if (g_str_has_prefix(envp[i], seek)) {
				result = true;
				break;
			}
		}
	}

	JSTStringFreeUTF8(seek);
	return result;

}

static JSTDeclareConvertor(jst_envp_convert) {
	if (type == JSTValueTypeNumber) {
		return JSTValueFromPointer(JSTObjectGetPrivate(object));
	}
	return false;
}

static JSTDeclareFinalizer(jst_envp_finalize) {
	char ** envp = JSTObjectGetPrivate(object);
	if (envp != jst_envp_default) {
		g_strfreev(envp);
		JSTObjectSetPrivate(object, NULL);
	}
}

JSTValue jst_envp_free JSTDeclareFunction() {
	char ** envp = JSTObjectGetPrivate(this);
	if (envp) jst_envp_finalize(this);
	return JSTValueUndefined;
}

static JSTDeclareSetProperty(jst_envp_set_pointer) {
	void * ptr = JSTValueToPointer(value);
	JSTObjectSetPrivate(object, ptr);
	return true;
}

static JSTDeclareGetProperty(jst_envp_get_pointer) {
	return JSTValueFromPointer(JSTObjectGetPrivate(object));
}

static JSTDeclareInitializer(jst_envp_initializer) {
	puts("init");
}

JSTClass jst_envp_init() {

	JSTClassFunction functions[] = {
		{"free", &jst_envp_free, JSTObjectPropertyAPI},
		{NULL, NULL, 0}
	};

	JSTClassAccessor properties[] = {
		{"pointer", &jst_envp_get_pointer, &jst_envp_set_pointer, JSTObjectPropertyAPI},
		{NULL, NULL, NULL, 0}
	};

	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	jsClass.className = "envp",
	jsClass.attributes = JSTClassPropertyManualPrototype,
	jsClass.initialize = &jst_envp_initializer,
	jsClass.staticFunctions = functions,
	jsClass.staticValues = properties,
	jsClass.setProperty = &jst_envp_set,
	jsClass.getProperty = &jst_envp_get,
	jsClass.deleteProperty = &jst_envp_delete,
	jsClass.hasProperty = &jst_envp_query,
	jsClass.getPropertyNames = &jst_envp_enumerate,
	jsClass.convertToType = &jst_envp_convert,
	jsClass.finalize = &jst_envp_finalize;
	jst_envp_class = JSClassRetain(JSClassCreate(&jsClass));

	return jst_envp_class;

}

/*
	This webkit constructor business is an utter mess. Trying to emulate the functionality
	of the standard classes requires too much "esoteric" knowledge.

	The easiest way to implement this functionality is to define two functions:

		1. constructor
		2. function

	In your script, you would then define a 3rd function to handle the new instance
	case, as well as the prototype:

	function() {
		if (this instanceof arguments.callee) {
			return Object.setPrototypeOf(constructor.apply(null, arguments), arguments.callee.prototype);
		} else {
			return Object.setPrototypeOf(function.apply(null, arguments), arguments.callee.prototype);
		}
	}

	Object.setPrototypeOf is a JSE extension allowing the setting of an object's prototype.

	Its not pretty, but it works flawlessly compared to trying to implement such
	things as a host object.

*/

static JSTValue jst_envp_constructor JSTDeclareFunction() {

	if (!jst_envp_class) jst_envp_init();

	char ** envp;

	if (argc) {
		if (!JSTValueIsNumber(argv[0])) {
			JSTScriptSetError(JSTScriptTypeError("expected char ** argument"));
			return NULL;
		} else envp = JSTValueToPointer(argv[0]);
	} else envp = g_get_environ();

	JSTObject object = JSTClassInstance(jst_envp_class, envp);

	return object;

}

JSTValue jst_envp_function JSTDeclareFunction() {

	if (!jst_envp_class) jst_envp_init();

	JSTObject object = JSTClassInstance(jst_envp_class, jst_envp_default);

	return object;
}
