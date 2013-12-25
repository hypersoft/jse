#include "sys/memory.block.inc"

static JSTValue jst_memory_free JSTDeclareFunction(Pointer target) {
	size_t i; for (i = 0; i < argc; i++) free(JSTValueToPointer(argv[i]));
	return JSTValueUndefined;
}

static JSValueRef jst_memory_allocate JSTDeclareFunction () {
	int width, length;
	if (JSTValueIsNumber(argv[0])) width = JSTValueToDouble(argv[0]);
	else { // TODO: fix this type width retrieval with error correction
		width = JSTValueToDouble(JSTObjectGetProperty((JSTObject)argv[0], "width"));
	}
	length = JSTValueToDouble(argv[1]);
	return JSTValueFromPointer(malloc(width * length));
}

static JSValueRef jst_memory_resize JSTDeclareFunction () {
	void * buf; size_t width, length;
	if (argc == 3) {
		buf = JSTValueToPointer(argv[0]),
		width = JSTValueToDouble(argv[1]),
		length = JSTValueToDouble(argv[2]);
	} else if (argc == 2) {
		JSTObject block = (JSTObject) argv[0];
		buf = JSTValueToPointer(block),
		width = JSTValueToDouble(JSTObjectGetProperty(block, "width"));
		length = JSTValueToDouble(argv[1]);
	} else return JSTScriptNativeError(
		"sys.memory.resize: expected 2 or 3 arguments: found `%i'", argc
	);
	JSTValue result = JSTValueFromPointer(realloc(buf, width * length));
	return result;
}

static JSValueRef jst_memory_clear JSTDeclareFunction () {
	void * buf; size_t width, length;
	if (argc == 3) {
		buf = JSTValueToPointer(argv[0]),
		width = JSTValueToDouble(argv[1]),
		length = JSTValueToDouble(argv[2]);
	} else if (argc == 2) {
		JSTObject block = (JSTObject) argv[0];
		buf = JSTValueToPointer(block),
		width = JSTValueToDouble(JSTObjectGetProperty(block, "width"));
		length = JSTValueToDouble(argv[1]);
	} else if (argc == 1) {
		JSTObject block = (JSTObject) argv[0];
		buf = JSTValueToPointer(block),
		width = JSTValueToDouble(JSTObjectGetProperty(block, "width"));
		length = JSTValueToDouble(JSTObjectGetProperty(block, "length"));
	} else return JSTScriptNativeError(
		"sys.memory.resize: expected 1-3 arguments: found `%i'", argc
	);
	memset(buf, 0, width * length);
	return JSTValueUndefined;
}

static JSValueRef jst_memory_open JSTDeclareFunction () {

	FILE * stream; void * data; size_t size; char * mode;

	if (argc == 4) {
		data = JSTValueToPointer(argv[0]),
		size = JSTValueToDouble(argv[1]) * JSTValueToDouble(argv[2]),
		mode = JSTValueToPointer(argv[3]);
	} else if (argc == 2) {
		JSTObject block = (JSTObject) argv[0];
		data = JSTValueToPointer(block),
		size = JSTValueToDouble(JSTObjectGetProperty(block, "size")),
		mode = JSTValueToPointer(argv[1]);
	} else return JSTScriptNativeError(
		"sys.memory.open: expected 2 or 4 arguments, recieved %i", argc
	);

	stream = fmemopen(data, size, mode);
	if (stream == NULL) {
		return JSTScriptNativeError("sys.memory.open: unable to open memory block: %s", strerror(errno));
	}
	return JSTValueFromPointer(stream);
}

static JSValueRef jst_memory_stream JSTDeclareFunction () {
	if (argc != 2) return JSTScriptNativeError("sys.memory.stream: unable to initialize memory stream: %s arguments", (argc > 2)?"too many":"insufficient");
	char ** buffer = JSTValueToPointer(argv[0]);
	size_t * length = JSTValueToPointer(argv[1]);
	FILE * stream = open_memstream(buffer, length);
	if (stream == NULL) {
		return JSTScriptNativeError("sys.memory.stream: unable to instantiate memory stream: %s", strerror(errno));
	}
	return JSTValueFromPointer(stream);
}

static JSTValue jst_memory_read JSTDeclareFunction (address, type) {

	const char * fname = "sys.memory.read", * job = "unable to read pointer contents";

	if (argc != 2) return JSTScriptNativeError("%s: %s: %s arguments", fname, job, (argc > 2)?"too many":"insufficient");

	void * address = JSTValueToPointer(argv[0]);
	if (! address ) return JSTScriptNativeError("%s: %s: address argument is null", fname, job);

	int type = JSTValueToDouble(argv[1]);
	bool unsign = ((type & jse_type_signed) == 0);

	if (type & jse_type_pointer || type & jse_type_string || type & jse_type_value)
		return JSTValueFromPointer(*(intptr_t*)(address));
	else if (type & jse_type_bool)
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
	else
		return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

}

