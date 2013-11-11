
#define JSTNativeInit(o) JST(JSTNativeInit_, o)

#define JSTNativeAlignment G_MEM_ALIGN
#define JSTNativeByteOrder G_BYTE_ORDER

#define JSN_C_TYPE_VOID 1
#define JSN_C_TYPE_CONST 2
#define JSN_C_TYPE_UNSIGNED 4
#define JSN_C_TYPE_BOOL 8
#define JSN_C_TYPE_CHAR 16
#define JSN_C_TYPE_SHORT 32
#define JSN_C_TYPE_INT 64
#define JSN_C_TYPE_ENUM 64
#define JSN_C_TYPE_LONG 128
#define JSN_C_TYPE_SIZE 256
#define JSN_C_TYPE_POINTER 512
#define JSN_C_TYPE_INT64 1024
#define JSN_C_TYPE_FLOAT 2048
#define JSN_C_TYPE_DOUBLE 4096
#define JSN_C_TYPE_STRUCT 8192
#define JSN_C_TYPE_UNION 16384
#define JSN_C_TYPE_ELLIPSIS 32768
#define JSN_C_TYPE_UTF8 65536
#define JSN_C_TYPE_UTF16 131072
#define JSN_C_TYPE_UTF32 262144
#define JSN_C_TYPE_STRING 524288
#define JSN_C_TYPE_VALUE 1048576

#define JSN_C_TYPE_VOID_SIZE 0
#define JSN_C_TYPE_CONST_SIZE 0
#define JSN_C_TYPE_UNSIGNED_SIZE sizeof(unsigned)
#define JSN_C_TYPE_BOOL_SIZE sizeof(bool)
#define JSN_C_TYPE_CHAR_SIZE sizeof(char)
#define JSN_C_TYPE_SHORT_SIZE sizeof(short)
#define JSN_C_TYPE_INT_SIZE sizeof(int)
#define JSN_C_TYPE_ENUM_SIZE sizeof(int)
#define JSN_C_TYPE_LONG_SIZE sizeof(long)
#define JSN_C_TYPE_SIZE_SIZE sizeof(size_t)
#define JSN_C_TYPE_POINTER_SIZE sizeof(void *)
#define JSN_C_TYPE_INT64_SIZE sizeof(long long)
#define JSN_C_TYPE_FLOAT_SIZE sizeof(float)
#define JSN_C_TYPE_DOUBLE_SIZE sizeof(double)
#define JSN_C_TYPE_STRUCT_SIZE 0
#define JSN_C_TYPE_UNION_SIZE 0
#define JSN_C_TYPE_ELLIPSIS_SIZE 0
#define JSN_C_TYPE_UTF8_SIZE sizeof(UTF8)
#define JSN_C_TYPE_UTF16_SIZE sizeof(UTF16)
#define JSN_C_TYPE_UTF32_SIZE sizeof(UTF32)
#define JSN_C_TYPE_STRING_SIZE sizeof(JSStringRef)
#define JSN_C_TYPE_VALUE_SIZE sizeof(JSValueRef)

#ifdef JSTools_h

extern JSTObject JSTNativeInit_ JSTUtility(JSTObject js);
extern int JSTNativeTypeGetSize(int t);
extern const char * JSTNativeTypeGetName(int t);

#else

#include "JSTNative.inc"

int JSTNativeTypeGetSize(int t) {
	if (t == 0) return t;
	else if (t & JSN_C_TYPE_POINTER) return sizeof(void*);
	else if (t == JSN_C_TYPE_BOOL) return sizeof(bool);
	else if (t & JSN_C_TYPE_CHAR) return sizeof(char);
	else if (t & JSN_C_TYPE_SHORT) return sizeof(short);
	else if (t & JSN_C_TYPE_INT || t == JSN_C_TYPE_UNSIGNED) return sizeof(int);
	else if (t & JSN_C_TYPE_LONG) return sizeof(long);
	else if (t & JSN_C_TYPE_INT64) return sizeof(long long);
	else if (t & JSN_C_TYPE_SIZE) return sizeof(size_t);
	else if (t == JSN_C_TYPE_FLOAT) return sizeof(float);
	else if (t == JSN_C_TYPE_DOUBLE) return sizeof(double);
	else if (t == JSN_C_TYPE_UTF8) return sizeof(UTF8);
	else if (t == JSN_C_TYPE_UTF16) return sizeof(UTF16);
	else if (t == JSN_C_TYPE_UTF32) return sizeof(UTF32);
	else return 0;
}

const char * JSTNativeTypeGetName(int t) {
	if (t == 0) return "void";
	else if (t & JSN_C_TYPE_POINTER || t == JSN_C_TYPE_VALUE || t == JSN_C_TYPE_STRING ) return "pointer";
	else if (t == JSN_C_TYPE_ELLIPSIS) return "ellipsis";
	else if (t == JSN_C_TYPE_BOOL) return "bool";
	else if (t & JSN_C_TYPE_CHAR) return (t & JSN_C_TYPE_UNSIGNED)?"char":"schar";
	else if (t & JSN_C_TYPE_SHORT) return (t & JSN_C_TYPE_UNSIGNED)?"ushort":"short";
	else if (t & JSN_C_TYPE_INT || t == JSN_C_TYPE_UNSIGNED) return (t & JSN_C_TYPE_UNSIGNED)?"uint":"int";
	else if (t & JSN_C_TYPE_LONG) return (t & JSN_C_TYPE_UNSIGNED)?"ulong":"long";
	else if (t & JSN_C_TYPE_INT64) return (t & JSN_C_TYPE_UNSIGNED)?"uint64":"int64";
	else if (t & JSN_C_TYPE_SIZE) return (t & JSN_C_TYPE_UNSIGNED)?"size":"ssize";
	else if (t == JSN_C_TYPE_FLOAT) return "float";
	else if (t == JSN_C_TYPE_DOUBLE) return "double";
	else if (t == JSN_C_TYPE_UTF8) return "utf8";
	else if (t == JSN_C_TYPE_UTF16) return "utf16";
	else if (t == JSN_C_TYPE_UTF32) return "utf32";
	else return NULL;
}

