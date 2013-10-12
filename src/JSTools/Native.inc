
#define JSTNativeInit(o) JST(JSTNativeInit_, o)

#define JSTNativeAlignment G_MEM_ALIGN
#define JSTNativeByteOrder G_BYTE_ORDER

#define JSTNativeTypeBoolean 1
#define JSTNativeTypeChar 2
#define JSTNativeTypeShort 3
#define JSTNativeTypeInt 4
#define JSTNativeTypeLong 5
#define JSTNativeTypeLongLong 6
#define JSTNativeTypeFloat 7
#define JSTNativeTypeDouble 8
#define JSTNativeTypeUTF8 9
#define JSTNativeTypeUTF16 10
#define JSTNativeTypeUTF32 11
#define JSTNativeTypePointer 12
#define JSTNativeTypeVoid 13
#define JSTNativeTypeStruct 14
#define JSTNativeTypeUnion 15
#define JSTNativeTypeEllipsis 16
#define JSTNativeTypeUnsigned 32

#ifdef JSTools_h

extern JSTObject JSTNativeInit_ JSTUtility(JSTObject js);
extern int JSTNativeTypeSize[JSTNativeTypeVoid];

#else

int JSTNativeTypeSize[] = {
	0, sizeof(bool), sizeof(char), sizeof(short), sizeof(int), sizeof(long),
	sizeof(long long), sizeof(float), sizeof(double), sizeof(char), sizeof(char16_t),
	sizeof(char32_t), sizeof(void*), 0, 0, 0, 0
};

#include "JSTNative.inc"

static JSTContext sigctx = NULL;
static JSTObject sigfn = NULL;
static JSTObject sigob = NULL;

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

static JSTValue jsNativeCallVMCreate JSTDeclareFunction(Number stackSize) {
	// must have first arg of type Number
	return JSTValueFromPointer(dcNewCallVM(JSTValueToDouble(argv[0])));
}

static JSTValue jsNativeCallVMFree JSTDeclareFunction(Pointer callVM) {
	// must have first arg of type Number
	dcFree(JSTValueToPointer(argv[0])); return JSTValueUndefined;
}

static JSTValue jsNativeCallVMGetError JSTDeclareFunction(Pointer callVM) {
	// must have first arg of type Number
	return JSTValueFromDouble(dcGetError(JSTValueToPointer(argv[0])));
}

static JSTValue jsNativeCallVMSetMode JSTDeclareFunction(Pointer callVM, Number mode) {
	// must have two args of type Number
	dcMode(JSTValueToPointer(argv[0]), JSTValueToDouble(argv[1])); return JSTValueUndefined;
}

static JSTValue jsNativeCallVMReset JSTDeclareFunction(Pointer callVM) {
	// must have first arg of type Number
	dcReset(JSTValueToPointer(argv[0])); return JSTValueUndefined;
}

static JSTValue jsNativeCallVMPush JSTDeclareFunction(Pointer callVM, Number[] signature, Number value) {
	// must have first arg of type Number
	// must have second arg of type Array (items must be numbers)
	// must be followed by a total number of Number arguments equal to signature.length
	void * callVM = JSTValueToPointer(argv[0]);
	JSTObject signature = JSTValueToObject(argv[1]);
	int type = 0; size_t sigIndex = 0, sigCount = JSTValueToDouble(JSTObjectGetProperty(signature, "length"));
	while (sigIndex < sigCount) {
		type = JSTValueToDouble(JSTObjectGetPropertyAtIndex(signature, sigIndex++));
		if (type >= JSTNativeTypeUnsigned) type -= JSTNativeTypeUnsigned; // JSTNativeTypeUnsigned is removed, sign does not matter here
		JSTValue value = argv[sigIndex + 1];
		if (type == JSTNativeTypeBoolean) dcArgBool(callVM, JSTValueToBoolean(value));
		else if (type == JSTNativeTypeChar) dcArgChar(callVM, JSTValueToDouble(value));
		else if (type == JSTNativeTypeShort) dcArgShort(callVM, JSTValueToDouble(value));
		else if (type == JSTNativeTypeInt) dcArgInt(callVM, JSTValueToDouble(value));
		else if (type == JSTNativeTypeLong) dcArgLong(callVM, JSTValueToDouble(value));
		else if (type == JSTNativeTypeLongLong) dcArgLongLong(callVM, JSTValueToDouble(value));
		else if (type == JSTNativeTypeFloat) dcArgFloat(callVM, JSTValueToDouble(value));
		else if (type == JSTNativeTypeDouble) dcArgDouble(callVM, JSTValueToDouble(value));
		else if (type == JSTNativeTypePointer) dcArgPointer(callVM, JSTValueToPointer(value));
	}
	return JSTValueFromBoolean(true);
}

