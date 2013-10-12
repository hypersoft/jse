
#define JSTNativeInit(o) JST(JSTNativeInit_, o)

#define JSTNativeAlignment G_MEM_ALIGN
#define JSTNativeByteOrder G_BYTE_ORDER

#define JSTNativeTypeVoid 0
#define JSTNativeTypeUnsigned (1 << 1)
#define JSTNativeTypePointer (1 << 2)
#define JSTNativeTypeEllipsis 3
#define JSTNativeTypeBoolean (1 << 3)
#define JSTNativeTypeChar ((1 << 4) | JSTNativeTypeUnsigned)
#define JSTNativeTypeShort (1 << 5)
#define JSTNativeTypeInt (1 << 6)
#define JSTNativeTypeLong (1 << 7)
#define JSTNativeTypeLongLong (1 << 8)
#define JSTNativeTypeFloat (1 << 9)
#define JSTNativeTypeDouble (1 << 10)
#define JSTNativeTypeSize (1 << 11)
#define JSTNativeTypeUTF8 (1 << 12)
#define JSTNativeTypeUTF16 (1 << 13)
#define JSTNativeTypeUTF32 (1 << 14)
#define JSTNativeTypeStruct (1 << 15)
#define JSTNativeTypeUnion (1 << 16)
#define JSTNativeTypeValue (1 << 17)
#define JSTNativeTypeString (1 << 18)
#define JSTNativeTypeException (1 << 19)

#ifdef JSTools_h

extern JSTObject JSTNativeInit_ JSTUtility(JSTObject js);
extern int JSTNativeTypeGetSize(int t);
extern const char * JSTNativeTypeNameUTF8(int t);

#else

#include "JSTNative.inc"

int JSTNativeTypeGetSize(int t) {
	if (t == 0) return t;
	else if (t & JSTNativeTypePointer
		|| t == JSTNativeTypeValue
		|| t == JSTNativeTypeString
		|| t == JSTNativeTypeException)
	return sizeof(void*);
	else if (t == JSTNativeTypeUnsigned) return sizeof(int);
	else if (t == JSTNativeTypeBoolean) return sizeof(bool);
	else if (t & JSTNativeTypeChar) return sizeof(char);
	else if (t & JSTNativeTypeShort) return sizeof(short);
	else if (t & JSTNativeTypeInt) return sizeof(int);
	else if (t & JSTNativeTypeLong) return sizeof(long);
	else if (t & JSTNativeTypeLongLong) return sizeof(long long);
	else if (t == JSTNativeTypeFloat) return sizeof(float);
	else if (t == JSTNativeTypeDouble) return sizeof(double);
	else if (t & JSTNativeTypeSize) return sizeof(size_t);
	else if (t == JSTNativeTypeUTF8) return sizeof(UTF8);
	else if (t == JSTNativeTypeUTF16) return sizeof(UTF16);
	else if (t == JSTNativeTypeUTF32) return sizeof(UTF32);
	else return 0;
}

const char * JSTNativeTypeNameUTF8(int t) {
	if (t == 0) return "void";
	else if (t & JSTNativeTypePointer
		|| t == JSTNativeTypeValue
		|| t == JSTNativeTypeString
		|| t == JSTNativeTypeException)
	return "pointer";
	else if (t == JSTNativeTypeUnsigned) return "int";
	else if (t == JSTNativeTypeEllipsis) return "ellipsis";
	else if (t == JSTNativeTypeBoolean) return "bool";
	else if (t & JSTNativeTypeChar) return (t & JSTNativeTypeUnsigned)?"char":"schar";
	else if (t & JSTNativeTypeShort) return (t & JSTNativeTypeUnsigned)?"ushort":"short";
	else if (t & JSTNativeTypeInt) return (t & JSTNativeTypeUnsigned)?"uint":"int";
	else if (t & JSTNativeTypeLong) return (t & JSTNativeTypeUnsigned)?"ulong":"long";
	else if (t & JSTNativeTypeLongLong) return (t & JSTNativeTypeUnsigned)?"uint64":"int64";
	else if (t == JSTNativeTypeFloat) return "float";
	else if (t == JSTNativeTypeDouble) return "double";
	else if (t & JSTNativeTypeSize) return (t & JSTNativeTypeUnsigned)?"size":"ssize";
	else if (t == JSTNativeTypeUTF8) return "utf8";
	else if (t == JSTNativeTypeUTF16) return "utf16";
	else if (t == JSTNativeTypeUTF32) return "utf32";
	else return NULL;
}

