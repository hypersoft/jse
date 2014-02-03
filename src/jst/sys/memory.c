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

const char * jst_memory_error_null = "%s: address argument is null";
const char * jst_memory_error_constant = "%s: pointer value is constant";
const char * jst_memory_error_argc = "%s: %s arguments";

const char * jst_memory_error_argv_overflow = "too many";
const char * jst_memory_error_argv_underflow = "insufficient";

const char * jst_memory_write_failed = "unable to write pointer contents";
const char * jst_memory_read_failed = "unable to read pointer contents";

const char * jst_memory_invalid_type = "%s: %i is an invalid type";

static JSTValue jst_memory_allocate JSTDeclareFunction(type, length, erase) {

	// A secondary boolean argument may be specified as true if you want zero 
	// initialized memory

	size_t type = JSTValueToDouble(argv[0]), length, erase = false, size = 0;

	if (! JSTArgumentToInt(1, &length)) return NULL;

	if (argc > 2) erase = JSTValueToBoolean(argv[2]);

	size = JSTCodeTypeWidth(type) * length;
	
	void * address = (erase) ?	g_malloc0(size) : g_malloc(size);
	
	return JSTValueFromPointer(address);

}

static JSTValue jst_memory_free JSTDeclareFunction(address) {

	void * address;
	if (JSTArgumentToPointer(0, &address)) g_free(address);
	return NULL;

}

static JSTValue jst_memory_read JSTDeclareFunction (address, type) {

	void * address = (void*)(unsigned long)JSTValueToDouble(argv[0]);
	gint32 type = JSTValueToDouble(argv[1]);


	bool unsign = ((type & jst_type_signed) == 0);

	if (type & jst_type_reference || type & jst_type_value)
		return JSTValueFromPointer(*(intptr_t*)(address));

	if (type & jst_type_integer) {
		if (type & jst_type_1)
			return JSTValueFromDouble((double) (unsign)?*(gint8*)(address):*(guint8*)(address));
		else if (type & jst_type_2)
			return JSTValueFromDouble((double) (unsign)?*(guint16*)(address):*(gint16*)(address));
		else if (type & jst_type_4)
			return JSTValueFromDouble((double) (unsign)?*(guint32*)(address):*(gint32*)(address));
		else if (type & jst_type_8)
			return JSTValueFromDouble((double) (unsign)?*(guint64*)(address):*(gint64*)(address));
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

	void * address; gint32 type;

	if (! JSTArgumentToPointer(0, &address)) return NULL;
	else if (! address ) return JSTScriptNativeError(
		JST_TYPE_ERROR, jst_memory_error_null, jst_memory_write_failed
	); else if (! JSTArgumentToInt(1, &type)) return NULL;

	double value = JSTValueToDouble(argv[2]);

	if (type & jst_type_reference || type & jst_type_value)
		*(intptr_t*)(address) = (intptr_t)value;
	else if (type & jst_type_integer) {
		if (type & jst_type_1) *(gint8*)(address) = (gint8) value;
		else if (type & jst_type_2) *(gint16*)(address) = (gint16) value;
		else if (type & jst_type_4) *(gint32*)(address) = (gint32) value;
		else if (type & jst_type_8) *(gint64*)(address) = (gint64) value;
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
