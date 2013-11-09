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
	defineAlias: function(name, model) {
		model = native.type(model);
		var alias = Object.create(model);
		var definition = {}; definition[name] = {value: alias,enumerable:false,configureble:true, writeable:true}
		Object.defineProperties(native.type, definition);
		return alias;
	},
})

var sigIndex = 0;

var NativeTypeNext = function() { return (1 << sigIndex++)}

var NativeType = function(name, size, unsigned, code, pointers) {
	this.name = name, this.size = size,
	this.isConstant = false,
	this.isUnsigned = (unsigned == true)?true:false,
	this.code = code || NativeTypeNext();
	this.derefCount = pointers || 0;
	native.type[name] = this;
}

NativeType.dummy = {
	variable: undefined, constant: undefined, deref: undefined,
	isPointer: false, pointer: undefined, signed: undefined, unsigned: undefined, 
	isConstant: false
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
//			for (name in NativeType.dummy) if (copy[name] == undefined)	delete copy[name];
			copy.unsigned = undefined;
		}
		copy.derefCount++;
		copy.size = size.pointer;
		return copy;
	},
	get isPointer() {
		return (this.derefCount > 0);
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
}

new NativeType('void', size.void);
js.extend(native.type.void, NativeType.dummy); // blocks prototype accessors
delete native.type.void.pointer; // allows prototype accessor

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

new NativeType('pointer', size.pointer, false, undefined, 1);
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
new NativeType('string', size.string, false, undefined, 1);
native.type.string.name = "JSStringRef";
new NativeType('value', size.value, false, undefined, 1);
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

var Address = js.extendPrototype(function Address(v, t, l){
	var o = native.instance(Address.container, 0);
	native.setPrototype(o, Address.prototype);
	if (v != undefined) o['&'] = v;
	if (t != undefined) o.type = (typeof t == 'string')?native.type[t]:t;
	if (l != undefined) o.length = l;
	return o;
}, {
	container: null, name: "Address", prototype: {},
	set: function(name, value) { var i = parseInt(name);
		if (! isNaN(i) ) {
			if (i >= this.length || i < 0) throw new RangeError("address index out of bounds");
			native.address.write((i * this.size) + this['&'], this.type, value);
			return true;
		}
		if (name == 'value' || name == '*') {
			native.address.write(this['&'], this.type, value);
			return true;		
		}
		if (name == 'type') {
			Object.defineProperties(this, {type:{'value': native.type(value), writeable:true,configurable:true}});
			Object.defineProperties(this, {size:{'value': native.type(value).size, writeable:true,configurable:true}});
			return true;
		}
		if (name == 'length') {
			Object.defineProperties(this, {length:{'value':value, writeable:true,configurable:true}})
			return true;
		}
		if (name == '&') {
			Object.defineProperties(this, {'&':{value: parseInt(value), configurable:true,writeable:true}});
		}
		if (name == 'free') {
			Object.defineProperties(this, {free:{value: value, configurable:true,writeable:true}});
		}
		return null;
	},
	get: function(name) { var i = parseInt(name);
		if (! isNaN(i) ) {
			if (i >= this.length || i < 0) throw new RangeError("address index out of bounds");
			return native.address.read((this['&'] + (i * this.size)), this.type);
		}
		if (name == 'value' || name == '*') return native.address.read(this['&'], this.type);
		var index = name.match(/^\&([0-9]+)$/);
		if (index != null) { index = index[1];
			if (index >= this.length || index < 0) throw new RangeError("address index out of bounds");
			return (this['&'] + (index * this.size));
		}
		return null;
	},
	enumerate: function() {
		var o = {length:this.length}; for (i = 0; i < o.length; i++) o[i] = i; return o;
	},
})

Address.prototype = js.extendPrototype(Object.defineProperties({}, {
	constructor: {value:Address}, length:{value:1},
}),{
	toString: function(){ return '[object Address 0x'+this['&'].toString(16)+']'},
	valueOf: function(){ return this.value; },
});

Address.container = native.container(Address);

js.extendPrototype(Number.prototype, {toAddress: function toAddress(t, l) {
	var a = parseInt(this);
	if (isNaN(a)) throw new RangeError("unable to parse integer address");
	return Address(a, t, l);
}});

var exit = new Procedure(native.engine, 'int', 'exit', ['int']);
var echo = new Command('echo', '-E');

