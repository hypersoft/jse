/*

Hypersoft Systems JST AND Hypersoft Systems JSE
Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/




static JSTValue jst_memory_resize JSTDeclareFunction(address, bytes) {

	void * address; size_t bytes;

	return (JSTArgumentToPointer(0, &address) && JSTArgumentToInt(1, &bytes))
	? JSTValueFromPointer(g_realloc(address, bytes))
	: NULL;

}

static JSTValue jst_memory_copy JSTDeclareFunction(dest, source, bytes) {

	void * dest, * source; size_t bytes;

	if (JSTArgumentToPointer(0, &dest) && JSTArgumentToPointer(1, &source) &&
	JSTArgumentToInt(2, &bytes)) { g_memmove(dest, source, bytes);
		return argv[0];
	} else return NULL;

}

static JSTValue jst_memory_new_copy JSTDeclareFunction(address, bytes) {

	void * address; size_t bytes;

	return (JSTArgumentToPointer(0, &address) && JSTArgumentToInt(1, &bytes))
	? JSTValueFromPointer(g_memdup(address, bytes))
	: NULL;

}

static JSTValue jst_memory_clear JSTDeclareFunction(address, width, units) {

	void * address; size_t index, width = 1, units = 0;

	if (!JSTArgumentToPointer(0, &address)) return NULL;
	else if (!address) return JSTScriptNativeError(
		JST_TYPE_ERROR, jst_memory_error_null, "cannot clear memory"
	);

	if (argc == 2) {
		if (! JSTArgumentToInt(1, &units)) return NULL;
	} else if (argc == 3) {
		if (! JSTArgumentToInt(1, &width)) return NULL;
		else if (! JSTArgumentToInt(2, &units)) return NULL;
	} else return JSTScriptNativeError(
		JST_TYPE_ERROR, jst_memory_error_argc, "unable to clear pointer contents",
		jst_memory_error_argv_overflow
	);

	if (width == 1)	{
		gint8 * clear = address;
		for (index = 0; index < units; index++) clear[index] = 0;
	} else if (width == 2) {
		gint16 * clear = address;
		for (index = 0; index < units; index++) clear[index] = 0;
	} else if (width == 4) {
		gint32 * clear = address;
		for (index = 0; index < units; index++) clear[index] = 0L;
	} else if (width == 8) {
		gint64 * clear = address;
		for (index = 0; index < units; index++) clear[index] = 0LL;
	} else return JSTScriptNativeError(JST_TYPE_ERROR,
		"unable to clear memory: expected width of 1, 2, 4, or 8: found %i", width
	);

}

