void * jst_object_class = NULL;
char * jst_object_class_value = "jse://sys/object/value";
char * jst_object_class_interface = "jse://sys/object/interface";

static JSValueRef jst_object_prototype JSTDeclareFunction () {
	if (argc == 1) return JSTObjectGetPrototype(argv[0]);
	else JSTObjectSetPrototype(argv[0], argv[1]);
	return argv[0];
}

static JSTDeclareDeleteProperty(jst_object_class_delete) {
	JSTObject interface = (JSTObject) JSTObjectGetPrivate(object);
	char *ss = "delete"; if (JSTObjectHasProperty(interface, ss)) return false;
	JSTObject data = (JSTObject) JSTObjectGetProperty(interface, jst_object_class_value);
	JSTObject delete = (JSTObject) JSTObjectGetProperty(interface, ss);
	JSTObjectDeleteProperty(interface, ss);
	JSTValue result = JSTFunctionCall(delete, object, data, JSTStringToValue(propertyName, false));
	JSTObjectSetProperty(interface, ss, delete, 0);
	if (JSTValueToBoolean(result)) {
		// remove property existence from custom hash
		char *kk = "keys", * keys = jst_object_class_value;
		if (JSTObjectHasProperty(interface, kk)) {
			JSTObject hash = (JSTObject) JSTObjectGetProperty(interface, keys);
			keys = JSTValueToUTF8(JSTObjectGetProperty(interface, kk));
			hash = (JSTObject) JSTObjectGetProperty(hash, keys);
			JSObjectDeleteProperty(ctx, hash, propertyName, exception);
			free(keys);
		}
		return true;
	} else return false;
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
	if (JSTValueToBoolean(result)) {
		// store property existence in custom hash
		char *kk = "keys", * keys = jst_object_class_value;
		if (JSTObjectHasProperty(interface, kk)) {
			JSTObject hash = (JSTObject) JSTObjectGetProperty(interface, keys);
			keys = JSTValueToUTF8(JSTObjectGetProperty(interface, kk));
			hash = (JSTObject) JSTObjectGetProperty(hash, keys);
			JSObjectSetProperty(ctx, hash, propertyName, JSTValueNull, 0, exception);
			free(keys);
		}
		return true;
	} else return false;
}

static JSTDeclareGetPropertyNames(jst_object_class_enumerate) {
	JSStringRef name; size_t i, length; void * exception = NULL;
	char *kk = "keys", * keys = jst_object_class_value;
	JSTObject interface = JSTObjectGetPrivate(object),
	hash = (JSTObject) JSTObjectGetProperty(interface, keys);
	if (JSTObjectHasProperty(interface, kk)) {
		keys = JSTValueToUTF8(JSTObjectGetProperty(interface, kk));
		hash = (JSTObject) JSTObjectGetProperty(hash, keys);
		free(keys);
	}
	JSTObject list = (JSTObject) JSTScriptNativeEval("Object.keys(this)", hash);
	length = JSTValueToDouble(JSTObjectGetProperty(list, "length"));
	for (i = 0; i < length; i++) { JSPropertyNameAccumulatorAddName(
		propertyNames, (name = JSTValueToString(JSTObjectGetPropertyAtIndex(list, i)))
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
	product = (JSTObject) JSObjectCallAsFunction(ctx, new, this, argc, argv, exception);
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
	// if interface has property "keys" that string value will be used to retrieve
	// the hash table from the class value, otherwise the class value will be used
	// as the hash table.
	// This process is repeated verbatim for the GetPropertyNames callback.
	char *kk = "keys", * keys = jst_object_class_value;
	JSTObject interface = JSTObjectGetPrivate(object),
	hash = (JSTObject) JSTObjectGetProperty(interface, keys);
	if (JSTObjectHasProperty(interface, kk)) {
		keys = JSTValueToUTF8(JSTObjectGetProperty(interface, kk));
		hash = (JSTObject) JSTObjectGetProperty(hash, keys);
		free(keys);
	}
	return JSObjectHasProperty(ctx, hash, propertyName);
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

