
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
#define JSTNativeTypeVoid 12
#define JSTNativeTypeUnsigned 16

#ifndef JSTools_h

JSTValue jsNativeLibraryLoad JSTDeclareFunction(String name) {
	char * buffer = JSTStringToUTF8(JSTValueToString(argv[0]), true);
	JSValueRef result = JSTValueFromDouble((unsigned long)dlLoadLibrary(buffer));
	JSTStringFreeUTF8(buffer);
	return result;
}

JSTValue jsNativeLibraryFree JSTDeclareFunction(Pointer library) {
	dlFreeLibrary(JSTValueToPointer(argv[0])); return JSTValueUndefined;
}

JSTValue jsNativeLibraryFindSymbol JSTDeclareFunction(Pointer library, String symbol) {
	char * buffer = JSTStringToUTF8(JSTValueToString(argv[1]), true);
	JSValueRef result = JSTValueFromDouble((unsigned long)dlFindSymbol(JSTValueToPointer(argv[0]), buffer));
	JSTStringFreeUTF8(buffer);
	return result;
}

JSTValue jsNativeCallVMCreate JSTDeclareFunction(Number stackSize) {
	return JSTValueFromDouble((unsigned long)dcNewCallVM(JSTValueToDouble(argv[0])));
}

JSTValue jsNativeCallVMFree JSTDeclareFunction(Pointer callVM) {
	dcFree(JSTValueToPointer(argv[0])); return JSTValueUndefined;
}

JSTValue jsNativeCallVMGetError JSTDeclareFunction(Pointer callVM) {
	return JSTValueFromDouble(dcGetError(JSTValueToPointer(argv[0])));
}

JSTValue jsNativeCallVMSetMode JSTDeclareFunction(Pointer callVM, Number mode) {
	dcMode(JSTValueToPointer(argv[0]), JSTValueToDouble(argv[1])); return JSTValueUndefined;
}

JSTValue jsNativeCallVMReset JSTDeclareFunction(Pointer callVM) {
	dcReset(JSTValueToPointer(argv[0])); return JSTValueUndefined;
}

JSTValue jsNativeCallVMPush JSTDeclareFunction(Pointer callVM, Number[] signature, ...) {
	void * callVM = JSTValueToPointer(argv[0]);
	JSTObject signature = JSTValueToObject(argv[1]);
	int type = 0; size_t sigIndex = 0, sigCount = JSTValueToDouble(JSTObjectGetProperty(signature, "length"));
	while (sigIndex < sigCount) {
		type = JSTValueToDouble(JSTObjectGetPropertyAtIndex(signature, sigIndex++));
		if (type >= 16) type -= 16; // 16 is removed, sign does not matter here
		JSTValue value = argv[sigIndex + 1];
		if (type & JSTNativeTypeBoolean) dcArgBool(callVM, JSTValueToBoolean(value));
		else if (type & JSTNativeTypeChar) dcArgChar(callVM, JSTValueToDouble(value));
		else if (type & JSTNativeTypeShort) dcArgShort(callVM, JSTValueToDouble(value));
		else if (type & JSTNativeTypeInt) dcArgInt(callVM, JSTValueToDouble(value));
		else if (type & JSTNativeTypeLong) dcArgLong(callVM, JSTValueToDouble(value));
		else if (type & JSTNativeTypeLongLong) dcArgLongLong(callVM, JSTValueToDouble(value));
		else if (type & JSTNativeTypeFloat) dcArgFloat(callVM, JSTValueToDouble(value));
		else if (type & JSTNativeTypeDouble) dcArgDouble(callVM, JSTValueToDouble(value));
	}
	return JSTValueFromBoolean(true);
}

