
#define JSTNativeInit(o) JST(JSTNativeInit_, o)

#define JSTNativeAlignment G_MEM_ALIGN
#define JSTNativeByteOrder G_BYTE_ORDER

#define JSTNativeTypeVoid 0
#define JSTNativeTypeUnsigned (1 << 0)
#define JSTNativeTypePointer (1 << 1)
#define JSTNativeTypeEllipsis 3
#define JSTNativeTypeBoolean (1 << 2)
#define JSTNativeTypeChar ((1 << 3) | JSTNativeTypeUnsigned)
#define JSTNativeTypeShort (1 << 4)
#define JSTNativeTypeInt (1 << 5)
#define JSTNativeTypeLong (1 << 6)
#define JSTNativeTypeLongLong (1 << 7)
#define JSTNativeTypeSize (1 << 8)
#define JSTNativeTypeFloat (1 << 9)
#define JSTNativeTypeDouble (1 << 10)
#define JSTNativeTypeUTF8 (1 << 11)
#define JSTNativeTypeUTF16 (1 << 12)
#define JSTNativeTypeUTF32 (1 << 13)
#define JSTNativeTypeValue (1 << 14)
#define JSTNativeTypeString (1 << 15)
#define JSTNativeTypeStruct (1 << 16)
#define JSTNativeTypeUnion (1 << 17)

#ifdef JSTools_h

extern JSTObject JSTNativeInit_ JSTUtility(JSTObject js);
extern int JSTNativeTypeGetSize(int t);
extern const char * JSTNativeTypeGetName(int t);

#else

#include "JSTNative.inc"

int JSTNativeTypeGetSize(int t) {
	if (t == 0) return t;
	else if (t & JSTNativeTypePointer || t == JSTNativeTypeValue || t == JSTNativeTypeString ) return sizeof(void*);
	else if (t == JSTNativeTypeBoolean) return sizeof(bool);
	else if (t & JSTNativeTypeChar) return sizeof(char);
	else if (t & JSTNativeTypeShort) return sizeof(short);
	else if (t & JSTNativeTypeInt || t == JSTNativeTypeUnsigned) return sizeof(int);
	else if (t & JSTNativeTypeLong) return sizeof(long);
	else if (t & JSTNativeTypeLongLong) return sizeof(long long);
	else if (t & JSTNativeTypeSize) return sizeof(size_t);
	else if (t == JSTNativeTypeFloat) return sizeof(float);
	else if (t == JSTNativeTypeDouble) return sizeof(double);
	else if (t == JSTNativeTypeUTF8) return sizeof(UTF8);
	else if (t == JSTNativeTypeUTF16) return sizeof(UTF16);
	else if (t == JSTNativeTypeUTF32) return sizeof(UTF32);
	else return 0;
}

const char * JSTNativeTypeGetName(int t) {
	if (t == 0) return "void";
	else if (t & JSTNativeTypePointer || t == JSTNativeTypeValue || t == JSTNativeTypeString ) return "pointer";
	else if (t == JSTNativeTypeEllipsis) return "ellipsis";
	else if (t == JSTNativeTypeBoolean) return "bool";
	else if (t & JSTNativeTypeChar) return (t & JSTNativeTypeUnsigned)?"char":"schar";
	else if (t & JSTNativeTypeShort) return (t & JSTNativeTypeUnsigned)?"ushort":"short";
	else if (t & JSTNativeTypeInt || t == JSTNativeTypeUnsigned) return (t & JSTNativeTypeUnsigned)?"uint":"int";
	else if (t & JSTNativeTypeLong) return (t & JSTNativeTypeUnsigned)?"ulong":"long";
	else if (t & JSTNativeTypeLongLong) return (t & JSTNativeTypeUnsigned)?"uint64":"int64";
	else if (t & JSTNativeTypeSize) return (t & JSTNativeTypeUnsigned)?"size":"ssize";
	else if (t == JSTNativeTypeFloat) return "float";
	else if (t == JSTNativeTypeDouble) return "double";
	else if (t == JSTNativeTypeUTF8) return "utf8";
	else if (t == JSTNativeTypeUTF16) return "utf16";
	else if (t == JSTNativeTypeUTF32) return "utf32";
	else return NULL;
}