static JSTContext sigctx = NULL;
static JSTObject sigfn = NULL;
static JSTObject sigob = NULL;

JSTObject JSTNativeCache = NULL;

static void signal_handler (int signum) {
	JSTContext ctx = sigctx; void * exception = NULL;
	if (sigfn) JSTFunctionCall(sigfn, sigob, JSTValueFromDouble(signum));
	else signal(signum, SIG_IGN);
}

static JSTValue jsNativeSignalTrap JSTDeclareFunction(Object obj, Function, fn) {
	if (!sigfn) sigfn = JSTValueToObject(argv[0]), sigob = JSTValueToObject(argv[1]);
	else return JSTValueFromBoolean(false);
	return JSTValueFromBoolean(true);
}

static JSTValue jsNativeTrap JSTDeclareFunction(Number, ...) {
	int number, i = 0, r = 0;
	if (sigctx) {
		while (i < argc)
		if (signal (number = JSTValueToDouble(argv[i++]), signal_handler) == SIG_IGN) signal (number, SIG_IGN);
		else {
			JSTObjectSetPropertyAtIndex(sigob, number, JSTValueUndefined);
			r++;
		}
	}
	return JSTValueFromDouble(r);
}

static JSTValue jsNativeLibraryLoad JSTDeclareFunction(String name) {
	// must have one arg of type String
	char * buffer = JSTStringToUTF8(JSTValueToString(argv[0]), true);
	JSValueRef result = JSTValueFromPointer(dlLoadLibrary(buffer));
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
	char * buffer = JSTStringToUTF8(JSTValueToString(argv[1]), true);
	JSValueRef result = JSTValueFromPointer(dlFindSymbol(JSTValueToPointer(argv[0]), buffer));
	JSTStringFreeUTF8(buffer);
	return result;
}

// This procedure is lacking a plethora of error checking and functional logic
static JSTValue jsNativeExec JSTDeclareFunction(...) {

	JSTValue result = JSTValueNull;

	void * fn = JSTValueToPointer(this); // check that this is a number!
	int returnType = JSTValueToDouble(JSTObjectGetProperty(this, "return"));
	JSTObject proto = JSTValueToObject(JSTObjectGetProperty(this, "proto"));

	int stackSize = JSTValueToDouble(JSTObjectGetProperty(this, "size"));
	int callVMMode = JSTValueToDouble(JSTObjectGetProperty(this, "mode"));

	void * callVM  = dcNewCallVM(stackSize);
	dcMode(callVM, callVMMode);

	int stackItem = 0, stackIndex = 0;
	bool ellipsis = false, unsign = (returnType & JSTNativeTypeUnsigned);

	int protoLen = JSTValueToDouble(JSTObjectGetProperty(proto, "length"));

	while (stackIndex < protoLen) {
		stackItem = JSTValueToDouble(JSTObjectGetPropertyAtIndex(proto, stackIndex));
		if (stackItem > JSTNativeTypeUnsigned) stackItem -= JSTNativeTypeUnsigned;
		JSTValue value = argv[stackIndex++];
		if (stackItem & JSTNativeTypePointer) dcArgPointer(callVM, JSTValueToPointer(value));
		else if (stackItem == JSTNativeTypeBoolean) dcArgBool(callVM, JSTValueToBoolean(value));
		else if (stackItem & JSTNativeTypeChar) dcArgChar(callVM, JSTValueToDouble(value));
		else if (stackItem & JSTNativeTypeShort) dcArgShort(callVM, JSTValueToDouble(value));
		else if (stackItem & JSTNativeTypeInt) dcArgInt(callVM, JSTValueToDouble(value));
		else if (stackItem & JSTNativeTypeLong) dcArgLong(callVM, JSTValueToDouble(value));
		else if (stackItem & JSTNativeTypeLongLong) dcArgLongLong(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeFloat) dcArgFloat(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeDouble) dcArgDouble(callVM, JSTValueToDouble(value));
		else break; // error!
	}

	if (unsign) returnType -= JSTNativeTypeUnsigned;

	if (returnType & JSTNativeTypePointer) result = JSTValueFromPointer(dcCallPointer(callVM, fn));
	else if (returnType == JSTNativeTypeBoolean) result = JSTValueFromBoolean(dcCallBool(callVM, fn));
	else if (returnType & JSTNativeTypeChar) result = JSTValueFromDouble((double) (unsign)?dcCallChar(callVM, fn):(signed char)dcCallChar(callVM, fn));
	else if (returnType & JSTNativeTypeShort) result = JSTValueFromDouble((double) (unsign)?(unsigned short)dcCallShort(callVM, fn):dcCallShort(callVM, fn));
	else if (returnType & JSTNativeTypeInt) result = JSTValueFromDouble((double) (unsign)?(unsigned int)dcCallInt(callVM, fn):dcCallInt(callVM, fn));
	else if (returnType & JSTNativeTypeLong) result = JSTValueFromDouble(dcCallLong(callVM, fn));
	else if (returnType & JSTNativeTypeLongLong) result = JSTValueFromDouble((double) (unsign)?(unsigned long long)dcCallLongLong(callVM, fn):dcCallLongLong(callVM, fn));
	else if (returnType == JSTNativeTypeFloat) result = JSTValueFromDouble(dcCallFloat(callVM, fn));
	else if (returnType == JSTNativeTypeDouble) result = JSTValueFromDouble(dcCallDouble(callVM, fn));
	else if (returnType == JSTNativeTypeVoid) { result = JSTValueUndefined; dcCallVoid(callVM, fn); }
	//else ; // error!

	dcFree(callVM); return result;

}