char JSTNativeTypeGetChar(int t) {
	if (t & JSN_C_TYPE_POINTER || t == JSN_C_TYPE_VALUE || t == JSN_C_TYPE_STRING ) return 'p';
	else if (t == JSN_C_TYPE_BOOL) return 'B';
	else if (t & JSN_C_TYPE_CHAR) return (t & JSN_C_TYPE_UNSIGNED)?'C':'c';
	else if (t & JSN_C_TYPE_SHORT) return (t & JSN_C_TYPE_UNSIGNED)?'S':'s';
	else if (t & JSN_C_TYPE_INT || t == JSN_C_TYPE_UNSIGNED) return (t & JSN_C_TYPE_UNSIGNED)?'I':'i';
	else if (t & JSN_C_TYPE_LONG) return (t & JSN_C_TYPE_UNSIGNED)?'J':'j';
	else if (t & JSN_C_TYPE_INT64) return (t & JSN_C_TYPE_UNSIGNED)?'L':'l';
	else if (t & JSN_C_TYPE_SIZE) return (t & JSN_C_TYPE_UNSIGNED)?'J':'j';
	else if (t == JSN_C_TYPE_FLOAT) return 'f';
	else if (t == JSN_C_TYPE_DOUBLE) return 'd';
	else if (t == JSN_C_TYPE_UTF8) return 'C';
	else if (t == JSN_C_TYPE_UTF16) return 'S';
	else if (t == JSN_C_TYPE_UTF32) return 'J';
	else return 'v';
}

typedef struct jsNativeCallbackStruct {
	void * cb; void * group; void * object; void * function; void * proto;
	int type; int length;
} jsNativeCallbackStruct;

char jsNativeCallbackHandler(DCCallback* cb, DCArgs* args, DCValue* result, jsNativeCallbackStruct * cbs) {

	JSTContext ctx = JSTContextCreateInGroup(cbs->group, NULL);
	JSTObject e = NULL; JSTValue *exception = (void*)&e;
	JSTValue argv[cbs->length]; int i, t;

	for (i = 0; i < cbs->length; i++) {
		t = JSTValueToDouble(JSTObjectGetPropertyAtIndex(cbs->proto, i));
		if (t & JSN_C_TYPE_POINTER || t == JSN_C_TYPE_STRING)
			argv[i] = JSTValueFromPointer(dcbArgPointer(args));
		else if (t & JSN_C_TYPE_SIZE) argv[i] = JSTValueFromDouble((t & JSN_C_TYPE_UNSIGNED)?dcbArgULong(args):dcbArgLong(args));
		else if (t & JSN_C_TYPE_LONG) argv[i] = JSTValueFromDouble((t & JSN_C_TYPE_UNSIGNED)?dcbArgULong(args):dcbArgLong(args));
		else if (t & JSN_C_TYPE_INT || t == JSN_C_TYPE_UNSIGNED) argv[i] = JSTValueFromDouble((t & JSN_C_TYPE_UNSIGNED)?dcbArgUInt(args):dcbArgInt(args));
		else if (t == JSN_C_TYPE_DOUBLE) argv[i] = JSTValueFromDouble(dcbArgDouble(args));
		else if (t == JSN_C_TYPE_BOOL) argv[i] = JSTValueFromBoolean(dcbArgBool(args));
		else if (t & JSN_C_TYPE_CHAR) argv[i] = JSTValueFromDouble((t & JSN_C_TYPE_UNSIGNED)?dcbArgUChar(args):dcbArgChar(args));
		else if (t & JSN_C_TYPE_SHORT) argv[i] = JSTValueFromDouble((t & JSN_C_TYPE_UNSIGNED)?dcbArgUShort(args):dcbArgShort(args));
		else if (t & JSN_C_TYPE_INT64) argv[i] = JSTValueFromDouble((t & JSN_C_TYPE_UNSIGNED)?dcbArgULongLong(args):dcbArgLongLong(args));
		else if (t == JSN_C_TYPE_VALUE) argv[i] = dcbArgPointer(args);
		else if (t == JSN_C_TYPE_FLOAT) argv[i] = JSTValueFromDouble(dcbArgFloat(args));
		else if (t == JSN_C_TYPE_UTF8) argv[i] = JSTValueFromDouble(dcbArgUChar(args));
		else if (t == JSN_C_TYPE_UTF16) argv[i] = JSTValueFromDouble(dcbArgUShort(args));
		else if (t == JSN_C_TYPE_UTF32) argv[i] = JSTValueFromDouble(dcbArgULong(args));
		else {
			JSTScriptNativeError("jsNativeCallbackHandler: argument %i: is an unknown type", i);
			JSTScriptReportException(), exit(1);
		}
		if (JSTScriptHasError) JSTScriptReportException(), exit(1);
	}

	JSTValue value = JST(JSObjectCallAsFunction, cbs->function, cbs->object, cbs->length, argv);

	if (JSTScriptHasError) JSTScriptReportException(), exit(1);

	t = cbs->type;
	if (t & JSN_C_TYPE_POINTER || t == JSN_C_TYPE_STRING) {
		result->p = JSTValueToPointer(value); return 'p';
	} else if (t & JSN_C_TYPE_SIZE) {result->j = JSTValueToDouble(value); return 'j';}
	else if (t & JSN_C_TYPE_LONG) {result->j =JSTValueToDouble(value); return 'j';}
	else if (t & JSN_C_TYPE_INT || t == JSN_C_TYPE_UNSIGNED) {result->i = JSTValueToDouble(value); return 'i';}
	else if (t == JSN_C_TYPE_DOUBLE) {result->d = JSTValueToDouble(value); return 'd';}
	else if (t == JSN_C_TYPE_BOOL) { result->B = JSTValueToBoolean(value); return 'B'; }
	else if (t & JSN_C_TYPE_CHAR) {result->c = JSTValueToDouble(value); return 'c';}
	else if (t & JSN_C_TYPE_SHORT) {result->s = JSTValueToDouble(value); return 's';}
	else if (t & JSN_C_TYPE_INT64) {result->l = JSTValueToDouble(value); return 'l';}
	else if (t == JSN_C_TYPE_VALUE) {result->p = (void*) value; return 'p';}
	else if (t == JSN_C_TYPE_FLOAT) {result->f = JSTValueToDouble(value); return 'f';}
	else if (t == JSN_C_TYPE_UTF8) {result->C = JSTValueToDouble(value); return 'C';}
	else if (t == JSN_C_TYPE_UTF16) {result->S = JSTValueToDouble(value); return 'S';}
	else if (t == JSN_C_TYPE_UTF32) {result->J = JSTValueToDouble(value); return 'J';}
	else {return 'v';}

}

