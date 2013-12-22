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