static JSValueRef jsNativeAddressRead JSTDeclareFunction (address, type) {

	const char * fname = "js.native.address.read";
	const char * job = "unable to read pointer contents";

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
	else if (type == JSTNativeTypeException) return JSTValueFromPointer(*(intptr_t*)(address));
	else if (unsign) return JSTScriptNativeError("%s: %s: `%i' is not a known or signable integer type", fname, job, type);
	else return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

}

static JSValueRef jsNativeAddressWrite JSTDeclareFunction (address, type, value) {

	const char * fname = "js.native.address.write";
	const char * job = "unable to write pointer contents";

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
	else if (type == JSTNativeTypeException) *(intptr_t*)(address) = ((intptr_t)value);
	else if (type & JSTNativeTypeUnsigned) return JSTScriptNativeError("%s: %s: `%i' is not a known or signable integer type", fname, job, type);
	else return JSTScriptNativeError("%s: %s: `%i' is an unknown type", fname, job, type);

	return JSTValueFromBoolean(true);

}

static JSTValue jsNativeTypeSize JSTDeclareFunction(Number type) {
	int t = JSTValueToDouble(argv[0]);
	if (t == 0) return JSTValueFromDouble(0);
	else if (t & JSTNativeTypePointer
		|| t == JSTNativeTypeValue
		|| t == JSTNativeTypeString
		|| t == JSTNativeTypeException)
	return JSTValueFromDouble(sizeof(void*));
	else if (t == JSTNativeTypeUnsigned) return JSTValueFromDouble(sizeof(int));
	else if (t == JSTNativeTypeBoolean) return JSTValueFromDouble(sizeof(bool));
	else if (t & JSTNativeTypeChar) return JSTValueFromDouble(sizeof(char));
	else if (t & JSTNativeTypeShort) return JSTValueFromDouble(sizeof(short));
	else if (t & JSTNativeTypeInt) return JSTValueFromDouble(sizeof(int));
	else if (t & JSTNativeTypeLong) return JSTValueFromDouble(sizeof(long));
	else if (t & JSTNativeTypeLongLong) return JSTValueFromDouble(sizeof(long long));
	else if (t == JSTNativeTypeFloat) return JSTValueFromDouble(sizeof(float));
	else if (t == JSTNativeTypeDouble) return JSTValueFromDouble(sizeof(double));
	else if (t & JSTNativeTypeSize) return JSTValueFromDouble(sizeof(size_t));
	else if (t == JSTNativeTypeUTF8) return JSTValueFromDouble(sizeof(UTF8));
	else if (t == JSTNativeTypeUTF16) return JSTValueFromDouble(sizeof(UTF16));
	else if (t == JSTNativeTypeUTF32) return JSTValueFromDouble(sizeof(UTF32));
	else return JSTValueFromDouble(0);
}