char JSTNativeTypeGetChar(int t) {
	if (t & JSTNativeTypePointer || t == JSTNativeTypeValue || t == JSTNativeTypeString ) return 'p';
	else if (t == JSTNativeTypeBoolean) return 'B';
	else if (t & JSTNativeTypeChar) return (t & JSTNativeTypeUnsigned)?'C':'c';
	else if (t & JSTNativeTypeShort) return (t & JSTNativeTypeUnsigned)?'S':'s';
	else if (t & JSTNativeTypeInt || t == JSTNativeTypeUnsigned) return (t & JSTNativeTypeUnsigned)?'I':'i';
	else if (t & JSTNativeTypeLong) return (t & JSTNativeTypeUnsigned)?'J':'j';
	else if (t & JSTNativeTypeLongLong) return (t & JSTNativeTypeUnsigned)?'L':'l';
	else if (t & JSTNativeTypeSize) return (t & JSTNativeTypeUnsigned)?'J':'j';
	else if (t == JSTNativeTypeFloat) return 'f';
	else if (t == JSTNativeTypeDouble) return 'd';
	else if (t == JSTNativeTypeUTF8) return 'C';
	else if (t == JSTNativeTypeUTF16) return 'S';
	else if (t == JSTNativeTypeUTF32) return 'J';
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
		if (t & JSTNativeTypePointer || t == JSTNativeTypeValue || t == JSTNativeTypeString)
			argv[i] = JSTValueFromPointer(dcbArgPointer(args));
		else if (t & JSTNativeTypeSize) argv[i] = JSTValueFromDouble((t & JSTNativeTypeUnsigned)?dcbArgULong(args):dcbArgLong(args));
		else if (t & JSTNativeTypeLong) argv[i] = JSTValueFromDouble((t & JSTNativeTypeUnsigned)?dcbArgULong(args):dcbArgLong(args));
		else if (t & JSTNativeTypeInt || t == JSTNativeTypeUnsigned) argv[i] = JSTValueFromDouble((t & JSTNativeTypeUnsigned)?dcbArgUInt(args):dcbArgInt(args));
		else if (t == JSTNativeTypeDouble) argv[i] = JSTValueFromDouble(dcbArgDouble(args));
		else if (t == JSTNativeTypeBoolean) argv[i] = JSTValueFromBoolean(dcbArgBool(args));
		else if (t & JSTNativeTypeChar) argv[i] = JSTValueFromDouble((t & JSTNativeTypeUnsigned)?dcbArgUChar(args):dcbArgChar(args));
		else if (t & JSTNativeTypeShort) argv[i] = JSTValueFromDouble((t & JSTNativeTypeUnsigned)?dcbArgUShort(args):dcbArgShort(args));
		else if (t & JSTNativeTypeLongLong) argv[i] = JSTValueFromDouble((t & JSTNativeTypeUnsigned)?dcbArgULongLong(args):dcbArgLongLong(args));
		else if (t == JSTNativeTypeFloat) argv[i] = JSTValueFromDouble(dcbArgFloat(args));
		else if (t == JSTNativeTypeUTF8) argv[i] = JSTValueFromDouble(dcbArgUChar(args));
		else if (t == JSTNativeTypeUTF16) argv[i] = JSTValueFromDouble(dcbArgUShort(args));
		else if (t == JSTNativeTypeUTF32) argv[i] = JSTValueFromDouble(dcbArgULong(args));
		else {
			JSTScriptNativeError("jsNativeCallbackHandler: argument %i: is an unknown type", i);
			JSTScriptReportException(), exit(1);
		}
		if (JSTScriptHasError) JSTScriptReportException(), exit(1);
	}

	JSTValue value = JST(JSObjectCallAsFunction, cbs->function, cbs->object, cbs->length, argv);

	if (JSTScriptHasError) JSTScriptReportException(), exit(1);

	t = cbs->type;
	if (t & JSTNativeTypePointer || t == JSTNativeTypeValue || t == JSTNativeTypeString) {
		result->p = JSTValueToPointer(value); return 'p';
	} else if (t & JSTNativeTypeSize) {result->j = JSTValueToDouble(value); return 'j';}
	else if (t & JSTNativeTypeLong) {result->j =JSTValueToDouble(value); return 'j';}
	else if (t & JSTNativeTypeInt || t == JSTNativeTypeUnsigned) {result->i = JSTValueToDouble(value); return 'i';}
	else if (t == JSTNativeTypeDouble) {result->d = JSTValueToDouble(value); return 'd';}
	else if (t == JSTNativeTypeBoolean) { result->B = JSTValueToBoolean(value); return 'B'; }
	else if (t & JSTNativeTypeChar) {result->c = JSTValueToDouble(value); return 'c';}
	else if (t & JSTNativeTypeShort) {result->s = JSTValueToDouble(value); return 's';}
	else if (t & JSTNativeTypeLongLong) {result->l = JSTValueToDouble(value); return 'l';}
	else if (t == JSTNativeTypeFloat) {result->f = JSTValueToDouble(value); return 'f';}
	else if (t == JSTNativeTypeUTF8) {result->C = JSTValueToDouble(value); return 'C';}
	else if (t == JSTNativeTypeUTF16) {result->S = JSTValueToDouble(value); return 'S';}
	else if (t == JSTNativeTypeUTF32) {result->J = JSTValueToDouble(value); return 'J';}
	else {return 'v';}

}

