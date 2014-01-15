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

void * jst_pointer_class = NULL;

typedef struct jst_pointer_data {
	void * value;
	bool allocated;
} jst_pointer_data;

static JSTDeclareConvertor(jst_pointer_convert) {
	if (type == JSTValueTypeNumber) {
		jst_pointer_data * d = JSTObjectGetPrivate(object);
		if (d) return JSTValueFromPointer(JSTObjectGetPrivate(d->value));
		return JSTValueFromDouble(0);
	}
	return false;
}

static JSTDeclareFinalizer(jst_pointer_finalize) {
	jst_pointer_data * d = JSTObjectGetPrivate(object);
	if (d->allocated) g_free(d->value);
	g_free(d);
	JSTObjectSetPrivate(object, NULL);
}

JSTValue jst_pointer_free JSTDeclareFunction() {
	jst_pointer_finalize(this);
	return JSTValueUndefined;
}

static JSTClass jst_pointer_init() {

	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	jsClass.className = "pointer",
	jsClass.convertToType = &jst_pointer_convert,
	jsClass.finalize = &jst_pointer_finalize;
	jst_pointer_class = JSClassRetain(JSClassCreate(&jsClass));

	return jst_pointer_class;

}

static JSTValue jst_pointer_constructor JSTDeclareFunction(width, count, zero) {

	if (!jst_pointer_class) jst_pointer_init();

	size_t width, count;
	
	if (!JSTArgumentToInt(0, &width) || !JSTArgumentToInt(1, &count))
		return NULL;

	jst_pointer_data * d = g_malloc0(sizeof(jst_pointer_data));
	d->allocated = true;
	
	if (argc > 2 && JSTValueToBoolean(argv[2]))
		d->value = g_malloc0(width * count);
	else d->value = g_malloc(width * count);
	
	JSTObject object = JSTClassInstance(jst_pointer_class, d);

	return object;

}

JSTValue jst_pointer_function JSTDeclareFunction(value, allocated) {

	if (!jst_pointer_class) jst_pointer_init();

	void * value; bool allocated = false;
	if (!JSTArgumentToPointer(0, &width)) return NULL;
	if (argc > 1) allocated = JSTValueToBoolean(argv[1]);
	
	jst_pointer_data * d = g_malloc0(sizeof(jst_pointer_data));
	d->value = value, d->allocated = allocated;

	JSTObject object = JSTClassInstance(jst_pointer_class, d);
	return object;
}
