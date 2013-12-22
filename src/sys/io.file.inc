
static JSValueRef jst_io_file_open JSTDeclareFunction(path, flags) {

	JSTValue result = JSTValueUndefined; int flags = 0;
	char s, * scan, * path = JSTValueToUTF8(argv[0]), * access = "rw";

	if (argc > 1) access = JSTValueToUTF8(argv[1]);
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	if (argc > 2) mode = JSTValueToDouble(argv[2]);

	for (scan = access; *scan; scan++) { s = *scan;
		if (s == 'r') {
			if (scan[1] == 'w') flags |= O_RDWR, scan++;
			else flags |= O_RDONLY;
			continue;
		} else
		if (s == '&') { flags |= O_CREAT; continue; } else
		if (s == 'w') { flags |= O_WRONLY; continue; } else
		if (s == 'e') { flags |= O_EXCL; continue; } else
		if (s == 't') { flags |= O_NOCTTY; continue; } else
		if (s == '+') { flags |= O_APPEND; continue; } else
		if (s == '-') { flags |= O_TRUNC; continue; } else
		if (s == '!') { flags |= O_NONBLOCK; continue; } else
		if (s == '<') { flags |= O_RSYNC; continue; } else
		if (s == '>') { flags |= O_SYNC; continue; } else
		if (s == '^') { flags |= O_DSYNC; continue; } else {
			JSTScriptNativeError("sys.io.file.open: invalid access mode: %c", s);
			goto quit;
		}
	}

	int fd = (argc > 2)? open(path, flags, mode) : open(path, flags);

	if (fd == -1)
		JSTScriptNativeError("sys.io.file.open: unable to open `%s': %s", path, strerror(errno));
	else
		result = JSTValueFromDouble(fd);

quit:
	free(path); if (argc > 1) free(access);
	return result;

}

static JSValueRef jst_io_file_close JSTDeclareFunction () {
	if (argc > 0) return JSTValueFromDouble(close(JSTValueToDouble(argv[0])));
	return JSTValueUndefined;
}

static JSValueRef jst_io_file_read JSTDeclareFunction () {
	if (argc == 3) {
		int file = JSTValueToDouble(argv[0]);
		void * data = JSTValueToPointer(argv[1]);
		size_t size = JSTValueToDouble(argv[2]);
		return JSTValueFromDouble(read(file, data, size));
	} else if (argc == 2) {
		int file = JSTValueToDouble(argv[0]);
		JSTObject block = (JSTObject) argv[1];
		void * data = JSTValueToPointer(block);
		size_t size = JSTValueToDouble(JSTObjectGetProperty(block, "size"));
		return JSTValueFromDouble(read(file, data, size));
	} else return JSTScriptNativeError(
		"sys.io.file.read: expected 2 or 3 arguments, recieved %i", argc
	);
}

static JSValueRef jst_io_file_write JSTDeclareFunction () {
	if (argc == 3) {
		int file = JSTValueToDouble(argv[0]);
		void * data = JSTValueToPointer(argv[1]);
		size_t size = JSTValueToDouble(argv[2]);
		return JSTValueFromDouble(write(file, data, size));
	} else if (argc == 2) {
		int file = JSTValueToDouble(argv[0]);
		JSTObject block = (JSTObject) argv[1];
		void * data = JSTValueToPointer(block);
		size_t size = JSTValueToDouble(JSTObjectGetProperty(block, "size"));
		return JSTValueFromDouble(write(file, data, size));
	} else return JSTScriptNativeError(
		"sys.io.file.write: expected 2 or 3 arguments, recieved %i", argc
	);
}

// converts fd to stream *
static JSValueRef jst_io_file_stream JSTDeclareFunction(fd, access) {
	JSTValue result = JSTValueUndefined;
	int fd = JSTValueToDouble(argv[0]);
	char * access = JSTValueToUTF8(argv[1]);
	result = JSTValueFromPointer(fdopen(fd, access));
	free(access);
	return result;
}

static JSValueRef jst_io_file_pipe JSTDeclareFunction(void) {
	JSTObject array = JSObjectMakeArray(ctx, 0, NULL, exception); int fd[2];
	if (pipe(fd) == 0) {
		JSTObjectSetPropertyAtIndex(array, 0, JSTValueFromDouble(fd[0]));
		JSTObjectSetPropertyAtIndex(array, 1, JSTValueFromDouble(fd[1]));
		return (JSTValue) array;
	}
	JSTScriptNativeError("sys.io.file.pipe: unable to create pipe: %s", strerror(errno));
	return JSTValueUndefined;
}

static JSValueRef jst_io_file_clone JSTDeclareFunction(fd) {
	JSTValue result = JSTValueUndefined;
	int val, fd = JSTValueToDouble(argv[0]);
	if (val = dup(fd) == -1) {
		JSTScriptNativeError("sys.io.file.clone: unable to clone descriptor %i: %s", fd, strerror(errno));
	} else result = JSTValueFromDouble(val);
	return result;
}

static JSValueRef jst_io_file_redirect JSTDeclareFunction(fd) {
	JSTValue result = JSTValueUndefined;
	int val, fd1 = JSTValueToDouble(argv[0]), fd2 = JSTValueToDouble(argv[1]);
	if (val = dup2(fd1, fd2) == -1) {
		JSTScriptNativeError("sys.io.file.redirect: unable to redirect file descriptor %i to %i: %s", fd2, fd1, strerror(errno));
	} else result = JSTValueFromDouble(val);
	return result;
}

static JSValueRef jst_io_file_mode JSTDeclareFunction(fd) {
	JSTValue result = JSTValueUndefined;
	int val, fd = JSTValueToDouble(argv[0]), mode = JSTValueToDouble(argv[1]);
	if (val = fchmod(fd, mode) == -1) {
		JSTScriptNativeError("sys.io.file.mode: unable to change file descriptor %i mode to %i: %s", fd, mode, strerror(errno));
	} else result = JSTValueFromDouble(val);
	return result;
}

