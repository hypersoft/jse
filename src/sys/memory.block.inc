
void * jst_memory_block_class = NULL;

static JSTDeclareGetProperty(jst_memory_block_get) {

	void * address; size_t index, length, type, width; bool unsign;

	JSTValue property = JSTStringToValue(propertyName, false);
	if (JSTValueToBoolean(JSTScriptNativeEval("isNaN(parseInt(this))", property))) return NULL;
	index = JSTValueToDouble(property);
	length = JSTValueToDouble(JSTObjectGetProperty(object, "length"));

	if (index < 0 || index >= length) {
		JSTScriptNativeError("sys.memory.block: get [%i]: range over flow", index);
		return NULL;
	}

	address = JSTValueToPointer(JSTObjectGetProperty(object, "address"));
	if (address == NULL) {
		JSTScriptNativeError("sys.memory.block: get [%i]: block address is null", index);
		return NULL;
	} else address += index;

	type = JSTValueToDouble(JSTObjectGetProperty(object, "type"));
	unsign = ((type & jse_type_signed) == 0);

	if (type & jse_type_pointer || type & jse_type_string || type & jse_type_value) {
		return JSTValueFromPointer(*(intptr_t*)(address));
	} else if (type & jse_type_bool)
		return JSTValueFromDouble((double) *(bool*)(address));
	else if (type & jse_type_char)
		return JSTValueFromDouble((double) (unsign)?*(char*)(address):*(signed char*)(address));
	else if (type & jse_type_short)
		return JSTValueFromDouble((double) (unsign)?*(unsigned short*)(address):*(short*)(address));
	else if (type & jse_type_int)
		return JSTValueFromDouble((double) (unsign)?*(unsigned int*)(address):*(int*)(address));
	else if (type & jse_type_long)
		return JSTValueFromDouble((double) (unsign)?*(unsigned long*)(address):*(long*)(address));
	else if (type & jse_type_int64)
		return JSTValueFromDouble((double) (unsign)?*(unsigned long long*)(address):*(long long*)(address));
	else if (type & jse_type_size)
		return JSTValueFromDouble((double) (unsign)?*(size_t*)(address):*(ssize_t*)(address));
	else if (type & jse_type_float)
		return JSTValueFromDouble((double) *(float*)(address));
	else if (type & jse_type_double)
		return JSTValueFromDouble((double) *(double*)(address));
	else JSTScriptNativeError("sys.memory.block: get `%i' is an unknown type", type);

	return NULL;

}

static JSTDeclareSetProperty(jst_memory_block_set) {

	void * address; size_t index, length, type, width;

	JSTValue property = JSTStringToValue(propertyName, false);
	if (JSTValueToBoolean(JSTScriptNativeEval("isNaN(parseInt(this))", property))) return false;
	index = JSTValueToDouble(property);
	length = JSTValueToDouble(JSTObjectGetProperty(object, "length"));

	if (index < 0 || index > length) {
		return JSTScriptNativeError("sys.memory.block: set [%i]: range over flow", index);
	}

	address = JSTValueToPointer(JSTObjectGetProperty(object, "address"));
	if (address == NULL) {
		return JSTScriptNativeError("sys.memory.block: set [%i]: block address is null", index);
	} else address += index;

	type = JSTValueToDouble(JSTObjectGetProperty(object, "type"));
	if (type & jse_type_const) {
		return JSTScriptNativeError("sys.memory.block: set: pointer type value is constant");
	}

	double native = JSTValueToDouble(value);

	if (type & jse_type_pointer || type & jse_type_string || type & jse_type_value)
		*(intptr_t*)(address) = (intptr_t) native;
	else if (type & jse_type_bool) *(bool*)(address) = (bool) native; 
	else if (type & jse_type_char) *(char*)(address) = (char) native;
	else if (type & jse_type_short) *(short*)(address) = (short) native;
	else if (type & jse_type_int) *(int*)(address) = (int) native;
	else if (type & jse_type_long) *(long*)(address) = (long) native;
	else if (type & jse_type_int64) *(long long*)(address) = (long long) native;
	else if (type & jse_type_size) *(size_t*)(address) = (size_t) native;
	else if (type & jse_type_float) *(float*)(address) = (float) native;
	else if (type & jse_type_double) *(double*)(address) = native;
	else return JSTScriptNativeError("sys.memory.block: set `%i' is an unknown type", type);

	return value;

}

static JSTDeclareGetPropertyNames(jst_memory_block_enumerate) {
	void * exception = NULL, *name;
	size_t i, length = JSTValueToDouble(JSTObjectGetProperty(object, "length"));
	for (i = 0; i < length; i++) { JSPropertyNameAccumulatorAddName(
		propertyNames, (name = JSTValueToString(JSTValueFromDouble(i)))
		);  JSTStringRelease(name);
	}
}

static JSTDeclareHasProperty(jst_memory_block_query) {
	size_t index, length; void * exception = NULL;
	JSTValue property = JSTStringToValue(propertyName, false);
	if (JSTValueToBoolean(JSTScriptNativeEval("isNaN(parseInt(this))", property))) return false;
	index = JSTValueToDouble(property);
	length = JSTValueToDouble(JSTObjectGetProperty(object, "length"));
	if (index < 0 || index > length) return false;
	return true;
}

static JSTValue jst_memory_block_addressOf JSTDeclareFunction(index) {
	if (argc != 1 || !JSTValueIsNumber(argv[0]))
		return JSTScriptNativeError("sys.memory.block: get offset: expected integer index");
	int width, length, index; void * address;
	width = JSTValueToDouble(JSTObjectGetProperty(function, "width")),
	length = JSTValueToDouble(JSTObjectGetProperty(function, "length")),
	index = JSTValueToDouble(argv[0]);
	if (index >= length || index < 0)
		return JSTScriptNativeError("sys.memory.block: get offset address: index out of range");
	address = JSTValueToPointer(JSTObjectGetProperty(function, "address"));
	return JSTValueFromPointer(address + (width * index));
}

void jst_memory_block_init() {
	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	jsClass.attributes = JSTClassPropertyManualPrototype,
	jsClass.className = "block",
	jsClass.callAsFunction = &jst_memory_block_addressOf,
	jsClass.setProperty = &jst_memory_block_set,
	jsClass.getProperty = &jst_memory_block_get,
	jsClass.hasProperty = &jst_memory_block_query,
	jsClass.getPropertyNames = &jst_memory_block_enumerate;
	jst_memory_block_class = JSClassRetain(JSClassCreate(&jsClass));
}

static JSTValue jst_memory_block JSTDeclareFunction (pointer, length) {

	if (argc != 2) return JSTScriptNativeError("sys.memory.block: insufficient arguments: expected: pointer, length");
	else if (!jst_memory_block_class) jst_memory_block_init();

	JSTObject object = JSTClassInstance(jst_memory_block_class, NULL);
	JSTObjectSetPrototype(object, JSTObjectGetProperty(function, "prototype"));
	JSTObjectSetProperty(object, "pointer", argv[0], JSTObjectPropertyHidden);
	JSTObjectSetProperty(object, "length", argv[1], 0);

	return (JSTValue) object;

}

