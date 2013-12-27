	
static JSValueRef jst_string_read JSTDeclareFunction (pointer, length) {
	return JSTScriptNativeError("sys.string.read: this operation is currently unsupported");
}

static JSValueRef jst_string_read_field JSTDeclareFunction (pointer, delimiter) {
	return JSTScriptNativeError("sys.string.read.field: this operation is currently unsupported");
}

static JSValueRef jst_string_read_line JSTDeclareFunction (pointer, index) {
	return JSTScriptNativeError("sys.string.read.line: this operation is currently unsupported");
}

static JSValueRef jst_string_read_format JSTDeclareFunction (pointer, format, ...) {
	return JSTScriptNativeError("sys.string.read.format: this operation is currently unsupported");
}

static JSValueRef jst_string_indexOf JSTDeclareFunction (pointer, substring) {
	return JSTScriptNativeError("sys.string.indexOf: this operation is currently unsupported");
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