static JSTValue jsNativeCallback JSTDeclareFunction() {

	jsNativeCallbackStruct * cbs = calloc(1, sizeof(jsNativeCallbackStruct));

	cbs->group = (void*)JSContextGetGroup(ctx);
	cbs->object = (void*)argv[0];
	cbs->function = (void*) argv[1];
	cbs->type = JSTValueToDouble(JSTObjectGetPropertyAtIndex(argv[2], 0));
	cbs->proto = (void*)JSTScriptNativeEval("this.slice(1)", (JSTObject) argv[2]);
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

	const char * fname = "js.native.exec", * job = "type error";

	if (argc != protoLen) {
		return JSTScriptNativeError("%s: %s: argument count does not match function prototype", fname, job);
	}

	int
		callVMMode = JSTValueToDouble(JSTObjectGetProperty(this, "mode")),
		resource = 0, resource16 = 0,
		returnType = JSTValueToDouble(JSTObjectGetProperty(this, "return")),
		stackSize = JSTValueToDouble(JSTObjectGetProperty(this, "size"));

	void 
		* callVM  = dcNewCallVM(stackSize),
		* conversion[argc], * conversion16[argc], 
		* fn = JSTValueToPointer(this);

	if (dcMode(callVM, callVMMode), dcGetError(callVM) == DC_ERROR_UNSUPPORTED_MODE) {
		result = JSTScriptNativeError("%s: %s: `%i' is an invalid or unknown virtual call mode", fname, job, callVMMode);
		goto bail;
	}

	int stackItem = 0, stackIndex = 0;
	bool ellipsis = false, unsign = (returnType & JSTNativeTypeUnsigned);
	while (stackIndex < protoLen) {
		stackItem = JSTValueToDouble(JSTObjectGetPropertyAtIndex(proto, stackIndex));
		JSTValue value = argv[stackIndex++];
		if (stackItem & JSTNativeTypeUTF8) {
			if (stackItem & JSTNativeTypePointer) {
				/* conversion required? */
				if (JSTValueIsNumber(value)) dcArgPointer(callVM, JSTValueToPointer(value));
				else dcArgPointer(callVM, conversion[resource++] = JSTValueToUTF8(value));
			} else dcArgChar(callVM, JSTValueToDouble(value));
		} else if (stackItem & JSTNativeTypeUTF16) {
			if (stackItem & JSTNativeTypePointer) {
				/* conversion required? */
				if (JSTValueIsNumber(value)) dcArgPointer(callVM, JSTValueToPointer(value));
				else dcArgPointer(callVM, (void*)JSTStringUTF16(conversion16[resource16++] = JSTValueToString(value)));
			} else dcArgShort(callVM, JSTValueToDouble(value));
		} else if (stackItem & JSTNativeTypeUTF32) {
			if (stackItem & JSTNativeTypePointer) {
				/* conversion required? */
				if (JSTValueIsNumber(value)) dcArgPointer(callVM, JSTValueToPointer(value));
				else dcArgPointer(callVM, conversion[resource++] = JSTStringToUTF32(JSTValueToString(value), JSTValueToDouble(JSTObjectGetProperty(value, "length")), true));
			} else dcArgLong(callVM, JSTValueToDouble(value));
		} else if (stackItem & JSTNativeTypePointer) dcArgPointer(callVM, JSTValueToPointer(value));
		else if (stackItem == JSTNativeTypeBoolean) dcArgBool(callVM, JSTValueToBoolean(value));
		else if (stackItem & JSTNativeTypeChar) dcArgChar(callVM, JSTValueToDouble(value));
		else if (stackItem & JSTNativeTypeShort) dcArgShort(callVM, JSTValueToDouble(value));
		else if (stackItem & JSTNativeTypeInt) dcArgInt(callVM, JSTValueToDouble(value));
		else if (stackItem & JSTNativeTypeLong) dcArgLong(callVM, JSTValueToDouble(value));
		else if (stackItem & JSTNativeTypeLongLong) dcArgLongLong(callVM, JSTValueToDouble(value));
		else if (stackItem & JSTNativeTypeSize) dcArgLong(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeFloat) dcArgFloat(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeDouble) dcArgDouble(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeValue) dcArgPointer(callVM, (void*) value);
		else if (stackItem == JSTNativeTypeString) dcArgPointer(callVM, (void*)(conversion16[resource16++] = JSTValueToString(value)));
		else {
			result = JSTScriptNativeError("%s: %s: argument #%i (%i) is an unknown or invalid argument type", fname, job, stackIndex, stackItem);
			goto bail;
		}
	}

	if (unsign) returnType -= JSTNativeTypeUnsigned;
	if (returnType & JSTNativeTypePointer) result = JSTValueFromPointer(dcCallPointer(callVM, fn));
	else if (returnType == JSTNativeTypeBoolean) result = JSTValueFromBoolean(dcCallBool(callVM, fn));
	else if (returnType & JSTNativeTypeChar) result = JSTValueFromDouble((double) (unsign)?dcCallChar(callVM, fn):(signed char)dcCallChar(callVM, fn));
	else if (returnType & JSTNativeTypeShort) result = JSTValueFromDouble((double) (unsign)?(unsigned short)dcCallShort(callVM, fn):dcCallShort(callVM, fn));
	else if (returnType & JSTNativeTypeInt) result = JSTValueFromDouble((double) (unsign)?(unsigned int)dcCallInt(callVM, fn):dcCallInt(callVM, fn));
	else if (returnType & JSTNativeTypeLong) result = JSTValueFromDouble(dcCallLong(callVM, fn));
	else if (returnType & JSTNativeTypeLongLong) result = JSTValueFromDouble((double) (unsign)?(unsigned long long)dcCallLongLong(callVM, fn):dcCallLongLong(callVM, fn));
	else if (returnType & JSTNativeTypeSize) result = JSTValueFromDouble(dcCallLong(callVM, fn));
	else if (returnType == JSTNativeTypeFloat) result = JSTValueFromDouble(dcCallFloat(callVM, fn));
	else if (returnType == JSTNativeTypeDouble) result = JSTValueFromDouble(dcCallDouble(callVM, fn));
	else if (returnType == JSTNativeTypeVoid) { result = JSTValueUndefined; dcCallVoid(callVM, fn); }
	else if (returnType == JSTNativeTypeValue) result = (JSTValue) dcCallPointer(callVM, fn);
	else if (returnType == JSTNativeTypeString) result =  JSTValueFromPointer(dcCallPointer(callVM, fn));
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

	const char * fname = "js.native.address.read", * job = "unable to read pointer contents";

	if (argc != 2) return JSTScriptNativeError("%s: %s: %s arguments", fname, job, (argc > 2)?"too many":"insufficient");

	// must have first arg of type Number
	// must have second arg of type Number
	void * address = JSTValueToPointer(argv[0]);
	if (! address ) return JSTScriptNativeError("%s: %s: pointer value is null", fname, job);

	int type = JSTValueToDouble(argv[1]); bool unsign = (type & JSTNativeTypeUnsigned);

	if (type & JSTNativeTypePointer) return JSTValueFromPointer(*(intptr_t*)(address));
	else if (type == JSTNativeTypeBoolean) return JSTValueFromDouble((double) *(bool*)(address));
	else if (type & JSTNativeTypeChar) return JSTValueFromDouble((double) (unsign)?*(char*)(address):*(signed char*)(address));
	else if (type & JSTNativeTypeShort) return JSTValueFromDouble((double) (unsign)?*(unsigned short*)(address):*(short*)(address));
	else if (type & JSTNativeTypeInt || type == JSTNativeTypeUnsigned) return JSTValueFromDouble((double) (unsign)?*(unsigned int*)(address):*(int*)(address));
	else if (type & JSTNativeTypeLong) return JSTValueFromDouble((double) (unsign)?*(unsigned long*)(address):*(long*)(address));
	else if (type & JSTNativeTypeLongLong) return JSTValueFromDouble((double) (unsign)?*(unsigned long long*)(address):*(long long*)(address));
	else if (type & JSTNativeTypeSize) return JSTValueFromDouble((double) (unsign)?*(size_t*)(address):*(ssize_t*)(address));
	else if (type == JSTNativeTypeFloat) return JSTValueFromDouble((double) *(float*)(address));
	else if (type == JSTNativeTypeDouble) return JSTValueFromDouble((double) *(double*)(address));
	else if (type == JSTNativeTypeUTF8) return JSTValueFromDouble((double) *(UTF8*)(address));
	else if (type == JSTNativeTypeUTF16) return JSTValueFromDouble((double) *(UTF16*)(address));
	else if (type == JSTNativeTypeUTF32) return JSTValueFromDouble((double) *(UTF32*)(address));
	else if (type == JSTNativeTypeValue) return ((JSTValue)*(intptr_t*)(address));
	else if (type == JSTNativeTypeString) return JSTValueFromPointer(*(intptr_t*)(address));
	else if (unsign) return JSTScriptNativeError("%s: %s: `%i' is not a known or signable integer type", fname, job, type);
	else return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

}

