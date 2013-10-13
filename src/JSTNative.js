js.exec.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
}

//js.type.sign = function(t) { return ((t & js.type.unsigned) && t > js.type.bool && t < js.type.float) ? t-- : t }
js.type.address = function(t) { return (t | js.type.pointer) }

js.extend(js.type, {
	uchar:js.type.char,// 						schar:js.type.sign(js.type.char),
	ushort:js.type.unsigned | js.type.short, 	sshort: js.type.short,
	uint:js.type.unsigned | js.type.int, 		sint: js.type.int,
	ulong:js.type.unsigned | js.type.long, 		slong: js.type.long,
	uint64:js.type.unsigned | js.type.int64, 	sint64: js.type.int64
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

var exit = new Procedure(js.engine, 'exit', js.call.native, [js.type.int, js.type.int]);

