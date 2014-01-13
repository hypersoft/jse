/*

Hypersoft Systems JST AND Hypersoft System JSE Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list 
   of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this 
   list of conditions and the following disclaimer in the documentation and/or other 
   materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGE.

*/

#include "jst.h"

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

	void * ctxgroup = (void*)JSTContextGroupCreate();
	JSTContext ctx = JSTContextCreateInGroup(ctxgroup, NULL);
	JSTObject global = JSTContextGetGlobalObject(ctx);
	JSTObject e = NULL; JSTValue *exception = (void*)&e;

	char * appName = g_path_get_basename(argv[0]);
	g_set_application_name(appName);
	g_free(appName);

	JSTInit(global, argc, argv, envp);

	if (ioFileExists(argv[1])) {
		char * script, * src = ioFileReadAllText(argv[1]); script = src; int c = 0;
		if (*script == '#' && *(script+1) == '!') {
			script+=2;
			while ((c = *script) && c != '\n') script++;
		}
		JSTScriptEval(script, NULL, argv[1], 1); g_free(src);
	} else { JSTScriptNativeError(JST_URI_ERROR, "unable to read user script: `%s'", argv[1]); }

	// You've failed young Jedi, Game Over! Finalize methods won't be called!
	if (e) return JSTScriptReportException();
	JSTContextRelease((JSGlobalContextRef)ctx);
	JSTContextGroupRelease(ctxgroup);
	return 0;
}