static JSTValue jsNativeCallback JSTDeclareFunction() {

	jsNativeCallbackStruct * cbs = calloc(1, sizeof(jsNativeCallbackStruct));

	cbs->group = (void*)JSContextGetGroup(ctx);
	cbs->object = (void*)argv[0];
	cbs->type = JSTValueToDouble(argv[1]);
	cbs->function = (void*) argv[2];
	cbs->proto = (void*) argv[3];
	cbs->length = JSTValueToDouble(JSTObjectGetProperty(cbs->proto, "length"));

	// These value/objects must not be garbage collected and shouldn't be carelessly modified.
	JSTValueProtect(cbs->object); JSTValueProtect(cbs->function); JSTValueProtect(cbs->proto);

	int i, type; char signature[cbs->length+3];

	for (i = 0; i < cbs->length; i++) {
		type = JSTValueToDouble(JSTObjectGetPropertyAtIndex(cbs->proto, i));
		signature[i] = JSTNativeTypeGetChar(type);
	};

	signature[i++] = ')'; signature[i++] = JSTNativeTypeGetChar(cbs->type); signature[i] = 0;

	cbs->cb = dcbNewCallback((char*) &signature, (void*) &jsNativeCallbackHandler, cbs);

	JSTObject out = JSTValueToObject(JSTValueFromPointer(cbs->cb));
	JSTObjectSetProperty(out, "struct", JSTValueFromPointer(cbs), 0);

	return (JSTValue) out;

}

static JSTValue jsNativeCallbackFree JSTDeclareFunction() {
	jsNativeCallbackStruct * cbs = JSTValueToPointer(JSTObjectGetProperty((JSTObject)argv[0], "struct"));
	JSTValueUnprotect(cbs->object); JSTValueUnprotect(cbs->function);
	JSTValueUnprotect(cbs->proto); dcbFreeCallback(cbs->cb);
	free(cbs); return JSTValueNull;
}

int jsNativeCallbackTest(void * cbs, int i) {
	return ((int(*)(int))cbs)(i);
}

static JSTValue jsNativeLibraryLoad JSTDeclareFunction(String name) {
	// must have one arg of type String
	char * buffer = JSTValueToUTF8(argv[0]);
	JSTValue result = JSTValueFromPointer(dlLoadLibrary(buffer));
	JSTStringFreeUTF8(buffer);
	return result;
}

static JSTValue jsNativeLibraryFree JSTDeclareFunction(Pointer library) {
	// must have one arg of type Number
	dlFreeLibrary(JSTValueToPointer(argv[0])); return JSTValueUndefined;
}

static JSTValue jsNativeLibraryFindSymbol JSTDeclareFunction(Pointer library, String symbol) {
	// must have first arg of type Number
	// must have second arg of type String
	char * buffer = JSTValueToUTF8(argv[1]);
	JSTValue result = JSTValueFromPointer(dlFindSymbol(JSTValueToPointer(argv[0]), buffer));
	JSTStringFreeUTF8(buffer);
	return result;
}

