	
static JSValueRef jst_string_read JSTDeclareFunction (pointer, length) {
	char * result, * source = JSTValueToPointer(argv[0]);
	size_t length = JSTValueToDouble(argv[1]);
	char buffer[length+1]; buffer[length] = 0;
	result = strncpy(buffer, source, length);
	return JSTValueFromUTF8(buffer);
}

// These are implemented in script post init...

/*static JSValueRef jst_string_read_field JSTDeclareFunction (pointer, delimiter) {*/
/*	char*/
/*	* source = JSTValueToPointer(argv[0]),*/
/*	* delimiter = JSTValueToPointer(argv[1]);*/
/*	return JSTScriptNativeError("sys.string.read.field: this operation is currently unsupported");*/
/*}*/

/*static JSValueRef jst_string_read_line JSTDeclareFunction (pointer) {*/
/*	char * source = JSTValueToPointer(argv[0]);*/
/*	return JSTScriptNativeError("sys.string.read.line: this operation is currently unsupported");*/
/*}*/

static JSValueRef jst_string_read_format JSTDeclareFunction (pointer, format, ...) {
	if (argc < 3) return JSTScriptNativeError("sys.io.stream.read.format: insufficient arguments");
	void * callVM  = dcNewCallVM(argc * sizeof(void*));
	dcMode(callVM, 0);
	char * string = JSTValueToPointer(argv[0]);
	char * fmt = JSTValueToUTF8(argv[1]);
	dcArgPointer(callVM, string), dcArgPointer(callVM, fmt);
	size_t i, r; for (i = 2; i < argc; i++) dcArgPointer(callVM, JSTValueToPointer(argv[i]));
	r = dcCallInt(callVM, sscanf); /* kill it */ dcFree(callVM);
	if (r == EOF)
		JSTScriptNativeError("sys.string.read.format: unable to scan format `%s': %s", fmt, strerror(errno));
	free(fmt);
	return JSTValueFromDouble(r);
}

static JSValueRef jst_string_indexOf JSTDeclareFunction (pointer, substring) {
	char
	* result,
	* source = JSTValueToPointer(argv[0]),
	* substr = JSTValueToUTF8(argv[1]);
	result = g_strstr_len(source, -1, substr);
	free(substr);
	return JSTValueFromPointer(result);
}

static JSValueRef jst_string_toUTF8 JSTDeclareFunction (string) {
	return JSTValueFromPointer(JSTValueToUTF8(argv[0]));
}

static JSValueRef jst_string_fromUTF8 JSTDeclareFunction (pointer, boolOptionFree) {
	void * ptr = (argc)?JSTValueToPointer(argv[0]):"";
	JSTValue result = JSTValueFromUTF8(ptr);
	if (argc == 2 && JSTValueToBoolean(argv[1])) free(ptr);
	return result;
}

static JSValueRef jst_string_toUTF16 JSTDeclareFunction (string) {
	return JSTScriptNativeError("sys.string.toUTF16: this operation is currently unsupported");
}

static JSValueRef jst_string_fromUTF16 JSTDeclareFunction (pointer, boolOptionFree) {
	return JSTScriptNativeError("sys.string.toUTF16: this operation is currently unsupported");
}

static JSValueRef jst_string_toUTF32 JSTDeclareFunction (string) {
	JSTString jss = JSTValueToString(argv[0]);
	size_t len = JSTValueToDouble(JSTObjectGetProperty(argv[0], "length"));
	return JSTValueFromPointer(JSTStringToUTF32(jss, len, true));
}

static JSValueRef jst_string_fromUTF32 JSTDeclareFunction (pointer, boolOptionFree) {
	char * utf32 = JSTValueToPointer(argv[0]);
	JSTString source = JSTStringFromUTF32(utf32, -1, (argc > 1)?JSTValueToBoolean(argv[1]):false);
	JSTValue result = JSTValueFromString(source, true);
	return result;
}


