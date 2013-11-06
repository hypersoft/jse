js.exec.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
}

function Procedure(lib, type, name, proto, optionalMode) {
	if (lib == undefined || lib == 'jse') lib = js.engine;
	var proc = (typeof name == 'number')?name:new Object(lib.find(name));
	proc.proto = js.type.resolve(proto);
	if (optionalMode != undefined) proc.mode = (typeof optionalMode == 'string')?js.call[optionalMode]:optionalMode;
	else proc.mode = js.call['native'];
	proc.return = js.type.resolve(type);
	proc.base = proc.proto.slice(0); // for reset base definition
	var bound = js.native.exec.bind(proc);
	bound.address = Number(proc);
	proc.length = Procedure.getLength.call(proc)
	proc.size = Procedure.getSize.call(proc)
	return bound;
}

Procedure.getLength = function() { return this.proto.length; }
Procedure.getSize = function(){ var s = 0; var p = this.proto; for (item in p) s += js.native.typeSize(p[item]); return s;}

Callback = function Callback(o, t, f, p) {
	var proto = p.slice(0);
	var object = js.native.callback(o, js.type.resolve(t), f, js.type.resolve(proto));
	object.free = js.native.callbackFree.bind(null, object);
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
	var cache = js.native.lib;
	if (s == undefined) s = '';
	this.name = (s == '') ? "jse" : s;
	if (this.name in cache) {
		cache[this.name].retainers++;
		return cache[this.name];
	}
	cache[this.name] = this;
	if ((this.pointer = js.native.library.load(s)) == 0) throw new ReferenceError("unable to locate library `"+s+"'");
}

SharedLibrary.prototype = js.extendPrototype({}, {
	constructor: SharedLibrary, retainers: 1, pointer: null,
	retain: function() {this.retainers++},
	release: function() {
		if (this.pointer == undefined) return true;
		if (this.retainers != 0) this.retainers--;
		if (this.retainers == 0) {
			js.native.library.free(this.pointer);
			delete js.native.lib[this.name];
			for (name in this) delete this[name];
			this.pointer = null;
			return true;
		} else return false;
	},
	find: function(s) {
		if (s == undefined || s == '') return s;
		if (s in this) return this[s];
		if ((this[s] = js.native.library.findSymbol(this.pointer, s)) == 0) throw new ReferenceError("unable to locate library symbol `"+s+"'");
		return this[s];
	},
	valueOf: function() { return this.pointer },
	toString: function() { return this.name },
});

Object.defineProperties(js, {engine:{value:new SharedLibrary(), enumerable:true}});

js.type.resolve = function (t) {
	if (typeof t == 'string') return js.type[t];
	if (js.classOf(t) == Array.name) {
		var proto = new Array();
		for (item in t) proto[item] = (typeof t[item] == 'string') ? js.type[t[item]] : t[item];
		return proto;
	}
	return t;
}

js.type.lookup = new Procedure("jse", 'utf8 *', 'JSTNativeTypeGetName', ['int']);

js.type.sign = function(t) {
	if (typeof t == 'string') t = js.type[t];
	return ((t & js.type.unsigned) && t > js.type.bool && t < js.type.float) ? t-- : t
}

js.extend(js.type, {
	uchar:js.type.char, 						schar:js.type.sign(js.type.char),
	ushort:js.type.unsigned | js.type.short, 	sshort: js.type.short,
	uint:js.type.unsigned | js.type.int, 		sint: js.type.int,
	ulong:js.type.unsigned | js.type.long, 		slong: js.type.long,
	uint64:js.type.unsigned | js.type.int64, 	sint64: js.type.int64
})

js.type['void *'] = js.type.void | js.type.pointer;
js.type['utf8 *'] = js.type.utf8 | js.type.pointer;
js.type['utf16 *'] = js.type.utf8 | js.type.pointer;
js.type['utf32 *'] = js.type.utf32 | js.type.pointer;

var Address = js.extendPrototype(function Address(v, t, l){
	var o = js.native.instance(Address.container, 0);
	js.native.setPrototype(o, Address.prototype);
	if (v != undefined) o['&'] = v;
	if (t != undefined) o.type = (typeof t == 'string')?js.type[t]:t;
	if (l != undefined) o.length = l;
	return o;
}, {
	container: null, name: "Address", prototype: {},
	set: function(name, value) { var i = parseInt(name);
		if (! isNaN(i) ) {
			if (i >= this.length || i < 0) throw new RangeError("address index out of bounds");
			js.native.address.write((i * this.size) + this['&'], this.type, value);
			return true;
		}
		if (name == 'value' || name == '*') {
			js.native.address.write(this['&'], this.type, value);
			return true;		
		}
		if (name == 'type') {
			Object.defineProperties(this, {type:{'value': (typeof value == 'string')?js.type[value]:value, writeable:true,writeable:true}});
			Object.defineProperties(this, {size:{'value':js.native.typeSize(this.type), writeable:true,writeable:true}});
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
			return js.native.address.read((this['&'] + (i * this.size)), this.type);
		}
		if (name == 'value' || name == '*') return js.native.address.read(this['&'], this.type);
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

Address.container = js.native.container(Address);

js.extendPrototype(Number.prototype, {toAddress: function toAddress(t, l) {
	var a = parseInt(this);
	if (isNaN(a)) throw new RangeError("unable to parse integer address");
	return Address(a, t, l);
}});

var exit = new Procedure(js.engine, 'int', 'exit', ['int']);