// This procedure is lacking a plethora of error checking and functional logic
static JSTValue jsNativeExec JSTDeclareFunction(...) {

	JSTValue result = JSTValueNull;
	JSTObject proto = JSTValueToObject(JSTObjectGetProperty(this, "proto"));
	int protoLen = JSTValueToDouble(JSTObjectGetProperty(proto, "length"));

	const char * fname = "Procedure", * job = "type error";
	int
		callVMMode = JSTValueToDouble(JSTObjectGetProperty(this, "mode")),
		resource = 0, resource16 = 0,
		stackSize = JSTValueToDouble(JSTObjectGetProperty(this, "size"));

	if (argc != protoLen && callVMMode != DC_CALL_C_ELLIPSIS) {
		return JSTScriptNativeError("%s: %s: argument count does not match function prototype", fname, job);
	}

	if (callVMMode == DC_CALL_C_ELLIPSIS) {
		stackSize += ((argc - protoLen) * JSN_C_TYPE_DOUBLE_SIZE); // plenty
	}

	void 
		* callVM  = dcNewCallVM(stackSize),
		* conversion[argc], * conversion16[argc], 
		* fn = JSTValueToPointer(this);

	if (dcMode(callVM, callVMMode), dcGetError(callVM) == DC_ERROR_UNSUPPORTED_MODE) {
		result = JSTScriptNativeError("%s: %s: `%i' is an invalid or unknown virtual call mode", fname, job, callVMMode);
		goto bail;
	}

	JSTObject argType = NULL, returnType = JSTObjectGetObject(this, "return");
	int argIndex = 0, argCode, retCode = JSTValueToDouble(returnType);

	while (argIndex < argc) {
		JSTValue value = argv[argIndex];
		int jsType = JSTValueGetType(value);
		if (argIndex < protoLen) {
			argCode = JSTValueToDouble(JSTObjectGetPropertyAtIndex(proto, argIndex));
		} else if (callVMMode == DC_CALL_C_ELLIPSIS) {
			JSTObject constructor = NULL;
			if (jsType == JSTValueTypeObject) {
				constructor = JSTObjectGetObject(value, "type");
				argCode = JSTValueToDouble(constructor);
			} else if (jsType == JSTValueTypeString) {
				argCode = JSN_C_TYPE_UTF8 | JSN_C_TYPE_POINTER;
			} else if (jsType == JSTValueTypeNumber) {
				argCode = JSN_C_TYPE_INT;
			} else printf("Oh shit: %i, %i\n", jsType, JSTValueTypeObject);

		} else {
			result = JSTScriptNativeError("%s: %s: `%i' unable to determine conversion procedure for argument", fname, job, argIndex);
			goto bail;
		}
		argIndex++;
		if (argCode & JSN_C_TYPE_POINTER) {
			if (argCode & JSN_C_TYPE_UTF8) {
				if (jsType == JSTValueTypeObject || jsType == JSTValueTypeNumber) dcArgPointer(callVM, JSTValueToPointer(value));
				else dcArgPointer(callVM, conversion[resource++] = JSTValueToUTF8(value));
				continue;
			} else if (argCode & JSN_C_TYPE_UTF16) {
				if (jsType == JSTValueTypeObject || jsType == JSTValueTypeNumber) dcArgPointer(callVM, JSTValueToPointer(value));
				else dcArgPointer(callVM, (void*)JSTStringUTF16(conversion16[resource16++] = JSTValueToString(value)));
				continue;
			} else if (argCode & JSN_C_TYPE_UTF32) {
				if (jsType == JSTValueTypeObject || jsType == JSTValueTypeNumber) dcArgPointer(callVM, JSTValueToPointer(value));
				else dcArgPointer(callVM, conversion[resource++] = JSTStringToUTF32(JSTValueToString(value), JSTValueToDouble(JSTObjectGetProperty(value, "length")), true));
				continue;
			} else if (argCode & JSN_C_TYPE_VALUE) {
				dcArgPointer(callVM, (void*)value);
				continue;
			} else if (argCode & JSN_C_TYPE_STRING) {
				dcArgPointer(callVM, JSTValueToString(value));
				continue;
			} else {
				dcArgPointer(callVM, JSTValueToPointer(value));
				continue;
			}
		} else if (argCode == JSN_C_TYPE_BOOL) dcArgBool(callVM, JSTValueToBoolean(value));
		else if (argCode & JSN_C_TYPE_CHAR) dcArgChar(callVM, JSTValueToDouble(value));
		else if (argCode & JSN_C_TYPE_SHORT) dcArgShort(callVM, JSTValueToDouble(value));
		else if (argCode & JSN_C_TYPE_INT) dcArgInt(callVM, JSTValueToDouble(value));
		else if (argCode & JSN_C_TYPE_LONG) dcArgLong(callVM, JSTValueToDouble(value));
		else if (argCode & JSN_C_TYPE_INT64) dcArgLongLong(callVM, JSTValueToDouble(value));
		else if (argCode & JSN_C_TYPE_SIZE) dcArgLong(callVM, JSTValueToDouble(value));
		else if (argCode == JSN_C_TYPE_FLOAT) dcArgFloat(callVM, JSTValueToDouble(value));
		else if (argCode == JSN_C_TYPE_DOUBLE) dcArgDouble(callVM, JSTValueToDouble(value));
		else if (argCode == JSN_C_TYPE_VALUE) dcArgPointer(callVM, (void*) value);
		else if (argCode == JSN_C_TYPE_STRING) dcArgPointer(callVM, (void*)(conversion16[resource16++] = JSTValueToString(value)));
		else {
			result = JSTScriptNativeError("%s: %s: argument #%i (%i) is an unknown or invalid argument type", fname, job, argIndex, argCode);
			goto bail;
		}
		continue;
	}

	bool unsign = (retCode & JSN_C_TYPE_UNSIGNED);
	if (unsign) retCode = (retCode &~ JSN_C_TYPE_UNSIGNED);

	if (retCode & JSN_C_TYPE_POINTER) result = JSTValueFromPointer(dcCallPointer(callVM, fn));
	else if (retCode == JSN_C_TYPE_BOOL) result = JSTValueFromBoolean(dcCallBool(callVM, fn));
	else if (retCode & JSN_C_TYPE_CHAR) result = JSTValueFromDouble((double) (unsign)?dcCallChar(callVM, fn):(signed char)dcCallChar(callVM, fn));
	else if (retCode & JSN_C_TYPE_SHORT) result = JSTValueFromDouble((double) (unsign)?(unsigned short)dcCallShort(callVM, fn):dcCallShort(callVM, fn));
	else if (retCode & JSN_C_TYPE_INT) result = JSTValueFromDouble((double) (unsign)?(unsigned int)dcCallInt(callVM, fn):dcCallInt(callVM, fn));
	else if (retCode & JSN_C_TYPE_LONG) result = JSTValueFromDouble(dcCallLong(callVM, fn));
	else if (retCode & JSN_C_TYPE_INT64) result = JSTValueFromDouble((double) (unsign)?(unsigned long long)dcCallLongLong(callVM, fn):dcCallLongLong(callVM, fn));
	else if (retCode & JSN_C_TYPE_SIZE) result = JSTValueFromDouble(dcCallLong(callVM, fn));
	else if (retCode == JSN_C_TYPE_FLOAT) result = JSTValueFromDouble(dcCallFloat(callVM, fn));
	else if (retCode == JSN_C_TYPE_DOUBLE) result = JSTValueFromDouble(dcCallDouble(callVM, fn));
	else if (retCode == JSN_C_TYPE_VOID) { result = JSTValueUndefined; dcCallVoid(callVM, fn); }
	else {
		result = JSTScriptNativeError("%s: %s: procedure return type `%i' is an unknown or invalid return type", fname, job, returnType);
		goto bail;
	}

bail:
	while (resource) g_free(conversion[--resource]);
	while (resource16) JSTStringRelease(conversion16[--resource16]);
	dcFree(callVM); 
done:
	return result;

}

