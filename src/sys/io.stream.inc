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

static JSValueRef jst_io_stream_read JSTDeclareFunction () {
	if (argc != 4) return JSTScriptNativeError(
		"sys.io.stream.read: expected 4 arguments, recieved %i", argc
	);
	FILE * stream = JSTValueToPointer(argv[0]); void * data = JSTValueToPointer(argv[1]);
	size_t width = JSTValueToDouble(argv[2]), length = JSTValueToDouble(argv[3]);
	return JSTValueFromDouble(fread(data, width, length, stream));
}

static JSValueRef jst_io_stream_write JSTDeclareFunction () {
	if (argc != 4) return JSTScriptNativeError(
		"sys.io.stream.write: expected 4 arguments, recieved %i", argc
	);
	FILE * stream = JSTValueToPointer(argv[0]); void * data = JSTValueToPointer(argv[1]);
	size_t width = JSTValueToDouble(argv[2]), length = JSTValueToDouble(argv[3]);
	return JSTValueFromDouble(fwrite(data, width, length, stream));
}

static JSValueRef jst_io_stream_buffer JSTDeclareFunction () {
	if (argc != 4) return JSTScriptNativeError(
		"sys.io.stream.read: expected 4 arguments, recieved %i", argc
	);
	FILE * stream = JSTValueToPointer(argv[0]); void * buffer = JSTValueToPointer(argv[1]);
	size_t mode = JSTValueToDouble(argv[2]), length = JSTValueToDouble(argv[3]);
	return JSTValueFromDouble(setvbuf(stream, buffer, mode, length));
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

