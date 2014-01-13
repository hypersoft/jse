/*

Hypersoft Systems JST AND Hypersoft System JSE Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list 
   of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this 
   list of conditions and the following disclaimer in the documentation and/or other 
   materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGE.

*/

void * jst_env_class = NULL;
char ** jst_env_default = NULL;

static char * jst_env_key_empty = "environment key is empty";
static char * jst_env_key_illegal = "illegal character '=' detected in environment key";

static char * jst_env_validate(char * key) {
	char * seek;
	if (!*key) return jst_env_key_empty;
	for (seek = key; *seek; seek++) if (*seek == '=') {
		return jst_env_key_illegal;
	}
	return NULL;
}

static JSTDeclareSetProperty(jst_env_set) {

	char ** envp = JSTObjectGetPrivate(object);
	char * key = JSTStringToUTF8(propertyName, false);
	char * error = jst_env_validate(key);
	bool result = false;

	if (error) JSTScriptSetError(JSTScriptSyntaxError(error));
	else {
		char * data = JSTValueToUTF8(value);
		if (jst_env_default == envp) result = g_setenv(key, data, true);
		else {
			char ** envp2 = g_environ_setenv(envp, key, data, true);
			if (envp2 != envp) JSTObjectSetPrivate(object, envp2);
			result = true;
		}
	}

	free(key);

	return result;

}

static JSTDeclareGetProperty(jst_env_get) {

	JSTValue result = JSTValueUndefined;

	char ** envp = JSTObjectGetPrivate(object);
	char * key = JSTStringToUTF8(propertyName, false);
	char * error = jst_env_validate(key);
	const char * value = NULL;

	if (error) JSTScriptSetError(JSTScriptSyntaxError(error));
	else {
		if (jst_env_default == envp) value = g_getenv(key);
		else value = g_environ_getenv(envp, key);
		if (value) result = JSTValueFromUTF8(value);
	}

	free(key);

	return result;

}

static JSTDeclareDeleteProperty(jst_env_delete) {

	char ** envp = JSTObjectGetPrivate(object);
	char * key = JSTStringToUTF8(propertyName, false);
	char * error = jst_env_validate(key);
	bool result = false;

	if (error) JSTScriptSetError(JSTScriptSyntaxError(error));
	else {
		if (jst_env_default == envp) {
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

static JSTDeclareGetPropertyNames(jst_env_enumerate) {

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

static JSTDeclareHasProperty(jst_env_query) {

	char ** envp = JSTObjectGetPrivate(object);
	size_t count = 0, i;
	char * seek = JSTStringToUTF8(propertyName, false);
	bool result = false;

	char * error = jst_env_validate(seek);

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

static JSTDeclareConvertor(jst_env_convert) {
	if (type == JSTValueTypeNumber) {
		return JSTValueFromPointer(JSTObjectGetPrivate(object));
	}
	return false;
}

static JSTDeclareFinalizer(jst_env_finalize) {
	char ** envp = JSTObjectGetPrivate(object);
	if (envp != jst_env_default) {
		g_strfreev(envp);
		JSTObjectSetPrivate(object, NULL);
	}
}

JSTValue jst_env_free JSTDeclareFunction() {
	char ** envp = JSTObjectGetPrivate(this);
	if (envp) jst_env_finalize(this);
	return JSTValueUndefined;
}

static JSTDeclareSetProperty(jst_env_set_pointer) {
	void * ptr;
	if (JSTValueToPointer(value, &ptr))	JSTObjectSetPrivate(object, ptr);
	else return false;
	return true;
}

static JSTDeclareGetProperty(jst_env_get_pointer) {
	return JSTValueFromPointer(JSTObjectGetPrivate(object));
}

static JSTDeclareInitializer(jst_env_initializer) {

}

static JSTClass jst_env_init(envp) {

	JSTClassFunction functions[] = {
		{"free", &jst_env_free, JSTObjectPropertyAPI},
		{NULL, NULL, 0}
	};

	JSTClassAccessor properties[] = {
		{"pointer", &jst_env_get_pointer, &jst_env_set_pointer, JSTObjectPropertyAPI},
		{NULL, NULL, NULL, 0}
	};

	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	jsClass.className = "env",
	jsClass.attributes = JSTClassPropertyManualPrototype,
	jsClass.initialize = &jst_env_initializer,
	jsClass.staticFunctions = functions,
	jsClass.staticValues = properties,
	jsClass.setProperty = &jst_env_set,
	jsClass.getProperty = &jst_env_get,
	jsClass.deleteProperty = &jst_env_delete,
	jsClass.hasProperty = &jst_env_query,
	jsClass.getPropertyNames = &jst_env_enumerate,
	jsClass.convertToType = &jst_env_convert,
	jsClass.finalize = &jst_env_finalize;
	jst_env_class = JSClassRetain(JSClassCreate(&jsClass));

	return jst_env_class;

}

/*
	This webkit constructor business is an utter mess. Trying to emulate the functionality
	of the standard classes requires too much "esoteric" knowledge.

	The easiest way to implement this functionality is to define two native functions:

		1. constructor
		2. function

	In your script, you would then define a 3rd function to handle the new instance
	case, as well as the prototype:

	sys.env = sys.class('env', sys_env_constructor, sys_env_function, {});

	Its not pretty, but it works flawlessly compared to trying to implement such
	things as a host object.

*/

static JSTValue jst_env_constructor JSTDeclareFunction() {

	if (!jst_env_class) jst_env_init();

	char ** envp;

	if (argc) {
		if (!JSTValueIsNumber(argv[0])) {
			JSTScriptSetError(JSTScriptTypeError("expected char ** argument"));
			return NULL;
		} else if (! JSTValueToPointer(argv[0], &envp)) return NULL;
	} else envp = g_get_environ();

	JSTObject object = JSTClassInstance(jst_env_class, envp);

	return object;

}

JSTValue jst_env_function JSTDeclareFunction() {

	if (!jst_env_class) jst_env_init();

	JSTObject object = JSTClassInstance(jst_env_class, jst_env_default);

	return object;
}