static JSTValue jsNativeAddressRead JSTDeclareFunction (address, type) {

	const char * fname = "native.address.read", * job = "unable to read pointer contents";

	if (argc != 2) return JSTScriptNativeError("%s: %s: %s arguments", fname, job, (argc > 2)?"too many":"insufficient");

	// must have first arg of type Number
	// must have second arg of type Number
	void * address = JSTValueToPointer(argv[0]);
	if (! address ) return JSTScriptNativeError("%s: %s: pointer value is null", fname, job);

	int type = JSTValueToDouble(argv[1]); bool unsign = (type & JSN_C_TYPE_UNSIGNED);

	if (type & JSN_C_TYPE_POINTER) return JSTValueFromPointer(*(intptr_t*)(address));
	else if (type == JSN_C_TYPE_BOOL) return JSTValueFromDouble((double) *(bool*)(address));
	else if (type & JSN_C_TYPE_CHAR) return JSTValueFromDouble((double) (unsign)?*(char*)(address):*(signed char*)(address));
	else if (type & JSN_C_TYPE_SHORT) return JSTValueFromDouble((double) (unsign)?*(unsigned short*)(address):*(short*)(address));
	else if (type & JSN_C_TYPE_INT || type == JSN_C_TYPE_UNSIGNED) return JSTValueFromDouble((double) (unsign)?*(unsigned int*)(address):*(int*)(address));
	else if (type & JSN_C_TYPE_LONG) return JSTValueFromDouble((double) (unsign)?*(unsigned long*)(address):*(long*)(address));
	else if (type & JSN_C_TYPE_INT64) return JSTValueFromDouble((double) (unsign)?*(unsigned long long*)(address):*(long long*)(address));
	else if (type & JSN_C_TYPE_SIZE) return JSTValueFromDouble((double) (unsign)?*(size_t*)(address):*(ssize_t*)(address));
	else if (type == JSN_C_TYPE_FLOAT) return JSTValueFromDouble((double) *(float*)(address));
	else if (type == JSN_C_TYPE_DOUBLE) return JSTValueFromDouble((double) *(double*)(address));
	else if (type == JSN_C_TYPE_UTF8) return JSTValueFromDouble((double) *(UTF8*)(address));
	else if (type == JSN_C_TYPE_UTF16) return JSTValueFromDouble((double) *(UTF16*)(address));
	else if (type == JSN_C_TYPE_UTF32) return JSTValueFromDouble((double) *(UTF32*)(address));
	else if (type == JSN_C_TYPE_VALUE) return ((JSTValue)*(intptr_t*)(address));
	else if (type == JSN_C_TYPE_STRING) return JSTValueFromPointer(*(intptr_t*)(address));
	else if (unsign) return JSTScriptNativeError("%s: %s: `%i' is not a known or signable integer type", fname, job, type);
	else return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

}