static JSTValue jsNativeAddressWrite JSTDeclareFunction (address, type, value) {

	const char * fname = "js.native.address.write", * job = "unable to write pointer contents";

	if (argc != 3) return JSTScriptNativeError("%s: %s: %s arguments", fname, job, (argc > 2)?"too many":"insufficient");

	// must have first arg of type Number
	// must have second arg of type Number
	// must have third arg of type Number
	void * address = JSTValueToPointer(argv[0]);
	if (! address ) return JSTScriptNativeError("%s: %s: pointer value is null", fname, job);

	int type = JSTValueToDouble(argv[1]);
	double value = JSTValueToDouble(argv[2]);

	if (type & JSTNativeTypePointer) *(intptr_t*)(address) = (intptr_t)value;
	else if (type == JSTNativeTypeBoolean) *(bool*)(address) = (bool) value; 
	else if (type & JSTNativeTypeChar) *(char*)(address) = (char) value;
	else if (type & JSTNativeTypeShort) *(short*)(address) = (short) value;
	else if (type & JSTNativeTypeInt || type == JSTNativeTypeUnsigned) *(int*)(address) = (int) value;
	else if (type & JSTNativeTypeLong) *(long*)(address) = (long) value;
	else if (type & JSTNativeTypeLongLong) *(long long*)(address) = (long long) value;
	else if (type & JSTNativeTypeSize) *(size_t*)(address) = (size_t) value;
	else if (type == JSTNativeTypeFloat) *(float*)(address) = (float) value;
	else if (type == JSTNativeTypeDouble) *(double*)(address) = value;
	else if (type == JSTNativeTypeUTF8) *(UTF8*)(address) = (UTF8) value;
	else if (type == JSTNativeTypeUTF16) *(UTF16*)(address) = (UTF16) value;
	else if (type == JSTNativeTypeUTF32) *(UTF32*)(address) = (UTF32) value;
	else if (type == JSTNativeTypeValue) *(intptr_t*)(address) = ((intptr_t)value);
	else if (type == JSTNativeTypeString) *(intptr_t*)(address) = ((intptr_t)value);
	else if (type & JSTNativeTypeUnsigned) return JSTScriptNativeError("%s: %s: `%i' is not a known or signable integer type", fname, job, type);
	else return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

	return JSTValueFromBoolean(true);

}

