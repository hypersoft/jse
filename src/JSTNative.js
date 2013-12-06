js.exec.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
};

(function init() {

var size = native.size; delete native.size;

native.type = function typeDeref(v) {
	if (typeof v == 'string' && native.type[v] && native.type[v].constructor == native.type) return native.type[v];
	else if (v.constructor == native.type) return v;
	else if (js.classOf(v) == 'Array') {
		var name, a = [];
		for (name in v) a.push(native.type(v[name]));
		return a;
	}
	throw new TypeError("native type identifier is not a string, or native type object");
}

js.extendPrototype(native.type, {
	toSource:function(type){
		if (typeof type != 'string' && type != undefined) throw new TypeError("Expected source type string");
		if (type == undefined || type.match(/^c$/i) != null) {
		var string = [];
		for (name in native.type) string.push('#define JSN_C_TYPE_'+name.toUpperCase()+' '+Number(native.type[name].code))
		return string.join('\n');
		}
		throw new TypeError("Unrecognized source type: "+type);
	},
	defineAlias: function(name, model) {
		model = native.type(model);
		var alias = Object.create(model);
		var definition = {}; definition[name] = {value: alias,enumerable:false,configureble:true, writeable:true}
		Object.defineProperties(native.type, definition);
		return alias;
	},
	align: function typeAlign(base, type) {
		var misalignment = (base % type.size)
		if (misalignment != 0) {
			return (type.size - misalignment);
		} else return 0;
	},
})

var sigIndex = 0;

var NativeTypeNext = function() { return (1 << sigIndex++)}

var NativeType = function(name, size, unsigned, code, pointers) {
	this.name = name, this.size = size,
	this.isConstant = false,
	this.isUnsigned = (unsigned == true)?true:false,
	this.code = code || NativeTypeNext(),
	this.derefCount = pointers || 0;
	native.type[name] = this;
}

NativeType.dummy = {
	variable: undefined, constant: undefined, deref: undefined,
	isPointer: false, pointer: undefined, signed: undefined, unsigned: undefined, 
	isConstant: false,
};

NativeType.prototype = {
	constructor: native.type,
	isConstant: false, isUnsigned: false, name: undefined, code: 0, size: 0, derefCount: 0,
	toString:function(){
		var string = "", name = (this.name == 'void *')?'void':this.name;
		if (this.isConstant) string += 'const ';
		if (name != 'void' && this.isUnsigned) string += 'unsigned ';
		if (this.isPointer) {
			var i = this.derefCount;
			string += name + ' '; while (i--) string += '*';
		} else string += name;
		return string;
	},
	valueOf:function(){
		var code = this.code;
		if (this.isConstant == true) code = (code | native.type.const.code);
		if (this.isUnsigned == true) code = (code | native.type.unsigned.code);
		if (this.derefCount > 0) code = (code | native.type.pointer.code);
		return code;
	},
	equals: function(v) {
		return Boolean(this.valueOf() == v.valueOf() && this.derefCount== v.derefCount)
	},
	get unsigned() {
		if (this.isUnsigned) return this;
		var o = Object.create(this); o.isUnsigned = true;
		return o;
	},
	get signed() {
		if (!this.isUnsigned) return this;
		var o = Object.create(this); o.isUnsigned = false;
		return o;
	},
	get pointer() {
		var copy = Object.create(this);
		if (this.name == 'void') {
			var name = '';
			copy.unsigned = undefined;
		}
		copy.derefCount++;
		copy.size = size.pointer;
		return copy;
	},
	get isPointer() {
		return (this.derefCount > 0);
	},
	get isVoid(){
		return Number(this) == native.type.void.code;
	},
	get deref() {
		if (this.derefCount < 1) return this;
		var o = Object.create(this); o.derefCount--;
		return o;
	},
	get constant() {
		if (this.isConstant) return this;
		var o = Object.create(this); o.isConstant = true;
		return o;
	},
	get variable() {
		if (!this.isConstant) return this;
		var o = Object.create(this); o.isConstant = false;
		return o;
	},
	value: function(value, address, length) {
		var o = new Object(value);
		o.type = this;
		o.address = (address)?address:undefined;
		o.length = (length)?length:1
		return o;
	}
}

new NativeType('void', size.void);
js.extend(native.type.void, NativeType.dummy); // blocks prototype accessors
delete native.type.void.deref;
delete native.type.void.pointer; // allows prototype accessor
delete native.type.void.isPointer; // allows prototype accessor

new NativeType('const', size.const);
js.extend(native.type.const, NativeType.dummy);
native.type.defineAlias('constant', 'const');

// forward declaration; it is it's own type: hence unsigned = false;
new NativeType('unsigned', size.unsigned, false);

new NativeType('bool', size.bool);
native.type.bool.name = 'bool'
native.type.defineAlias('boolean', 'bool');

new NativeType('char', size.char, true);
new NativeType('short', size.short);
new NativeType('int', size.int);

new NativeType('enum', size.enum, true, native.type.int.code); // unique; a copy of 'int'
native.type.enum.name = native.type.int.name;
native.type.enum.isConstant = true;

new NativeType('long', size.long);

new NativeType('size', size.size, true);
native.type.size.name = 'size_t'

new NativeType('pointer', size.pointer, true, undefined, 1);
native.type.pointer.name = 'void *'
native.type.pointer.unsigned = undefined;
native.type.defineAlias('void *', 'pointer');

new NativeType('int64', size.int64);
native.type.int64.name = "long long";
native.type.defineAlias('long long', 'int64');

new NativeType('float', size.float);
new NativeType('double', size.double);

// complex reference definitions
new NativeType('struct', size.struct);
new NativeType('union', size.union);
new NativeType('ellipsis', size.ellipsis);
native.type.ellipsis.name = '...';
js.extend(native.type.ellipsis, NativeType.dummy); // blocks prototype accessors
native.type.defineAlias('...', 'ellipsis');

// translation reference definitions
new NativeType('utf8', size.utf8, true);
native.type.utf8.name = "UTF8";
new NativeType('utf16', size.utf16, true);
native.type.utf16.name = "UTF16";
new NativeType('utf32', size.utf32, true);
native.type.utf32.name = "UTF32";
new NativeType('string', size.string, true, undefined, 1);
native.type.string.name = "JSStringRef";
new NativeType('value', size.value, true, undefined, 1);
native.type.value.name = "JSValueRef";

})();

