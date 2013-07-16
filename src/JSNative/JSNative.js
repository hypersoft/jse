
new JSNative.Type.Alias(0, "void");
new JSNative.Type.Alias(10, "bool");

new JSNative.Type.Alias(20, "char");
new JSNative.Type.Alias(20, "signed char");
new JSNative.Type.Alias(20, "char signed");
new JSNative.Type.Alias(21, "unsigned char");
new JSNative.Type.Alias(21, "char unsigned");

new JSNative.Type.Alias(30, "short");
new JSNative.Type.Alias(30, "signed short");
new JSNative.Type.Alias(30, "short signed");
new JSNative.Type.Alias(31, "unsigned short");
new JSNative.Type.Alias(31, "short unsigned");

new JSNative.Type.Alias(40, "int");
new JSNative.Type.Alias(40, "signed int");
new JSNative.Type.Alias(40, "int signed");
new JSNative.Type.Alias(41, "unsigned int");
new JSNative.Type.Alias(41, "int unsigned");

new JSNative.Type.Alias(50, "long");
new JSNative.Type.Alias(50, "signed long");
new JSNative.Type.Alias(50, "long signed");
new JSNative.Type.Alias(51, "unsigned long");
new JSNative.Type.Alias(51, "long unsigned");

new JSNative.Type.Alias(60, "long long");
new JSNative.Type.Alias(60, "signed long long");
new JSNative.Type.Alias(60, "long long signed");
new JSNative.Type.Alias(61, "unsigned long long");
new JSNative.Type.Alias(61, "long long unsigned");

new JSNative.Type.Alias(70, "float");
new JSNative.Type.Alias(80, "double");

new JSNative.Type.Alias(90, "pointer");
new JSNative.Type.Alias(90, "void *");

JSNative.Library = function(path) {

	if (path in JSNative.Library) return JSNative.Library[path];
	var lib = JSNative.jsnLoadLibrary(path);
	Object.defineProperties(lib, {
		constructor: { value: JSNative.Library },
		findSymbol: { value: JSNative.Library.findSymbol.bind(lib) }
	});
	return (JSNative.Library[path] = lib);

}

JSNative.Library.findSymbol = function(searchString) {
	if (searchString in this) return this[searchString];
	this[searchString] = JSNative.jsnFindSymbol(this, searchString);
	return this[searchString];
}