static JSTValue jsNativeAddressWrite JSTDeclareFunction (address, type, value) {

	const char * fname = "native.address.write", * job = "unable to write pointer contents";

	if (argc != 3) return JSTScriptNativeError("%s: %s: %s arguments", fname, job, (argc > 2)?"too many":"insufficient");

	// must have first arg of type Number
	// must have second arg of type Number
	// must have third arg of type Number
	void * address = JSTValueToPointer(argv[0]);
	if (! address ) return JSTScriptNativeError("%s: %s: pointer value is null", fname, job);

	int type = JSTValueToDouble(argv[1]);

	if (type & JSN_C_TYPE_CONST) {
		return JSTScriptNativeError("%s: %s: pointer value is constant", fname, job);
	}

	double value = JSTValueToDouble(argv[2]);

	if (type & JSN_C_TYPE_POINTER) *(intptr_t*)(address) = (intptr_t)value;
	else if (type == JSN_C_TYPE_BOOL) *(bool*)(address) = (bool) value; 
	else if (type & JSN_C_TYPE_CHAR) *(char*)(address) = (char) value;
	else if (type & JSN_C_TYPE_SHORT) *(short*)(address) = (short) value;
	else if (type & JSN_C_TYPE_INT || type == JSN_C_TYPE_UNSIGNED) *(int*)(address) = (int) value;
	else if (type & JSN_C_TYPE_LONG) *(long*)(address) = (long) value;
	else if (type & JSN_C_TYPE_INT64) *(long long*)(address) = (long long) value;
	else if (type & JSN_C_TYPE_SIZE) *(size_t*)(address) = (size_t) value;
	else if (type == JSN_C_TYPE_FLOAT) *(float*)(address) = (float) value;
	else if (type == JSN_C_TYPE_DOUBLE) *(double*)(address) = value;
	else if (type == JSN_C_TYPE_UTF8) *(UTF8*)(address) = (UTF8) value;
	else if (type == JSN_C_TYPE_UTF16) *(UTF16*)(address) = (UTF16) value;
	else if (type == JSN_C_TYPE_UTF32) *(UTF32*)(address) = (UTF32) value;
	else if (type == JSN_C_TYPE_VALUE) *(intptr_t*)(address) = ((intptr_t)value);
	else if (type == JSN_C_TYPE_STRING) *(intptr_t*)(address) = ((intptr_t)value);
	else if (type & JSN_C_TYPE_UNSIGNED) return JSTScriptNativeError("%s: %s: `%i' is not a known or signable integer type", fname, job, type);
	else return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

	return JSTValueFromBoolean(true);

}

static JSTValue jsStringToUTF8 JSTDeclareFunction(Value target) {
	// must have one arg of type String
	return JSTValueFromPointer(JSTValueToUTF8(argv[0]));
}

static JSTValue jsStringFromUTF8 JSTDeclareFunction(Pointer target) {
	// must have one arg of type char *
	return JSTValueFromUTF8(JSTValueToPointer(argv[0]));
}

static JSTValue jsStringToUTF32 JSTDeclareFunction(Value target) {
	JSTString jss = JSTValueToString(argv[0]);
	size_t len = JSTValueToDouble(JSTObjectGetProperty(argv[0], "length"));
	return JSTValueFromPointer(JSTStringToUTF32(jss, len, true));
}

static JSTValue jsNativeAddressFree JSTDeclareFunction(Pointer target, ...) {
	void *p; JSTObject o; JSTValue v; size_t i = 0;
	while (i < argc) {
		v = argv[i++];
		if (JSTValueIsVoid(v)) continue;
		if (JSTValueIsObject(v)) {
			o = JSTValueToObject(v);
			if (JSTObjectHasProperty(o, "release")) { // assume its a releaseable object
				JSTScriptNativeEval("this.release()", o);
				continue;
			} else if (JSTObjectHasProperty(o, "free")) { // assume its a freeable object
				JSTScriptNativeEval("this.free()", o);
				continue;
			} else if (JSTObjectHasProperty(o, "&")) { // assume its a freeable object
				free(JSTValueToPointer(JSTScriptNativeEval("this['&']", o)));
				continue;
			} else { // assume its an object with toValue
				p = JSTValueToPointer(v); if (p) free(p);
				continue;
			}
		} else { // Assume its a number
			p = JSTValueToPointer(v); if (p) free(p);
		}
	}
	return JSTValueUndefined;
}

static JSTDeclareConvertor(jsNativeContainerConvert) {
	JSTValue conversion;
	JSTObject convert = (JSTObject) JSTObjectGetProperty(JSTObjectGetConstructor(object), "convert");
	if (JSTValueIsVoid(convert)) return false;
	else if (kJSTypeString == type) conversion = JSTFunctionCall(convert, object, JSTScriptNativeEval("String", NULL));
	else if (kJSTypeNumber == type) conversion = JSTFunctionCall(convert, object, JSTScriptNativeEval("Number", NULL));
	else if (kJSTypeBoolean == type) conversion = JSTFunctionCall(convert, object, JSTScriptNativeEval("Boolean", NULL));
	else if (kJSTypeObject == type) conversion = JSTFunctionCall(convert, object, JSTScriptNativeEval("Object", NULL));
	if (JSTValueIsVoid(conversion)) return false;
	return conversion;
}

static JSTDeclareDeleteProperty(jsNativeContainerDelete) {
	static bool requestInProgress; if (requestInProgress) return false; else requestInProgress = true;
	bool result = false;
	JSTObject delete = (JSTObject) JSTObjectGetProperty(JSTObjectGetConstructor(object), "delete");
	if (JSTObjectIsFunction(delete)) result = JSTValueToBoolean(JSTFunctionCall(delete, object, JSTStringToValue(propertyName, false)));
	requestInProgress = false; return result;
}