function Procedure(lib, type, name, proto, optionalMode) {
	if (lib == undefined || lib == 'jse') lib = native.engine;
	var proc = new Object(lib.find(name)); proc.mode = 0;
	if (optionalMode != undefined) {
		if (typeof optionalMode == 'string') {
			proc.mode = native.call[optionalMode];
		} else proc.mode = optionalMode;
	} 
	proc.return = native.type(type);
	proc.proto = native.type(proto);
	if (proc.proto[proc.proto.length - 1].code == native.type.ellipsis.code) {
		proc.proto.pop(); proc.mode = native.call.ellipsis;
	}
	var bound = native.exec.bind(proc);
	bound.address = Number(proc);
	proc.length = Procedure.getLength.call(proc)
	proc.size = Procedure.getSize.call(proc)
	return bound;
}

Procedure.getLength = function() { return this.proto.length; }
Procedure.getSize = function(){ var s = 0; var p = this.proto; 
	for (item in p) {
		//Command('echo')(p[item]);
		s += p[item].size;
	}
	s += this.return.size;
return s;
}

Callback = function Callback(o, t, f, p) {
	var proto = p.slice(0);
	var object = native.callback(o, native.type(t), f, native.type(proto));
	object.free = native.callbackFree.bind(null, object);
	return object;
}

var Command = function(command) {
	this.argv = Array.apply(null, arguments);
	var bound = js.exec.bind(this);
	bound.capture = this;
	bound.argv = this.argv;
	bound.toString = function(){return this.argv[0]};
	return bound;
}