JSTValue jsNativeCallVMCall JSTDeclareFunction(Pointer callVM, Number signature, Pointer function) {
	void * callVM = JSTValueToPointer(argv[0]);
	int type = JSTValueToDouble(argv[1]);
	void * fn = JSTValueToPointer(argv[2]);
	bool unsign = type >= 16;
	if (unsign) type -= 16;
	if (type & JSTNativeTypeBoolean) return JSTValueFromBoolean(dcCallBool(callVM, fn));
	else if (type & JSTNativeTypeChar) return JSTValueFromDouble((double) (unsign)?dcCallChar(callVM, function):(signed char)dcCallChar(callVM, fn));
	else if (type & JSTNativeTypeShort) return JSTValueFromDouble((double) (unsign)?(unsigned short)dcCallShort(callVM, function):dcCallShort(callVM, fn));
	else if (type & JSTNativeTypeInt) return JSTValueFromDouble((double) (unsign)?(unsigned int)dcCallInt(callVM, function):dcCallInt(callVM, fn));
	else if (type & JSTNativeTypeLong) return JSTValueFromDouble((double) (unsign)?(unsigned long)dcCallLong(callVM, function):dcCallLong(callVM, fn));
	else if (type & JSTNativeTypeLongLong) return JSTValueFromDouble((double) (unsign)?(unsigned long long)dcCallLongLong(callVM, function):dcCallLongLong(callVM, fn));
	else if (type & JSTNativeTypeFloat) return JSTValueFromDouble(dcCallFloat(callVM, fn));
	else if (type & JSTNativeTypeDouble) return JSTValueFromDouble(dcCallDouble(callVM, fn));
	else if (type & JSTNativeTypeVoid) dcCallVoid(callVM, function);
	return JSTValueUndefined;

}

static JSValueRef jsNativeAddressRead JSTDeclareFunction (address, type) {

	void * address = JSTValueToPointer(argv[0]);
	int type = JSTValueToDouble(argv[1]);
	bool unsign = type >= 16; if (unsign) type -= 16;
	if (type & JSTNativeTypeBoolean) return JSTValueFromDouble((double) *(bool*)(address));
	else if (type & JSTNativeTypeChar) return JSTValueFromDouble((double) (unsign)?*(char*)(address):*(signed char*)(address));
	else if (type & JSTNativeTypeShort) return JSTValueFromDouble((double) (unsign)?*(unsigned short*)(address):*(short*)(address));
	else if (type & JSTNativeTypeInt) return JSTValueFromDouble((double) (unsign)?*(unsigned int*)(address):*(int*)(address));
	else if (type & JSTNativeTypeLong) return JSTValueFromDouble((double) (unsign)?*(unsigned long*)(address):*(long*)(address));
	else if (type & JSTNativeTypeLongLong) return JSTValueFromDouble((double) (unsign)?*(unsigned long long*)(address):*(long long*)(address));
	else if (type & JSTNativeTypeFloat) return JSTValueFromDouble((double) *(float*)(address));
	else if (type & JSTNativeTypeDouble) return JSTValueFromDouble((double) *(double*)(address));
	return NULL;

}

static JSValueRef jsNativeAddressWrite JSTDeclareFunction (address, type, value) {

	void * address = JSTValueToPointer(argv[0]);
	int type = JSTValueToDouble(argv[1]);
	JSValueRef value = argv[2];

	if (type & JSTNativeTypeBoolean) { *(bool*)(address) = JSValueToBoolean(ctx, value); return JSTValueFromBoolean(true); }
	else if (type & JSTNativeTypeChar) { *(char*)(address) = (char) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); }
	else if (type & JSTNativeTypeShort) { *(short*)(address) = (short) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); }
	else if (type & JSTNativeTypeInt) { *(int*)(address) = (int) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); }
	else if (type & JSTNativeTypeLong) { *(long*)(address) = (long) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); } 
	else if (type & JSTNativeTypeLongLong) { *(long long*)(address) = (long long) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); } 
	else if (type & JSTNativeTypeFloat) { *(float*)(address) = (float) JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); } 
	else if (type & JSTNativeTypeDouble) { *(double*)(address) = JSValueToNumber(ctx, value, exception); return JSTValueFromBoolean(true); } 
	return JSTValueFromBoolean(false);

}

JSTObject JSTNativeInit_ JSTUtility(JSTObject js) {

	JSTObject native = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(js, "native", native, JSTObjectPropertyRequired);

	JSTObjectSetProperty(native, "alignment", JSTValueFromDouble(G_MEM_ALIGN), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);
	JSTObjectSetProperty(native, "byteOrder", JSTValueFromDouble(G_BYTE_ORDER), JSTObjectPropertyReadOnly | JSTObjectPropertyRequired);

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
	JSTObjectSetProperty(callVM, "call", JSTFunctionCallback("call", jsNativeCallVMCall), JSTObjectPropertyRequired);

	JSTObject address = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(native, "address", address, JSTObjectPropertyRequired);
	JSTObjectSetProperty(address, "read", JSTFunctionCallback("read", jsNativeAddressRead), JSTObjectPropertyRequired);
	JSTObjectSetProperty(address, "write", JSTFunctionCallback("write", jsNativeAddressWrite), JSTObjectPropertyRequired);

	return native;

}

#endif