static JSTValue jsNativeToUTF8 JSTDeclareFunction(Value target) {
	// must have one arg of type String
	return JSTValueFromPointer(JSTStringToUTF8(JSTValueToString(argv[0]), true));
}

static JSTValue jsNativeFromUTF8 JSTDeclareFunction(Pointer target) {
	// must have one arg of type char *
	return JSTValueFromString(JSTStringFromUTF8(JSTValueToPointer(argv[0])), true);
}

static JSTValue jsNativeAddressFree JSTDeclareFunction(Pointer target, ...) {
	void *p; JSTObject o; JSTValue v;
	size_t i = 0;
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

static JSTValue jsNativeToUTF32 JSTDeclareFunction(Value target) {
	// must have one arg of type String

	// this is not reliable, buffer is guessed and unchecked! Proof-of-concept

	JSTString jss = JSTValueToString(argv[0]);

	long utf16bytes = JSTStringUTF16Length(jss);
	long utf32bytes = ((utf16bytes) * 2);

	const UTF16 * utf16 = JSTStringUTF16(jss);
	const UTF16 * utf16in = utf16;

	UTF32 * utf32 = malloc(utf32bytes + 4); // pad terminate
	UTF32 * utf32out = utf32;
	
	ConvertUTF16toUTF32(
		&utf16in, utf16in + utf16bytes,
		&utf32out, utf32out + utf32bytes,
		0
	);

	JSTStringRelease(jss);

	*(utf32out) = L'\0';

	return JSTValueFromPointer(utf32);

}

JSTObject JSTNativeInit_ JSTUtility(JSTObject js) {

	sigctx = ctx; // for global signal callback

	JSTObject native = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(js, "native", native, JSTObjectPropertyRequired);

	JSTObject lib = JSTClassInstance(NULL, NULL);

	JSTObjectSetProperty(native, "signalTrap", JSTFunctionCallback("signalTrap", jsNativeSignalTrap), JSTObjectPropertyRequired);
	JSTObjectSetProperty(native, "trap", JSTFunctionCallback("trap", jsNativeTrap), JSTObjectPropertyRequired);

	JSTObjectSetProperty(native, "lib", lib, JSTObjectPropertyRequired);

	JSTObjectSetProperty(native, "alignment", JSTValueFromDouble(G_MEM_ALIGN), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(native, "byteOrder", JSTValueFromDouble(G_BYTE_ORDER), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);

	JSTObjectSetProperty(native, "typeSize", JSTFunctionCallback("typeSize", jsNativeTypeSize), JSTObjectPropertyRequired);
	JSTObjectSetProperty(native, "toUTF8", JSTFunctionCallback("toUTF8", jsNativeToUTF8), JSTObjectPropertyRequired);
	JSTObjectSetProperty(native, "fromUTF8", JSTFunctionCallback("fromUTF8", jsNativeFromUTF8), JSTObjectPropertyRequired);
	JSTObjectSetProperty(native, "toUTF32", JSTFunctionCallback("toUTF32", jsNativeToUTF32), JSTObjectPropertyRequired);
	JSTObjectSetProperty(native, "exec", JSTFunctionCallback("exec", jsNativeExec), JSTObjectPropertyRequired);

	JSTObject library = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "library", library, JSTObjectPropertyRequired);
	JSTObjectSetProperty(library, "load", JSTFunctionCallback("load", jsNativeLibraryLoad), JSTObjectPropertyRequired);
	JSTObjectSetProperty(library, "free", JSTFunctionCallback("free", jsNativeLibraryFree), JSTObjectPropertyRequired);
	JSTObjectSetProperty(library, "findSymbol", JSTFunctionCallback("findSymbol", jsNativeLibraryFindSymbol), JSTObjectPropertyRequired);

	JSTObject address = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "address", address, JSTObjectPropertyRequired);
	JSTObjectSetProperty(address, "read", JSTFunctionCallback("read", jsNativeAddressRead), JSTObjectPropertyRequired);
	JSTObjectSetProperty(address, "write", JSTFunctionCallback("write", jsNativeAddressWrite), JSTObjectPropertyRequired);
	JSTObjectSetProperty(address, "free", JSTFunctionCallback("free", jsNativeAddressFree), JSTObjectPropertyRequired);

	JSTScriptEval(JSTNativeScript, NULL, "JSTNative.js", 1);

	if (*exception) {
		JSTScriptReportException(); exit(1);
	}

	return native;

}

#endif
