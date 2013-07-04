#include "dyncall.inc"

#include "JSTools.h"

char * _JSTNativeString JSTProcedure (register JSStringRef STR) {
	register unsigned long size = JSStringGetMaximumUTF8CStringSize(STR);
	register char * BUFFER = g_new(char, size);
	JSStringGetUTF8CString(STR, BUFFER, size);
	return BUFFER;
}

char * _JSTStringCopy JSTProcedure (register JSValueRef VAL) {
	register JSStringRef STR = JSTCoreString(VAL);
	register char * this = JSTNativeString(STR);
	JSTCoreStringDead(STR); return this;
}

JSValueRef _JSTMakeString JSTProcedure (register char * BUFFER) {
	register JSStringRef STR = JSTMakeNativeString(BUFFER);
	register JSValueRef VAL = JSTMakeCoreString(STR);
	JSTCoreStringDead(STR); return VAL;
}

JSObjectRef _JSTMakeFunction JSTFastProcedure (char * BUFFER, void * PROC) {
	register JSStringRef STR = JSTMakeNativeString(BUFFER); 
	register JSObjectRef OBJ = JSTCoreMakeFunction(STR, PROC);
	JSTCoreStringDead(STR);
	return OBJ;
}

JSValueRef _JSTEval JSTFastProcedure (char * BUFFER, JSObjectRef OBJ) {
	register JSStringRef STR = JSTMakeNativeString (BUFFER);
	register JSValueRef VAL = JSTCoreEval(STR, OBJ);
	JSTCoreStringDead(STR);
	return VAL;
}

void _JSTSetProperty JSTFastProcedure (JSObjectRef OBJ, char * BUFFER, JSValueRef VAL, int ATTR) {
	register JSStringRef STR = JSTMakeNativeString (BUFFER);
	JSTSetCoreProperty(OBJ, STR, VAL, ATTR);
	JSTCoreStringDead(STR);
}
JSValueRef _JSTGetProperty JSTFastProcedure (JSObjectRef OBJ, char * BUFFER) {
	register JSStringRef JSTFunctionTemp = JSStringCreateWithUTF8CString (BUFFER); 
	register JSValueRef JSTFunctionValue = JSTGetCoreProperty(OBJ, JSTFunctionTemp);
	JSStringRelease(JSTFunctionTemp);
	return (JSValueRef) JSTFunctionValue;
}

JSValueRef _JSTCallFunction (register JSContextRef ctx, JSObjectRef THIS, JSObjectRef FUNC, JSValueRef * exception, ...) {
	if ( ! THIS ) THIS = JSTGlobalObject;
	if ( ! JSTReference(FUNC) || ! JSTFunction(FUNC))  {
		/* that's not a function */
		return JSTMakeUndefined();
	}
	register int argc = 0, argi = 0; va_list ap; JSValueRef arg = NULL;
	va_start(ap, exception);
		while ((arg = (JSValueRef) va_arg(ap, JSValueRef))) argc++;
	va_end(ap);
	JSValueRef argv[argc+1];
	va_start(ap, exception);
		while (argi <= argc) argv[argi++] = va_arg(ap, JSValueRef);
	va_end(ap);
	return JSObjectCallAsFunction JSTExec (FUNC, THIS, argc, argv);
}

JSValueRef _JSTCallConstructor(register JSContextRef ctx, JSObjectRef OBJ, JSValueRef * exception, ...) {
	if ( ! JSTReference(OBJ) || ! JSTConstructor(OBJ))  {
		/* that's not a constructor */
		return JSTMakeUndefined();
	}
	register int argc = 0, argi = 0; va_list ap; JSValueRef arg = NULL;
	va_start(ap, exception);
		while ((arg = (JSValueRef) va_arg(ap, JSValueRef))) argc++;
	va_end(ap);
	JSValueRef argv[argc+1];
	va_start(ap, exception);
		while (argi <= argc) argv[argi++] = va_arg(ap, JSValueRef);
	va_end(ap);
	return JSObjectCallAsConstructor JSTExec (OBJ, argc, argv);
}

JSObjectRef _JSTMakeCorePropertyFunction JSTFastProcedure (JSObjectRef OBJ, register JSStringRef STR, void * PROC) {
	register JSObjectRef FUNC = JSTCoreMakeFunction(STR, PROC);
	JSTSetCoreProperty(OBJ, STR, (JSValueRef) FUNC, JSTPropertyConst);
	return FUNC;
}

JSObjectRef _JSTMakePropertyFunction JSTFastProcedure (JSObjectRef OBJ, char * BUFFER, void * PROC) {
	register JSStringRef STR = JSTMakeNativeString(BUFFER);
	JSObjectRef FUNC = JSTMakeCorePropertyFunction(OBJ, STR, PROC);
	JSTCoreStringDead(STR);
	return FUNC;
}

JSObjectRef _JSTMakeCoreConstructorProperty JSTFastProcedure (JSObjectRef OBJ, JSStringRef STR, JSClassRef CLASS, void * PROC) {
	register JSObjectRef FUNC = JSTMakeConstructor(CLASS, PROC);
	JSTSetCoreProperty(OBJ, STR, (JSValueRef) FUNC, JSTPropertyConst);
	return FUNC;
	
}