static JSTDeclareGetProperty(jsNativeContainerGet) {
	static bool requestInProgress; if (requestInProgress) return NULL; else requestInProgress = true;
	JSTValue result = NULL;
	JSTObject get = (JSTObject) JSTObjectGetProperty(JSTObjectGetProperty(JSTObjectGetPrototype(object), "constructor"), "get");
	if (JSTValueIsFunction(get)) result = JSTFunctionCall(get, object, JSTStringToValue(propertyName, false));
	requestInProgress = false; return (JSTValueIsNull(result))?NULL:result;
}

static JSTDeclareSetProperty(jsNativeContainerSet) {
	static bool requestInProgress; if (requestInProgress) return false; else requestInProgress = true;
 	bool result = false;
	JSTObject set = (JSTObject) JSTObjectGetProperty(JSTObjectGetConstructor(object), "set");
	if (JSTObjectIsFunction(set)) result = JSTValueToBoolean(JSTFunctionCall(set, object, JSTStringToValue(propertyName, false), value));
	requestInProgress = false; return result;
}

// The array of names returned by 'enumerate', must be handled by 'get' in order to show up 
// in the resulting list of enumerated property names.
static JSTDeclareGetPropertyNames(jsNativeContainerEnumerate) {
	static bool requestInProgress; if (requestInProgress) return; else requestInProgress = true;
	void * exception = NULL;
	JSTObject enumerate = (JSTObject) JSTObjectGetProperty(JSTObjectGetConstructor(object), "enumerate");
	if (JSTObjectIsFunction(enumerate)) {
		JSTObject names = (JSTObject) JSTFunctionCall(enumerate, object); // must return numeric indexed pseudo array with length
		register long length = JSTValueToDouble(JSTObjectGetProperty(names, "length")),
		i; JSStringRef name; for (i = 0; i < length; i++) {
		JSPropertyNameAccumulatorAddName(
			propertyNames, (name = JSTValueToString(JSTObjectGetPropertyAtIndex(names, i)))
		);
			JSTStringRelease(name);
		};
	}
	requestInProgress = false;
}

static JSTDeclareHasProperty(jsNativeContainerQuery) {
	static bool requestInProgress; if (requestInProgress) return false; else requestInProgress = true;
	void * exception = NULL; bool result = false;
	JSTObject query = (JSTObject) JSTObjectGetProperty(JSTObjectGetConstructor(object), "query");
	if (JSTObjectIsFunction(query)) result = JSTValueToBoolean(JSTFunctionCall(query, object, JSTStringToValue(propertyName, false)));
	requestInProgress = false; return result;
}

static JSTDeclareFinalizer(jsNativeContainerDeallocate) {
	void * unload = JSTObjectGetPrivate(object);
	if (unload) free(unload), JSTObjectSetPrivate(object, NULL);
}

static JSTValue jsNativeContainer JSTDeclareFunction () {

	JSTObject definition = JSTValueToObject(argv[0]);
	char * name = JSTValueToUTF8(JSTObjectGetProperty(definition, "name"));
	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	jsClass.attributes = JSTClassPropertyManualPrototype, jsClass.className = name;

	// define these properties on your constructor function (passed to this callback)
	// to enable desired custom container features

	if (JSTObjectHasProperty(definition, "convert")) jsClass.convertToType = &jsNativeContainerConvert;
	if (JSTObjectHasProperty(definition, "delete")) jsClass.deleteProperty = &jsNativeContainerDelete;
	if (JSTObjectHasProperty(definition, "get")) jsClass.getProperty = &jsNativeContainerGet;
	if (JSTObjectHasProperty(definition, "set")) jsClass.setProperty = &jsNativeContainerSet;
	if (JSTObjectHasProperty(definition, "enumerate")) jsClass.getPropertyNames = &jsNativeContainerEnumerate;
	if (JSTObjectHasProperty(definition, "query")) jsClass.hasProperty = &jsNativeContainerQuery;
	//if (JSTObjectHasProperty(definition, "deallocate")) jsClass.finalize = &jsNativeContainerDeallocate;
	JSTValue r = JSTValueFromPointer(JSClassRetain(JSClassCreate(&jsClass))); free(name);
	return r;

}

static JSTValue jsNativeInstance JSTDeclareFunction() {
	return JSTClassInstance(JSTValueToPointer(argv[0]), JSTValueToPointer(argv[1]));
}

static JSTValue jsNativeSetPrototype JSTDeclareFunction() {
	JSTObjectSetPrototype((JSTObject)argv[0], argv[1]); return argv[1];
}

static JSTValue jsNativeGetPrototype JSTDeclareFunction() {
	return (JSTValue) JSTObjectGetPrototype((JSTObject)argv[0]);
}

static JSTValue jsNativeSetPrivate JSTDeclareFunction() {
	JSTObjectSetPrivate((JSTObject) argv[0], JSTValueToPointer(argv[1]));
	return JSTValueFromBoolean(1);
}

static JSTValue jsNativeGetPrivate JSTDeclareFunction() {
	return JSTValueFromPointer(JSTObjectGetPrivate((JSTObject)argv[0]));
}