static JSTValue jsNativeCallVMCall JSTDeclareFunction(Pointer callVM, Number type, Pointer fn) {
	// must have first arg of type Number
	// must have second arg of type Number
	// must have third arg of type Number
	void * callVM = JSTValueToPointer(argv[0]);
	int type = JSTValueToDouble(argv[1]);
	void * fn = JSTValueToPointer(argv[2]);
	bool unsign = (type >= JSTNativeTypeUnsigned);
	if (unsign) type -= JSTNativeTypeUnsigned;
	if (type == JSTNativeTypeBoolean) return JSTValueFromBoolean(dcCallBool(callVM, fn));
	else if (type == JSTNativeTypeChar) return JSTValueFromDouble((double) (unsign)?dcCallChar(callVM, fn):(signed char)dcCallChar(callVM, fn));
	else if (type == JSTNativeTypeShort) return JSTValueFromDouble((double) (unsign)?(unsigned short)dcCallShort(callVM, fn):dcCallShort(callVM, fn));
	else if (type == JSTNativeTypeInt) return JSTValueFromDouble((double) (unsign)?(unsigned int)dcCallInt(callVM, fn):dcCallInt(callVM, fn));
	else if (type == JSTNativeTypeLong) return JSTValueFromDouble(dcCallLong(callVM, fn));
	else if (type == JSTNativeTypeLongLong) return JSTValueFromDouble((double) (unsign)?(unsigned long long)dcCallLongLong(callVM, fn):dcCallLongLong(callVM, fn));
	else if (type == JSTNativeTypeFloat) return JSTValueFromDouble(dcCallFloat(callVM, fn));
	else if (type == JSTNativeTypeDouble) return JSTValueFromDouble(dcCallDouble(callVM, fn));
	else if (type == JSTNativeTypePointer) return JSTValueFromPointer(dcCallPointer(callVM, fn));
	else if (type == JSTNativeTypeVoid) dcCallVoid(callVM, fn);
	return JSTValueUndefined;

}

