#include "JSTools.h"

int main(int argc, char *argv[], char *envp[]) {

	JSTContext ctx = JSTContextCreateInGroup(JSTContextGroupCreate(), NULL);
	JSTObject global = JSTContextGetGlobalObject(ctx);
	JSTValue e = NULL; JSTValue *exception = &e;

	JSTInit(global, argc, argv, envp);

	// print out our pid:
	printf("%s\n", JSTStringToUTF8(JSTValueToString(JSTScriptNativeEval("js.run.pid", NULL)),true));

}