function SharedLibrary(s) {
	var cache = native.lib;
	if (s == undefined) s = '';
	this.name = (s == '') ? "jse" : s;
	if (this.name in cache) {
		cache[this.name].retainers++;
		return cache[this.name];
	}
	cache[this.name] = this;
	if ((this.pointer = native.library.load(s)) == 0) throw new ReferenceError("unable to locate library `"+s+"'");
}

SharedLibrary.prototype = js.extendPrototype({}, {
	constructor: SharedLibrary, retainers: 1, pointer: null,
	retain: function() {this.retainers++},
	release: function() {
		if (this.pointer == undefined) return true;
		if (this.retainers != 0) this.retainers--;
		if (this.retainers == 0) {
			native.library.free(this.pointer);
			delete native.lib[this.name];
			for (name in this) delete this[name];
			this.pointer = null;
			return true;
		} else return false;
	},
	find: function(s) {
		if (s == undefined || s == '') return s;
		if (s in this) return this[s];
		if ((this[s] = native.library.findSymbol(this.pointer, s)) == 0) throw new ReferenceError("unable to locate library symbol `"+s+"'");
		return this[s];
	},
	valueOf: function() { return this.pointer },
	toString: function() { return this.name },
});

Object.defineProperties(native, {engine:{value:new SharedLibrary(), enumerable:true}});

