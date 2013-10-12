js.exec.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
}

js.cache = new Array();

js.call = {native: 0};
js.type = {
	'void': 0, unsigned: 1, pointer: 4, ellipsis: 3, boolean: 8, char: (16 | 1),
	short: 32, int: 64, long: 128, int64: 256, float: 512, double: 1024, size: 2048,
	utf8: 4096, utf16: 8192, utf32: 16384, struct: 32768, union: 65536, value: 131072,
	string: 262144, exception: 524288
}

js.extend(js.type, {
	uchar:js.type.unsigned | js.type.char,
	ushort:js.type.unsigned | js.type.short,
	uint:js.type.unsigned | js.type.int,
	ulong:js.type.unsigned | js.type.long,
	uint64:js.type.unsigned | js.type.int64,
})

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
	if (s == undefined || s == null) s = '';
	this.name = (s == '') ? "jse" : s;
	if (this.name in cache) {
		cache[this.name].retainers++;
		return cache[this.name];
	}
	this.pointer = js.native.library.load(s);
	cache[this.name] = this;
}

SharedLibrary.prototype = js.extendPrototype({}, {
	constructor: SharedLibrary, retainers: 1, pointer: null,
	retain: function() {this.retainers++},
	release: function() {
		if (this.pointer == null) return true;
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
		if (s == undefined || s == null || s == '') return s;
		if (s in this) return this[s];
		else return this[s] = js.native.library.findSymbol(this.pointer, s);
	},
	valueOf: function() { return this.pointer },
	toString: function() { return this.name },
});

Object.defineProperties(js, {engine:{value:new SharedLibrary(), enumerable:true}});

function Procedure(lib, name, mode, proto) {
	var proc = new Object(lib.find(name));
	proc.proto = new Array();
	proc.mode = (typeof mode == 'string') ? js.call[mode] : mode;
	for (item in proto) proc.proto[item] = (typeof proto[item] == 'string') ? js.type[proto[item]] : proto[item];
	proc.return = proc.proto.shift();
	proc.length = proc.proto.length, proc.size = 0;
	for (item in proc.proto) proc.size += js.native.typeSize(proc.proto[item]);
	var bound = js.native.exec.bind(proc);
	// bound.proc = proc; need property accessors on proc to validate data
	return bound;
}

var exit = new Procedure(js.engine, 'exit', 0, [4, 4]);