static JSTValue jsNativeTypeSize JSTDeclareFunction(Number type) {
	int t = JSTValueToDouble(argv[0]);
	if (t == 0) return JSTValueFromDouble(0); else if (
		t & JSTNativeTypePointer
		|| t == JSTNativeTypeValue
		|| t == JSTNativeTypeString
	) return JSTValueFromDouble(sizeof(void*));
	else if (t == JSTNativeTypeUnsigned) return JSTValueFromDouble(sizeof(int));
	else if (t == JSTNativeTypeBoolean) return JSTValueFromDouble(sizeof(bool));
	else if (t & JSTNativeTypeChar) return JSTValueFromDouble(sizeof(char));
	else if (t & JSTNativeTypeShort) return JSTValueFromDouble(sizeof(short));
	else if (t & JSTNativeTypeInt) return JSTValueFromDouble(sizeof(int));
	else if (t & JSTNativeTypeLong) return JSTValueFromDouble(sizeof(long));
	else if (t & JSTNativeTypeLongLong) return JSTValueFromDouble(sizeof(long long));
	else if (t & JSTNativeTypeSize) return JSTValueFromDouble(sizeof(size_t));
	else if (t == JSTNativeTypeFloat) return JSTValueFromDouble(sizeof(float));
	else if (t == JSTNativeTypeDouble) return JSTValueFromDouble(sizeof(double));
	else if (t == JSTNativeTypeUTF8) return JSTValueFromDouble(sizeof(UTF8));
	else if (t == JSTNativeTypeUTF16) return JSTValueFromDouble(sizeof(UTF16));
	else if (t == JSTNativeTypeUTF32) return JSTValueFromDouble(sizeof(UTF32));
	else return JSTValueFromDouble(0);
}