var Native = (function init(global, native) {

	global.double = native.type.double, global.float = native.type.float,
	global.int64 = native.type.int64,
	global.int = native.type.int, global.long = native.type.long,
	global.vptr = native.type.void.pointer, global.void_t = native.type.void;
	global.short = native.type.short, global.char = native.type.char,
	global.size_t = native.type.size, global.boolean = native.type.boolean,
	global.utf8 = native.type.utf8, global.utf16 = native.type.utf16,
	global.utf32 = native.type.utf32,
	global.value_t = native.type.value, global.string_t = native.type.string;
	global.file_t = native.type.void; file_t.name = 'FILE'

	global.io = {print:Procedure(native.engine, int, 'fprintf', [file_t, utf8.pointer, '...'])};
	global.print = Procedure(native.engine, int, 'printf', [utf8.pointer, '...']);
	global.exit = new Procedure(native.engine, 'int', 'exit', ['int']);
	global.echo = new Command('echo', '-E');

	var malloc = Procedure(native.engine, vptr, 'malloc', [size_t]);

	var Native = {Number:Number,Boolean:Boolean};

	Native.Object = function(){	return Object.apply(this, arguments); }
	native.setPrototype(Native.Object, Object);

	Native.Object.Extend = function Extend(object, properties, permissions) {
		var o = {};
		for (name in properties) {
			o[name] = {
				value: properties[name], enumerable: permissions[0],
				writeable: permissions[1], configurable: permissions[2]
			};
		}
		return Object.defineProperties(object, o);
	}

	Native.Object.Accessor = function(object, name, get, set, permissions) {
		var o = {};
		o[name] = { get:get, set:set, enumerable: permissions[0],
			writeable: permissions[1], configurable: permissions[2]
		};
		return Object.defineProperties(object, o);
	}

	Native.Object.Prototype = function(object, prototype) {
		if (prototype === undefined) return native.getPrototype(object);
		native.setPrototype(object, prototype);
		return object;
	}

	Native.Object.Private = function(object, prototype) {
		if (prototype === undefined) return native.getPrivate(object);
		if (isNaN(parseInt(prototype))) throw new ReferenceError("unable to set object private");
		native.setPrivate(object, prototype);
		return object;
	}

	Native.Address = function(t, v) {
		var _this = Native.Object.Extend({},{_address:v,type:t},[false,true,false]);
		return Native.Object.Prototype(Native.Object.Extend(_this, {
			write:Native.Address.Write.bind(_this), read:Native.Address.Read.bind(_this)
		},[false,false,false]), Native.Address.prototype);
	}

	Native.Address.prototype = {
		constructor: Native.Address, write:null, read:null, free:null,
		get address(){return this._address},
		set address(v){this._address = v},
		set value(v){this.write(v); return v},
		get value(){return this.read()},
		valueOf: function(){return this.address},
		toString:function(){return this.value.toString()},
		get deref(){
			if (this.type.derefCount < 2) throw new TypeError("cannot dereference type " + String(this.value.type));
			return Native.Address(this.value.type, this.value)
		},
		defineAs:function property(object, name){
			return Native.Address.Accessor(object, name, this)
		},
	}

	Native.Address.Accessor = function(object, name, address, type) {
		if (type != undefined) {
			address = Native.Address(address, type);
		}
		var o = {};
		o[name] = { get:address.read, set:address.write, enumerable: true,
			writeable: false, configurable: true
		};
		return Object.defineProperties(object, o);
	}

	Native.Address.Write = function (value) {
		var type = this.type.deref;
		if (type.equals(utf8)) this.address = String.toUTF8(value);
		else native.address.write(this.address, type, value);
		return value;
	}

	Native.Address.Read = function() { return new Native.Address.Reference(this); }

	Native.Address.Reference = function(a) { this.header = a; }

	Native.Address.Reference.prototype = {
		constructor: Native.Address,
		get clone(){return this.header.deref},
		get defineAs(){return this.header.defineAs},
		get type(){return this.header.type.deref},
		set type(v){return this.header.type = v},
		get address(){return this.header.address},
		set address(v){ return this.header.address = v},
		get read(){return this.header.read},
		get write(){return this.header.write},
		get free(){return this.header.free},
		set read(v){return this.header.read = v},
		set write(v){return this.header.write = v},
		set free(v){return this.header.free = v},
		valueOf:function(){
			if (this.type.equals(utf8)) return this.address;
			return native.address.read(this.address, this.type)
		},
		toString:function(){
			if (this.type.equals(utf8.pointer))
				return String.fromUTF8(native.address.read(this.address, this.type));
			else if (this.type.equals(utf8))
				return String.fromUTF8(this.address);
			return String(native.address.read(this.address, this.type))
		},
	}

	Native.Array = function Array(t, a, l) {
		var o = Native.Object.Extend(native.instance(Native.Array.container, 0), {
			type:(t)?t:void_t, address:(a)?a:0, length:(l)?l:1
		}, [false,true,true]);
		Native.Object.Prototype(o, Native.Array.prototype);
		return o;
	}

	Native.Array.prototype = {
		constructor: Native.Array, length: 0,
		valueOf: function() {return this.address},
		toString: function() {return String(this.address)},
	}

	Native.Array.set = function index(name, value) { var i = parseInt(name);
		if (! isNaN(i) ) {
			if (i >= this.length || i < 0) throw new RangeError("address index out of bounds");
			return Native.Address(this.type, this.address + (i * this.type.size)).value = value;
		}
		return null;
	}

	Native.Array.get = function index(name) { var i = parseInt(name);
		if (! isNaN(i) ) {
			if (i >= this.length || i < 0) throw new RangeError("address index out of bounds");
			Native.Address(this.type, this.address + (i * this.type.size)).defineAs(this, i);
			return this[i];
		}
		return null;
	}

	Native.Array.enumerate = function list() {
		var o = {length:this.length}; for (i = 0; i < o.length; i++) o[i] = i; return o;
	}

	Native.Array.container = native.container(Native.Array);

	global.argv = Native.Array(utf8.pointer.pointer, js.run.argv, js.run.argc);

	var locate = native.engine.find;
	Native.Address(file_t.pointer.pointer, locate('stdin')).defineAs(io, 'stdin');
	Native.Address(file_t.pointer.pointer, locate('stdout')).defineAs(io, 'stdout');
	Native.Address(file_t.pointer.pointer, locate('stderr')).defineAs(io, 'stderr');

	io.print.error = Procedure(
		native.engine, int, 'fprintf', [vptr, utf8.pointer, '...']
	).bind(null, io.stderr);

	return Native;

})(this, native);