JSTObject JSTNativeInit_ JSTUtility(JSTObject js) {

	JSTObject string = JSTValueToObject(JSTScriptNativeEval("String", NULL));
	JSTObjectSetMethod(string, "toUTF8", jsStringToUTF8, 0);
	JSTObjectSetMethod(string, "fromUTF8", jsStringFromUTF8, 0);
	JSTObjectSetMethod(string, "toUTF32", jsStringToUTF32, 0);

	JSTObject native = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(JSTContextGetGlobalObject(ctx), "native", native, 0);

	JSTObject lib = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "lib", lib, JSTObjectPropertyRequired);

	char * jsNativeDefines = JSTConstructUTF8("native.alignment = %i; native.byteOrder = %i; native.call = {native: %i, ellipsis: %i, '...': %i, x86:{cdecl: %i, win32:{std:%i, fast:{ms: %i, gnu: %i}, 'this':{ms: %i, gnu: %i}}, plan9: %i}, x64:{win64: %i, sysv: %i}, ppc32:{darwin: %i, sysv: %i}, arm:{default: %i, thumb: %i, eabi:{default: %i, thumb: %i}}, mips32:{eabi: %i, o32: %i}, mips64:{n64: %i, n32: %i}};",
		JSTNativeAlignment, JSTNativeByteOrder,
		0, DC_CALL_C_ELLIPSIS, DC_CALL_C_ELLIPSIS, DC_CALL_C_X86_CDECL, DC_CALL_C_X86_WIN32_STD,
		DC_CALL_C_X86_WIN32_FAST_MS, DC_CALL_C_X86_WIN32_FAST_GNU, DC_CALL_C_X86_WIN32_THIS_MS,
		DC_CALL_C_X86_WIN32_THIS_GNU, DC_CALL_C_X86_PLAN9, DC_CALL_C_X64_WIN64, DC_CALL_C_X64_SYSV,
		DC_CALL_C_PPC32_DARWIN, DC_CALL_C_PPC32_SYSV, DC_CALL_C_ARM_ARM, DC_CALL_C_ARM_THUMB,
		DC_CALL_C_ARM_ARM_EABI, DC_CALL_C_ARM_THUMB_EABI ,DC_CALL_C_MIPS32_EABI, DC_CALL_C_MIPS32_O32,
		DC_CALL_C_MIPS64_N64, DC_CALL_C_MIPS64_N32
	);

	JSTScriptNativeEval(jsNativeDefines, NULL);
	free(jsNativeDefines);

	jsNativeDefines = JSTConstructUTF8("native.size = js.extend({}, {'void': %i, 'const': %i, unsigned: %i, bool: %i, char: %i, short: %i, int: %i, 'enum': %i, long: %i, size: %i, pointer: %i, int64: %i, float: %i, double: %i, struct: %i, union: %i, ellipsis: %i, utf8: %i, utf16: %i, utf32: %i, string: %i, value: %i})",
		JSN_C_TYPE_VOID_SIZE, JSN_C_TYPE_CONST_SIZE, JSN_C_TYPE_UNSIGNED_SIZE,
		JSN_C_TYPE_BOOL_SIZE, JSN_C_TYPE_CHAR_SIZE, JSN_C_TYPE_SHORT_SIZE,
		JSN_C_TYPE_INT_SIZE, JSN_C_TYPE_ENUM_SIZE, JSN_C_TYPE_LONG_SIZE,
		JSN_C_TYPE_SIZE_SIZE, JSN_C_TYPE_POINTER_SIZE, JSN_C_TYPE_INT64_SIZE,
		JSN_C_TYPE_FLOAT_SIZE, JSN_C_TYPE_DOUBLE_SIZE, JSN_C_TYPE_STRUCT_SIZE,
		JSN_C_TYPE_UNION_SIZE, JSN_C_TYPE_ELLIPSIS_SIZE, JSN_C_TYPE_UTF8_SIZE,
		JSN_C_TYPE_UTF16_SIZE, JSN_C_TYPE_UTF32_SIZE, JSN_C_TYPE_STRING_SIZE,
		JSN_C_TYPE_VALUE_SIZE
	);

	JSTScriptNativeEval(jsNativeDefines, NULL);
	free(jsNativeDefines);

	if (JSTScriptHasError) JSTScriptReportException(), exit(1);
	
	JSTObjectSetMethod(native, "container", jsNativeContainer, 0);
	JSTObjectSetMethod(native, "instance", jsNativeInstance, 0);
	JSTObjectSetMethod(native, "callback", jsNativeCallback, 0);
	JSTObjectSetMethod(native, "callbackFree", jsNativeCallbackFree, 0);
	JSTObjectSetMethod(native, "setPrototype", jsNativeSetPrototype, 0);
	JSTObjectSetMethod(native, "getPrototype", jsNativeGetPrototype, 0);
	JSTObjectSetMethod(native, "setPrivate", jsNativeSetPrivate, 0);
	JSTObjectSetMethod(native, "getPrivate", jsNativeGetPrivate, 0);
	JSTObjectSetMethod(native, "exec", jsNativeExec, 0);

	JSTObject library = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "library", library, JSTObjectPropertyRequired);

	JSTObjectSetMethod(library, "load", jsNativeLibraryLoad, 0);
	JSTObjectSetMethod(library, "free", jsNativeLibraryFree, 0);
	JSTObjectSetMethod(library, "findSymbol", jsNativeLibraryFindSymbol, 0);

	JSTObject address = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "address", address, JSTObjectPropertyRequired);

	JSTObjectSetMethod(address, "read", jsNativeAddressRead, 0);
	JSTObjectSetMethod(address, "write", jsNativeAddressWrite, 0);
	JSTObjectSetMethod(address, "free", jsNativeAddressFree, 0);

	JSTScriptEval(JSTNativeScript, NULL, "JSTNative.js", 1);
	if (JSTScriptHasError) JSTScriptReportException(), exit(1);

	return native;

}

#endif


