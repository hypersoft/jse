static JSValueRef jst_error_number JSTDeclareFunction () {
	if (argc == 0) return JSTValueFromDouble(errno);
	else errno = JSTValueToDouble(argv[0]); return argv[0];
}

static JSValueRef jst_error_message JSTDeclareFunction () {
	if (argc == 1) return JSTValueFromUTF8(strerror(JSTValueToDouble(argv[0])));
	return JSTValueUndefined;
}