JSObjectRef _JSTMakeConstructorProperty JSTFastProcedure (JSObjectRef OBJ, char * BUFFER, JSClassRef CLASS, void * PROC) {
	register JSStringRef STR = JSTMakeNativeString(BUFFER);
	register JSObjectRef FUNC = JSTMakeCoreConstructorProperty(OBJ, STR, CLASS, PROC);
	JSTCoreStringDead(STR);
	return FUNC;
}

JSStringRef * JSTCoreStringTable(long * items, ...) {
	register long count = 0, index = 0; va_list ap; va_start(ap, items);
	while (va_arg(ap, char *) != NULL) count++; va_end(ap); va_start(ap, items);
	if ( ! count ) return NULL; if (items) *items = count;
	register JSStringRef * table = g_new0(JSStringRef, count + 1);
	while (--count) table[index++] = JSTMakeNativeString(va_arg(ap, char *)); va_end(ap);
	return table;
}

JSTCharBuffer * JSTNewCharBuffer(void) {
	JSTCharBuffer *new_string = g_new0(JSTCharBuffer, 1); new_string->dynamic = 1;
	return new_string;
}

int JSTCharBufferDead(register JSTCharBuffer * string) {
	if (string->dynamic) { g_free(string->pointer);  g_free(string); return 1; }
	return 0;
}

int JSTCharBufferExpand(register JSTCharBuffer * string, register unsigned long amount) {
	if (string && string->dynamic) {
		register unsigned long size = (string->allocated + amount);
		string->pointer = g_realloc(string->pointer, size);
		string->allocated = size;
		return 1;
	} else return 0;
}

int JSTCharBufferShrink(register JSTCharBuffer * string, register unsigned long amount) {
	if (string && string->dynamic && string->allocated && string->allocated >= amount) {
		register unsigned long size = (string->allocated - amount);
		string->pointer = g_realloc(string->pointer, size);
		string->allocated = size;
		if (string->index >= size) string->index = (size - 1);
		string->pointer[string->index] = '\0'; string->length = string->index;
		return 1;
	} else return 0;
}

/*
	Internal Procedure JSTCharBufferJustify
	Extend string memory according to index if applicable.
	The validity of the string parameter must be validated or trusted by the caller.
	If the memory is dynamic, and index exceeds allocated, the memory will be resized.
	if the memory is not dynamic and index exceeds allocated, index will point to
	the end of the data record.
	
*/  static void JSTCharBufferJustify(register JSTCharBuffer * string) {
	if (string->index > string->allocated)
		if (! JSTCharBufferExpand(string, (string->index - string->allocated)+1) )
			string->index = string->allocated - 1;
}

char * JSTCharBufferIndexPointer(register JSTCharBuffer * string) {
	if (string) {
		JSTCharBufferJustify(string);
		if (string->pointer) return (string->pointer + string->index);
	}
	return NULL;
}

int JSTCharBufferPrintFormat(register JSTCharBuffer * string, char * format, ...) {
	/* Modifies: index, length, allocated, pointer, *pointer */
	/* returns number of bytes written (if any) excluding zero terminator */
	register unsigned long writeable = 0, written = 0;
	va_list args; va_start(args, format);
	if (! string ) return 0; JSTCharBufferJustify(string);
	_JSTCharBufferPrintFormat_:
	writeable = (string->allocated - string->index);
	written = vsnprintf((string->pointer + string->index), writeable, format, args);
	if (written >= writeable) {
		if (string->dynamic) {
			JSTCharBufferExpand(string, (written - writeable) + 1);
			goto _JSTCharBufferPrintFormat_;
		} else return 0;
	}
	string->index += written; string->length +=written;
	return written;
}

/* recognize a char * as a string */
JSTCharBuffer * JSTCharBufferCopy(char * data) {
	register unsigned long finalIndex = 0;
	register char *fastSource = data;
	if ( ! fastSource ) return NULL;
	register JSTCharBuffer *this_string = g_new0(JSTCharBuffer, 1);
	this_string->pointer = fastSource;
	while ( fastSource[finalIndex++] );	this_string->allocated = finalIndex--;
	this_string->index = finalIndex; this_string->length = finalIndex;
	return this_string;
}

/* allocate a new dynamic string from char pointer; optional truncate/buffer length */
JSTCharBuffer * JSTCharBufferCopyN(register char *data, register unsigned long length) {
	if ( ! data ) return NULL; if (! length ) { while (data[length++]); length--; };
	JSTCharBuffer *this_string = JSTNewCharBuffer();
	if (! length ) return this_string;
	register char *fastSource = data, *fastDest = g_new0(char, length + 1);
	this_string->pointer = fastDest; this_string->allocated = length;
	while ( (*fastDest++ = *fastSource++) && length-- );
	this_string->index = (this_string->allocated++ - length);
	this_string->length = this_string->index;
	return this_string;
}

unsigned long JSTCharBufferLength(register JSTCharBuffer * string) {
	if (!string) return 0;
	register char *fastSource = string->pointer;
	register unsigned long finalIndex = 0;
	while ( finalIndex != string->allocated ) if (! fastSource[finalIndex++] ) return --finalIndex;
	return finalIndex;
}

unsigned long JSTCharBufferIndexLength(register JSTCharBuffer * string) {
	if (!string) return 0; register char *fastSource = (string->pointer + string->index);
	register unsigned long finalIndex = 0;
	while ( finalIndex != string->allocated ) if (! fastSource[finalIndex++] ) return --finalIndex;
	return finalIndex;
}



