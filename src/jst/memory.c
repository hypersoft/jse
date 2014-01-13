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

const char * jst_memory_error_null = "%s: address argument is null";
const char * jst_memory_error_constant = "%s: pointer value is constant";
const char * jst_memory_error_argc = "%s: %s arguments";

const char * jst_memory_error_argv_overflow = "too many";
const char * jst_memory_error_argv_underflow = "insufficient";

const char * jst_memory_write_failed = "unable to write pointer contents";
const char * jst_memory_read_failed = "unable to read pointer contents";

const char * jst_memory_invalid_type = "%s: %i is an invalid type";

static JSTValue jst_memory_allocate JSTDeclareFunction(bytes) {

	// a secondary boolean argument may be specified as true if you want zero initialized memory

	size_t bytes;

	if (! JSTArgumentToInt(0, &bytes)) return NULL;

	void * address = (argc > 1 && JSTValueToBoolean(argv[1])) ?
		g_malloc0(bytes) : g_malloc(bytes)
	;

	return JSTValueFromPointer(address);

}

static JSTValue jst_memory_free JSTDeclareFunction(address) {

	void * address;
	if (JSTArgumentToPointer(0, &address)) g_free(address);
	return NULL;

}

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

	if (!JSTValueToPointer(argv[0], &address)) return NULL;
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
		int8_t * clear = address;
		for (index = 0; index < units; index++) clear[index] = 0;
	} else if (width == 2) {
		int16_t * clear = address;
		for (index = 0; index < units; index++) clear[index] = 0;
	} else if (width == 4) {
		int32_t * clear = address;
		for (index = 0; index < units; index++) clear[index] = 0L;
	} else if (width == 8) {
		int64_t * clear = address;
		for (index = 0; index < units; index++) clear[index] = 0LL;
	} else return JSTScriptNativeError(JST_TYPE_ERROR,
		"unable to clear memory: expected width of 1, 2, 4, or 8: found %i", width
	);

}

static JSTValue jst_memory_read JSTDeclareFunction (address, type) {

	void * address; int32_t type;

	if (!JSTArgumentToPointer(0, &address) || !JSTArgumentToInt(1, &type)) return NULL;
	else if (!address) return JSTScriptNativeError(
		JST_TYPE_ERROR, jst_memory_error_null, jst_memory_read_failed
	);

	bool unsign = ((type & jst_type_signed) == 0);

	if (type & jst_type_reference || type & jst_type_string || type & jst_type_value)
		return JSTValueFromPointer(*(intptr_t*)(address));

	if (type & jst_type_integer) {
		if (type & jst_type_1)
			return JSTValueFromDouble((double) (unsign)?*(int8_t*)(address):*(uint8_t*)(address));
		else if (type & jst_type_2)
			return JSTValueFromDouble((double) (unsign)?*(uint16_t*)(address):*(int16_t*)(address));
		else if (type & jst_type_4)
			return JSTValueFromDouble((double) (unsign)?*(uint32_t*)(address):*(int32_t*)(address));
		else if (type & jst_type_8)
			return JSTValueFromDouble((double) (unsign)?*(uint64_t*)(address):*(int64_t*)(address));
	} else {
		if (type & jst_type_1)
			return JSTValueFromDouble((double) *(bool*)(address));
		else if (type & jst_type_4)
			return JSTValueFromDouble((double) *(float*)(address));
		else if (type & jst_type_8)
			return JSTValueFromDouble((double) *(double*)(address));
	}

	return JSTScriptNativeError(
		JST_TYPE_ERROR, jst_memory_invalid_type, jst_memory_read_failed, type
	);

}

static JSTValue jst_memory_write JSTDeclareFunction (address, type, value) {

	void * address; int32_t type;

	if (! JSTArgumentToPointer(0, &address)) return NULL;
	else if (! address ) return JSTScriptNativeError(
		JST_TYPE_ERROR, jst_memory_error_null, jst_memory_write_failed
	); else if (! JSTArgumentToInt(1, &type)) return NULL;
	else if (type & jst_type_constant) return JSTScriptNativeError(
		JST_TYPE_ERROR, jst_memory_error_constant, jst_memory_write_failed
	);

	double value = JSTValueToDouble(argv[2]);

	if (type & jst_type_reference || type & jst_type_string || type & jst_type_value)
		*(intptr_t*)(address) = (intptr_t)value;
	else if (type & jst_type_integer) {
		if (type & jst_type_1) *(int8_t*)(address) = (int8_t) value;
		else if (type & jst_type_2) *(int16_t*)(address) = (int16_t) value;
		else if (type & jst_type_4) *(int32_t*)(address) = (int32_t) value;
		else if (type & jst_type_8) *(int64_t*)(address) = (int64_t) value;
		else goto fail;
	} else {
		if (type & jst_type_1) *(bool*)(address) = (bool) value; 
		else if (type & jst_type_4) *(float*)(address) = (float) value;
		else if (type & jst_type_8) *(double*)(address) = value;
		else goto fail;
	}

	return argv[2];

fail:
	return JSTScriptNativeError(
		JST_TYPE_ERROR, jst_memory_invalid_type, jst_memory_write_failed, type
	);
}