static JSTValue jsNativeToUTF8 JSTDeclareFunction(Value target) {
	// must have one arg of type String
	return JSTValueFromPointer(JSTValueToUTF8(argv[0]));
}

static JSTValue jsNativeFromUTF8 JSTDeclareFunction(Pointer target) {
	// must have one arg of type char *
	return JSTValueFromUTF8(JSTValueToPointer(argv[0]));
}

static JSTValue jsNativeToUTF32 JSTDeclareFunction(Value target) {
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

JSTObject JSTNativeInit_ JSTUtility(JSTObject js) {

	JSTObject native = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(js, "native", native, JSTObjectPropertyRequired);

	JSTObject lib = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "lib", lib, JSTObjectPropertyRequired);

	char * jsNativeTypes = JSTConstructUTF8("js.signal = {fpe: %i, ill: %i, segv: %i, bus: %i, abrt: %i, iot: %i, trap: %i, sys: %i, term: %i, int: %i, quit: %i, kill: %i, hup: %i, alrm: %i, vtalrm: %i, prof: %i, io: %i, urg: %i, poll: %i, chld: %i, cld: %i, cont: %i, stop: %i, ttin: %i, ttou: %i, pipe: %i, xcpu: %i, xfsz: %i, usr1: %i, usr2: %i, winch: %i}; js.native.alignment = %i; js.native.byteOrder = %i; js.call = {native: %i, ellipsis: %i, '...': %i, x86:{cdecl: %i, win32:{std:%i, fast:{ms: %i, gnu: %i}, 'this':{ms: %i, gnu: %i}}, plan9: %i}, x64:{win64: %i, sysv: %i}, ppc32:{darwin: %i, sysv: %i}, arm:{default: %i, thumb: %i, eabi:{default: %i, thumb: %i}}, mips32:{eabi: %i, o32: %i}, mips64:{n64: %i, n32: %i}}; Object.freeze(js.call); js.type = { 'void': %i, unsigned: %i, pointer: %i, ellipsis: %i, boolean: %i, char: %i, short: %i, int: %i, long: %i, int64: %i, float: %i, double: %i, size: %i, utf8: %i, utf16: %i, utf32: %i, struct: %i, union: %i, value: %i, string: %i};",
		SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT, SIGIOT, SIGTRAP, SIGSYS, SIGTERM, SIGINT, SIGQUIT, SIGKILL, SIGHUP, SIGALRM, SIGVTALRM, SIGPROF, SIGIO, SIGURG, SIGPOLL, SIGCHLD, SIGCLD, SIGCONT, SIGSTOP, SIGTTIN, SIGTTOU, SIGPIPE, SIGXCPU, SIGXFSZ, SIGUSR1, SIGUSR2, SIGWINCH, 
		JSTNativeAlignment, JSTNativeByteOrder,
		0, DC_CALL_C_ELLIPSIS, DC_CALL_C_ELLIPSIS, DC_CALL_C_X86_CDECL, DC_CALL_C_X86_WIN32_STD,
		DC_CALL_C_X86_WIN32_FAST_MS, DC_CALL_C_X86_WIN32_FAST_GNU, DC_CALL_C_X86_WIN32_THIS_MS,
		DC_CALL_C_X86_WIN32_THIS_GNU, DC_CALL_C_X86_PLAN9, DC_CALL_C_X64_WIN64, DC_CALL_C_X64_SYSV,
		DC_CALL_C_PPC32_DARWIN, DC_CALL_C_PPC32_SYSV, DC_CALL_C_ARM_ARM, DC_CALL_C_ARM_THUMB,
		DC_CALL_C_ARM_ARM_EABI, DC_CALL_C_ARM_THUMB_EABI ,DC_CALL_C_MIPS32_EABI, DC_CALL_C_MIPS32_O32,
		DC_CALL_C_MIPS64_N64, DC_CALL_C_MIPS64_N32,
		0, JSTNativeTypeUnsigned, JSTNativeTypePointer, JSTNativeTypeEllipsis, JSTNativeTypeBoolean,
		JSTNativeTypeChar, JSTNativeTypeShort, JSTNativeTypeInt, JSTNativeTypeLong,
		JSTNativeTypeLongLong, JSTNativeTypeFloat, JSTNativeTypeDouble, JSTNativeTypeSize,
		JSTNativeTypeUTF8, JSTNativeTypeUTF16,JSTNativeTypeUTF32, JSTNativeTypeStruct,
		JSTNativeTypeUnion, JSTNativeTypeValue, JSTNativeTypeString
	);	// supplemental type aliases are also provided in JSTNative.js

	JSTScriptNativeEval(jsNativeTypes, NULL); free(jsNativeTypes);
	if (JSTScriptHasError) JSTScriptReportException(), exit(1);
	
	JSTObjectSetMethod(native, "container", jsNativeContainer, 0);
	JSTObjectSetMethod(native, "instance", jsNativeInstance, 0);
	JSTObjectSetMethod(native, "callback", jsNativeCallback, 0);
	JSTObjectSetMethod(native, "callbackFree", jsNativeCallbackFree, 0);
	JSTObjectSetMethod(native, "setPrototype", jsNativeSetPrototype, 0);
	JSTObjectSetMethod(native, "getPrototype", jsNativeGetPrototype, 0);
	JSTObjectSetMethod(native, "setPrivate", jsNativeSetPrivate, 0);
	JSTObjectSetMethod(native, "typeSize", jsNativeTypeSize, 0);
	JSTObjectSetMethod(native, "toUTF8", jsNativeToUTF8, 0);
	JSTObjectSetMethod(native, "fromUTF8", jsNativeFromUTF8, 0);
	JSTObjectSetMethod(native, "toUTF32", jsNativeToUTF32, 0);
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