// This procedure is lacking a plethora of error checking and functional logic
static JSTValue jsNativeCallVMExec JSTDeclareFunction(Pointer fn, Number mode, Array proto, ...) {

	void * callVM = NULL, * fn = JSTValueToPointer(this); // check that this is a number!
	// int callVMMode = JSTValueToDouble(argv[0]);
	JSTValue result = NULL;
	JSTObject proto = JSTValueToObject(argv[1]);

	int protoLen = JSTValueToDouble(JSTObjectGetProperty(proto, "length"));
	int stackItem = 0, stackSize = 0, stackIndex = 0, returnType = 0;
	bool ellipsis = false, unsign = false;

	while (stackIndex < protoLen) {
		stackItem = JSTValueToDouble(JSTObjectGetPropertyAtIndex(proto, stackIndex++));
		if (ellipsis = (stackItem == JSTNativeTypeEllipsis)) { 
			// need to count & scan argv items here or only support integers
			// DO NOT USE!
			break;
		}
		if (stackItem > JSTNativeTypeUnsigned) stackItem -= JSTNativeTypeUnsigned;
		stackSize += JSTNativeTypeSize[stackItem];
	}

	dcMode(callVM = dcNewCallVM(stackSize), JSTValueToDouble(argv[0])), stackIndex = 0,
	returnType = JSTValueToDouble(JSTObjectGetPropertyAtIndex(proto, stackIndex++)),
	(unsign = (returnType >= JSTNativeTypeUnsigned))?returnType -= JSTNativeTypeUnsigned:0;

	while (stackIndex < protoLen) {
		stackItem = JSTValueToDouble(JSTObjectGetPropertyAtIndex(proto, stackIndex++));
		if (stackItem >= JSTNativeTypeUnsigned) stackItem -= JSTNativeTypeUnsigned;
		JSTValue value = argv[stackIndex];
		if (stackItem == JSTNativeTypeBoolean) dcArgBool(callVM, JSTValueToBoolean(value));
		else if (stackItem == JSTNativeTypeChar) dcArgChar(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeShort) dcArgShort(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeInt) dcArgInt(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeLong) dcArgLong(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeLongLong) dcArgLongLong(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeFloat) dcArgFloat(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypeDouble) dcArgDouble(callVM, JSTValueToDouble(value));
		else if (stackItem == JSTNativeTypePointer) dcArgPointer(callVM, JSTValueToPointer(value));
		else break; // error!
	}

	if (returnType == JSTNativeTypeBoolean) result = JSTValueFromBoolean(dcCallBool(callVM, fn));
	else if (returnType == JSTNativeTypeChar) result = JSTValueFromDouble((double) (unsign)?dcCallChar(callVM, fn):(signed char)dcCallChar(callVM, fn));
	else if (returnType == JSTNativeTypeShort) result = JSTValueFromDouble((double) (unsign)?(unsigned short)dcCallShort(callVM, fn):dcCallShort(callVM, fn));
	else if (returnType == JSTNativeTypeInt) result = JSTValueFromDouble((double) (unsign)?(unsigned int)dcCallInt(callVM, fn):dcCallInt(callVM, fn));
	else if (returnType == JSTNativeTypeLong) result = JSTValueFromDouble(dcCallLong(callVM, fn));
	else if (returnType == JSTNativeTypeLongLong) result = JSTValueFromDouble((double) (unsign)?(unsigned long long)dcCallLongLong(callVM, fn):dcCallLongLong(callVM, fn));
	else if (returnType == JSTNativeTypeFloat) result = JSTValueFromDouble(dcCallFloat(callVM, fn));
	else if (returnType == JSTNativeTypeDouble) result = JSTValueFromDouble(dcCallDouble(callVM, fn));
	else if (returnType == JSTNativeTypePointer) result = JSTValueFromPointer(dcCallPointer(callVM, fn));
	else if (returnType == JSTNativeTypeVoid) { result = JSTValueUndefined; dcCallVoid(callVM, fn); }
	//else ; // error!

	dcFree(callVM); return result;

}

static JSValueRef jsNativeAddressRead JSTDeclareFunction (address, type) {
	// must have first arg of type Number
	// must have second arg of type Number
	void * address = JSTValueToPointer(argv[0]);
	int type = JSTValueToDouble(argv[1]);
	bool unsign = type >= JSTNativeTypeUnsigned; if (unsign) type -= JSTNativeTypeUnsigned;
	if (type == JSTNativeTypeBoolean) return JSTValueFromDouble((double) *(bool*)(address));
	else if (type == JSTNativeTypeChar) return JSTValueFromDouble((double) (unsign)?*(char*)(address):*(signed char*)(address));
	else if (type == JSTNativeTypeShort) return JSTValueFromDouble((double) (unsign)?*(unsigned short*)(address):*(short*)(address));
	else if (type == JSTNativeTypeInt) return JSTValueFromDouble((double) (unsign)?*(unsigned int*)(address):*(int*)(address));
	else if (type == JSTNativeTypeLong) return JSTValueFromDouble((double) (unsign)?*(unsigned long*)(address):*(long*)(address));
	else if (type == JSTNativeTypeLongLong) return JSTValueFromDouble((double) (unsign)?*(unsigned long long*)(address):*(long long*)(address));
	else if (type == JSTNativeTypeFloat) return JSTValueFromDouble((double) *(float*)(address));
	else if (type == JSTNativeTypeDouble) return JSTValueFromDouble((double) *(double*)(address));
	else if (type == JSTNativeTypePointer) return JSTValueFromPointer(*(intptr_t*)(address));
	return NULL;

}

