js.exec.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
}

js.type.sign = function(t) {
	if (typeof t == 'string') t = js.type[t];
	return ((t & js.type.unsigned) && t > js.type.bool && t < js.type.float) ? t-- : t
}

js.type.address = function(t) { return (((typeof t == 'string')?js.type[t]:t) | js.type.pointer) }

js.extend(js.type, {
	uchar:js.type.char, 						schar:js.type.sign(js.type.char),
	ushort:js.type.unsigned | js.type.short, 	sshort: js.type.short,
	uint:js.type.unsigned | js.type.int, 		sint: js.type.int,
	ulong:js.type.unsigned | js.type.long, 		slong: js.type.long,
	uint64:js.type.unsigned | js.type.int64, 	sint64: js.type.int64
})

js.type['utf8 *'] = js.type.utf8 | js.type.pointer;
js.type['utf16 *'] = js.type.utf8 | js.type.pointer;
js.type['utf32 *'] = js.type.utf32 | js.type.pointer;

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
	this.pointer = js.native.library.load(s);
	cache[this.name] = this;
	// hmmm... js.native.compressLibCache... not very useful and doesn't promote sound
	// coding ethic. load known vectors. its better to hit the cache than disk any day
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
		else return this[s] = js.native.library.findSymbol(this.pointer, s);
	},
	valueOf: function() { return this.pointer },
	toString: function() { return this.name },
});

Object.defineProperties(js, {engine:{value:new SharedLibrary(), enumerable:true}});

function Procedure(lib, name, mode, proto) {
	if (lib == undefined || lib == 'jse') lib = js.engine;
	var proc = new Object(lib.find(name)); proc.proto = new Array();
	proc.mode = (typeof mode == 'string') ? js.call[mode] : mode;
	for (item in proto) proc.proto[item] = (typeof proto[item] == 'string') ? js.type[proto[item]] : proto[item];
	proc.return = proc.proto.shift();
	proc.base = proc.proto.slice(0); // for reset base definition
	var bound = js.native.exec.bind(proc);
	if (proc.mode == js.call.ellipsis) bound.proc = proc, bound.parameters = Procedure.setParameters;
	proc.length = Procedure.getLength.call(proc)
	proc.size = Procedure.getSize.call(proc)
	return bound;
}
Procedure.getLength = function() { return this.proto.length; }
Procedure.getSize = function(){ var s = 0; var p = this.proto; for (item in p) s += js.native.typeSize(p[item]); return s;}
Procedure.setParameters = function(type) {
	if (this.parameters) { // only if defined
		var cache = this.proc;
		cache.proto = cache.base.slice(0); // reset to base definition
		for (t in arguments) cache.proto.push((typeof arguments[t] == 'string')?js.type[arguments[t]]:arguments[t]); // push new types
		cache.length = Procedure.getLength.call(cache); // calculate
		cache.size = Procedure.getSize.call(cache); // calculate
	} // fire when ready
}

var exit = new Procedure(js.engine, 'exit', 'native', ['int', 'int']);

var Address = js.extendPrototype(function Address(v, t, l){
	var o = js.native.instance(Address.container, 0);
	js.native.setPrototype(o, Address.prototype);
	if (v != undefined) Object.defineProperties(o, {'&':{value: parseInt(v), writeable:true}});
	if (t != undefined) o.type = (typeof t == 'string')?js.type[t]:t;
	if (l != undefined) o.length = l;
	return o;
}, {
	container: null, name: "Address", prototype: {},
	set: function(name, value) {
		if (!isNaN(i = parseInt(name))) {
			if (i >= this.length || i < 0) throw new RangeError("address index out of bounds");
			return js.native.address.write(this['&'] + (i * this.size), this.type, this.value);
		}
		if (name == 'value' || name == '*') {
			return js.native.address.write(this['&'], this.type, this.value);			
		}
		if (name == 'type') {
			Object.defineProperties(this, {type:{'value': (typeof value == 'string')?js.type[value]:value, writeable:true}});
			Object.defineProperties(this, {size:{'value':js.native.typeSize(this.type), writeable:true}});
			return null;
		}
		if (name == 'length') {
			Object.defineProperties(this, {length:{'value':value, writeable:true,configurable:true}})
			return null;
		}
		if (name == '&') { this['&'] = value;
			return null;
		}
		return null;
	},
	get: function(name) {
		if (name == 'value' || name == '*') return js.native.address.read(this['&'], this.type);
		if (!isNaN(i = parseInt(name))) {
			if (i >= this.length || i < 0) throw new RangeError("address index out of bounds");
			return js.native.address.read((this['&'] + (i * this.size)), this.type);
		}
		var index = name.match(/^\&([0-9]+)$/);
		if (index != null) { index = index[1];
			if (index >= this.length || index < 0) throw new RangeError("address index out of bounds");
			return (this['&'] + (index * this.size));
		}
		return null;
	},
	enumerate: function() {
		var o = {length:this.length};
		for (i = 0; i < o.length; i++) o[i] = i;
		return o;
	},
})

Address.prototype = js.extendPrototype(Object.defineProperties({}, {
	constructor: {value:Address},
	length:{value:1},
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


