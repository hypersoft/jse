#include "JSTools.h"

bool ioFileExists(char * file) {
	return access( file, F_OK | R_OK ) != -1;
}

char * ioFileReadAllText(char * file) {
	char * src = NULL;
	if (g_file_get_contents(file, &src, NULL, NULL))
		return src;
	else return NULL;
}

int main(int argc, char *argv[], char *envp[]) {

	JSTContext ctx = JSTContextCreateInGroup(JSTContextGroupCreate(), NULL);
	JSTObject global = JSTContextGetGlobalObject(ctx);
	JSTObject e = NULL; JSTValue *exception = (void*)&e;

	JSTInit(global, argc, argv, envp);

	if (ioFileExists(argv[1])) {
		char * script, * src = ioFileReadAllText(argv[1]); script = src;
		int c = 0;
		if (*script == '#' && *(script+1) == '!') {
			script+=2;
			while ((c = *script) && c != '\n') script++;
		}
		JSTScriptEval(script, NULL, argv[1], 1);
	} else { JSTScriptNativeError("unable to read user script: `%s'", argv[1]); }

	if (e) JSTScriptReportException();

}