static JSValueRef jsNativeAddressWrite JSTDeclareFunction (address, type, value) {

	// must have first arg of type Number
	// must have second arg of type Number
	// must have third arg of type Number

	void * address = JSTValueToPointer(argv[0]);
	int type = JSTValueToDouble(argv[1]);
	JSValueRef value = argv[2];

	if (type == JSTNativeTypeBoolean) { *(bool*)(address) = JSValueToBoolean(ctx, value); return JSTValueFromBoolean(true); }
	else if (type == JSTNativeTypeChar) { *(char*)(address) = (char) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); }
	else if (type == JSTNativeTypeShort) { *(short*)(address) = (short) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); }
	else if (type == JSTNativeTypeInt) { *(int*)(address) = (int) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); }
	else if (type == JSTNativeTypeLong) { *(long*)(address) = (long) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); } 
	else if (type == JSTNativeTypeLongLong) { *(long long*)(address) = (long long) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); } 
	else if (type == JSTNativeTypeFloat) { *(float*)(address) = (float) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); } 
	else if (type == JSTNativeTypeDouble) { *(double*)(address) = JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); } 
	else if (type == JSTNativeTypePointer) { *(intptr_t*)(address) = (intptr_t)JSTValueToPointer(value); return JSTValueFromBoolean(true); } 
	return JSTValueFromBoolean(false);

}

static JSTValue jsNativeTypeSize JSTDeclareFunction(Number type) {
	unsigned int i = JSTValueToDouble(argv[0]);
	if (i >= JSTNativeTypeVoid) i = 0;
	return JSTValueFromDouble(JSTNativeTypeSize[i]);
}

static JSTValue jsNativeToUTF8 JSTDeclareFunction(Value target) {
	// must have one arg of type String
	return JSTValueFromPointer(JSTStringToUTF8(JSTValueToString(argv[0]), true));
}

static JSTValue jsNativeFromUTF8 JSTDeclareFunction(Pointer target) {
	// must have one arg of type char *
	return JSTValueFromString(JSTStringFromUTF8(JSTValueToPointer(argv[0])), true);
}

static JSTValue jsNativeFree JSTDeclareFunction(Pointer target, ...) {
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
	JSTObjectSetProperty(native, "free", JSTFunctionCallback("free", jsNativeFree), JSTObjectPropertyRequired);

	JSTObject library = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "library", library, JSTObjectPropertyRequired);
	JSTObjectSetProperty(library, "load", JSTFunctionCallback("load", jsNativeLibraryLoad), JSTObjectPropertyRequired);
	JSTObjectSetProperty(library, "free", JSTFunctionCallback("free", jsNativeLibraryFree), JSTObjectPropertyRequired);
	JSTObjectSetProperty(library, "findSymbol", JSTFunctionCallback("findSymbol", jsNativeLibraryFindSymbol), JSTObjectPropertyRequired);

	JSTObject callVM = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "callVM", callVM, JSTObjectPropertyRequired);
	JSTObjectSetProperty(callVM, "create", JSTFunctionCallback("create", jsNativeCallVMCreate), JSTObjectPropertyRequired);
	JSTObjectSetProperty(callVM, "free", JSTFunctionCallback("free", jsNativeCallVMFree), JSTObjectPropertyRequired);
	JSTObjectSetProperty(callVM, "getError", JSTFunctionCallback("getError", jsNativeCallVMGetError), JSTObjectPropertyRequired);
	JSTObjectSetProperty(callVM, "setMode", JSTFunctionCallback("setMode", jsNativeCallVMSetMode), JSTObjectPropertyRequired);
	JSTObjectSetProperty(callVM, "reset", JSTFunctionCallback("reset", jsNativeCallVMReset), JSTObjectPropertyRequired);
	JSTObjectSetProperty(callVM, "push", JSTFunctionCallback("push", jsNativeCallVMPush), JSTObjectPropertyRequired);
	JSTObjectSetProperty(callVM, "call", JSTFunctionCallback("call", jsNativeCallVMCall), JSTObjectPropertyRequired);
	JSTObjectSetProperty(callVM, "exec", JSTFunctionCallback("exec", jsNativeCallVMExec), JSTObjectPropertyRequired);

	JSTObject address = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "address", address, JSTObjectPropertyRequired);
	JSTObjectSetProperty(address, "read", JSTFunctionCallback("read", jsNativeAddressRead), JSTObjectPropertyRequired);
	JSTObjectSetProperty(address, "write", JSTFunctionCallback("write", jsNativeAddressWrite), JSTObjectPropertyRequired);

	JSTScriptEval(JSTNativeScript, NULL, "JSTNative.js", 1);

	return native;

}

#endif
