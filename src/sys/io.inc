#include "sys/io.stream.inc"
#include "sys/io.file.inc"

static JSValueRef jst_io_flush JSTDeclareFunction(void) { sync(); return JSTValueUndefined;}

static JSValueRef jst_io_path JSTDeclareFunction () {
	if (argc == 0) {
		char buffer[PATH_MAX]; getcwd(buffer, PATH_MAX);
		return JSTValueFromUTF8(buffer);
	}
	char * val; JSValueRef
	result = JSTValueFromDouble(chdir(val = JSTStringToUTF8(JSTStringFromValue(argv[0]), true)));
	JSTStringFreeUTF8(val);
	return result;
}