static JSTValue jst_memory_read_block JSTDeclareFunction (address, type, length) {

	const char * fname = "sys.memory.read.block", * job = "unable to read pointer contents";

	if (argc != 3) return JSTScriptNativeError("%s: %s: %s arguments", fname, job, (argc > 3)?"too many":"insufficient");

	void * address = JSTValueToPointer(argv[0]);
	if (! address ) return JSTScriptNativeError("%s: %s: address argument is null", fname, job);

	int i, type = JSTValueToDouble(argv[1]), length = JSTValueToDouble(argv[2]);
	JSTValue array[length];

	bool unsign = ((type & jse_type_signed) == 0);

	if (type & jse_type_pointer || type & jse_type_string || type & jse_type_value) {
		for (i = 0; i < length; i++) array[i] = JSTValueFromPointer(((intptr_t*)(address))[i]);
	}	else if (type & jse_type_bool) {
		for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((bool*)(address))[i]);
	}	else if (type & jse_type_char) {
		if (unsign) for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((char*)(address))[i]);
		else for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((signed char*)(address))[i]);
	}	else if (type & jse_type_short) {
		if (unsign) for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((unsigned short*)(address))[i]);
		else for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((short*)(address))[i]);
	}	else if (type & jse_type_int) {
		if (unsign) for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((unsigned int*)(address))[i]);
		else for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((int*)(address))[i]);
	}	else if (type & jse_type_long) {
		if (unsign) for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((unsigned long*)(address))[i]);
		else for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((long*)(address))[i]);
	}	else if (type & jse_type_int64) {
		if (unsign) for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((unsigned long long*)(address))[i]);
		else for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((long long*)(address))[i]);
	}	else if (type & jse_type_size) {
		if (unsign) for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((size_t*)(address))[i]);
		else for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((ssize_t*)(address))[i]);
	}	else if (type & jse_type_float) {
		for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((float*)(address))[i]);
	}	else if (type & jse_type_double) {
		for (i = 0; i < length; i++) array[i] = JSTValueFromDouble(((double*)(address))[i]);
	}	else return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

	JSTObject result = JSObjectMakeArray(ctx, length, array, exception);
	JSTObjectSetProperty(result, "address", argv[0], JSTObjectPropertyHidden);
	JSTObjectSetProperty(result, "type", argv[1], JSTObjectPropertyHidden);

	return (JSTValue) result;

}

static JSTValue jst_memory_write JSTDeclareFunction (address, type, value) {

	const char * fname = "sys.memory.write", * job = "unable to write pointer contents";

	if (argc != 3) return JSTScriptNativeError("%s: %s: %s arguments", fname, job, (argc > 2)?"too many":"insufficient");

	void * address = JSTValueToPointer(argv[0]);
	if (! address ) return JSTScriptNativeError("%s: %s: address argument is null", fname, job);

	int type = JSTValueToDouble(argv[1]);

	if (type & jse_type_const) {
		return JSTScriptNativeError("%s: %s: pointer value is constant", fname, job);
	}

	double value = JSTValueToDouble(argv[2]);

	if (type & jse_type_pointer || type & jse_type_string || type & jse_type_value)
		*(intptr_t*)(address) = (intptr_t)value;
	else if (type & jse_type_bool) *(bool*)(address) = (bool) value; 
	else if (type & jse_type_char) *(char*)(address) = (char) value;
	else if (type & jse_type_short) *(short*)(address) = (short) value;
	else if (type & jse_type_int) *(int*)(address) = (int) value;
	else if (type & jse_type_long) *(long*)(address) = (long) value;
	else if (type & jse_type_int64) *(long long*)(address) = (long long) value;
	else if (type & jse_type_size) *(size_t*)(address) = (size_t) value;
	else if (type & jse_type_float) *(float*)(address) = (float) value;
	else if (type & jse_type_double) *(double*)(address) = value;
	else return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

	return argv[2];

}

static JSTValue jst_memory_write_block JSTDeclareFunction (address, type, value) {

	const char * fname = "sys.memory.write", * job = "unable to write pointer contents";

	if (argc != 3) return JSTScriptNativeError("%s: %s: %s arguments", fname, job, (argc > 2)?"too many":"insufficient");

	void * address = JSTValueToPointer(argv[0]);
	if (! address ) return JSTScriptNativeError("%s: %s: address argument is null", fname, job);

	int type = JSTValueToDouble(argv[1]);

	if (type & jse_type_const) {
		return JSTScriptNativeError("%s: %s: pointer value is constant", fname, job);
	}

	JSTObject value = (JSTObject) argv[2];
	size_t i, length = JSTValueToDouble(JSTObjectGetProperty(value, "length"));
	if (type & jse_type_pointer || type & jse_type_string || type & jse_type_value)
		for (i = 0; i < length; i++) ((intptr_t*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i));
	else if (type & jse_type_bool) for (i = 0; i < length; i++) ((bool*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i)); 
	else if (type & jse_type_char) for (i = 0; i < length; i++) ((char*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i));
	else if (type & jse_type_short) for (i = 0; i < length; i++) ((short*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i));
	else if (type & jse_type_int) for (i = 0; i < length; i++) ((int*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i));
	else if (type & jse_type_long) for (i = 0; i < length; i++) ((long*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i));
	else if (type & jse_type_int64) for (i = 0; i < length; i++) ((long long*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i));
	else if (type & jse_type_size) for (i = 0; i < length; i++) ((size_t*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i));
	else if (type & jse_type_float) for (i = 0; i < length; i++) ((float*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i));
	else if (type & jse_type_double) for (i = 0; i < length; i++) ((double*)(address))[i] = JSTValueToDouble(JSTObjectGetPropertyAtIndex(value, i));
	else return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

	return argv[2];

}