JSNative.CallVM = function() {
	var size = (JSNative.Address.alignment * 2); // some buffering
	for (var i = 0; i < arguments.count; i++) {
		var n = parseInt(arguments[i]);
		if (isNaN(n)) n = Number(JSNative.Type(arguments[i]));
		if (isNaN(n)) throw new ReferenceError("CallVM: size integer argument is NaN: argument: "+i);
		if (n < JSNative.Address.alignment) n = JSNative.Address.alignment;
		size += n;
	}
	var vm = JSNative.jsnNewCallVM(size); var mode = { value: "default" };
	Object.defineProperties(vm, {
		constructor: { value: JSNative.CallVM },
		free: { value: JSNative.CallVM.free.bind(vm), enumerable:true },
		error: { get: JSNative.CallVM.error.bind(vm), enumerable:true },
		mode: { set: JSNative.CallVM.mode.bind(vm, mode), get: function() { return mode.value; }, enumerable:true },
		reset: { value: JSNative.CallVM.reset.bind(vm, mode), enumerable:true },
		push: { value: JSNative.CallVM.push.bind(vm), enumerable:true },
		call: { value: JSNative.CallVM.call.bind(vm), enumerable:true },
		conversion: { value: [], writeable: true }, // allocated addreses to free post-call
	});
	return vm;
}
JSNative.CallVM.free = function() { if (this === JSNative.CallVM) return;
	JSNative.jsnCallVMFree(this);
}
JSNative.CallVM.error = function() { if (this === JSNative.CallVM) return;
	return JSNative.jsnCallVMGetError(this);
}
JSNative.CallVM.mode = function(writeBack, mode) { if (this === JSNative.CallVM) return;
	writeBack.value = mode; JSNative.jsnCallVMSetMode(this, JSNative.CallVM.mode[mode]);
}
JSNative.CallVM.reset = function(writeBack, mode) { if (this === JSNative.CallVM) return;
	var allocator = new JSNative.Allocator(), address;
	while ((address = this.conversion.pop()) != undefined) allocator.free(address.pointer);
	writeBack.value = mode; JSNative.jsnCallVMReset(this, JSNative.CallVM.mode[mode]);
}
JSNative.CallVM.push = function() {  if (this === JSNative.CallVM) return;
	if (arguments.length == 0) throw new ReferenceError("CallVM: push requires at least one argument");
	for (var i = 0; i < arguments.length; i++) {
		var arg = arguments[i];
		if (arg === null) {
			JSNative.jsnArgPointer(this, 0); continue;
		} else if (arg === undefined) {
			JSNative.jsnArgInt(this, 0); continue;	
		} else if (typeof arg == "string") {
			var s = new JSNative.Array("char", arg);
			this.conversion.push(s);
			JSNative.jsnArgPointer(this, s); continue;
		} else if (arg instanceof JSNative.Array || arg instanceof JSNative.Address) { 
			JSNative.jsnArgPointer(this, arg); continue; 
		} else if (arg instanceof JSNative.Value) {
			var pCode = arg.type.code; pCode -= (pCode % 2); // unsigned type code
			if (pCode == 0) throw new TypeError("CallVM: unable to push void type: argument "+i);
			else if (pCode == 90) JSNative.jsnArgPointer(this, arg);
			else if (pCode == 40) JSNative.jsnArgInt(this, arg);
			else if (pCode == 50) JSNative.jsnArgLong(this, arg);
			else if (pCode == 10) JSNative.jsnArgBool(this, arg);
			else if (pCode == 20) JSNative.jsnArgChar(this, arg);
			else if (pCode == 30) JSNative.jsnArgShort(this, arg);
			else if (pCode == 60) JSNative.jsnArgLongLong(this, arg);
			else if (pCode == 70) JSNative.jsnArgFloat(this, arg);
			else if (pCode == 80) JSNative.jsnArgDouble(this, arg);
			else throw new TypeError("CallVM: unable to push unknown type code: argument "+i);
			continue;
		} else if (typeof arg == "boolean") {
			JSNative.jsnArgBool(this, Number(arg)); continue;
		} else if (typeof arg == "number") {
			if (! isNaN(parseInt(arg))) {
				JSNative.jsnArgInt(this, arg); continue;
			} else if (! isNaN(parseFloat(arg))) {
				JSNative.jsnArgFloat(this, parseFloat(arg)); continue;
			} else JSNative.jsnArgDouble(this, arg);
			continue;
		} else throw new TypeError("CallVM: unable to push type: "+typeof arg+": argument "+i);
	}
	return true;
}
JSNative.CallVM.call = function(type, symbol) {  if (this === JSNative.CallVM) return;
	var tCode = type.code; tCode -= (tCode % 2);
	if (tCode == 0) {
		JSNative.jsnCallVoid(this, symbol);
	} else if (tCode == 10) {
		JSNative.jsnCallBool(this, symbol);
	} else if (tCode == 20) {
		JSNative.jsnCallChar(this, symbol);
	} else if (tCode == 30) {
		JSNative.jsnCallShort(this, symbol);
	} else if (tCode == 40) {
		JSNative.jsnCallInt(this, symbol);
	} else if (tCode == 50) {
		JSNative.jsnCallLong(this, symbol);
	} else if (tCode == 60) {
		JSNative.jsnCallLongLong(this, symbol);
	} else if (tCode == 70) {
		JSNative.jsnCallFloat(this, symbol);
	} else if (tCode == 80) {
		JSNative.jsnCallDouble(this, symbol);
	} else if (tCode == 90) {
		JSNative.jsnCallPointer(this, symbol);
	} else {
		throw new TypeError("CallVM: unknown call type: "+tCode+" ("+String(type)+")");
	}
}

Object.defineProperties(JSNative.CallVM.mode, {
	"default": { value: 0, enumerable: true },
	"system default": { value: 200, enumerable: true },
	"linux system": { value: 201, enumerable: true },
	"ellipsis": { value: 100, enumerable: true },
	"varargs": { value: 101, enumerable: true },
	"cdecl": { value: 1, enumerable: true },

	// not adding any more because I cannot test them (nor do I plan to...) - pc.wiz.tt

});

