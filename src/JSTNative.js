js.exec.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
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

function CallVM(i) {
	var localMode = 0, cache = js.native.callVM;
	Object.defineProperty(this, "mode",
		{get: function() { return localMode; }, set: function(m) {
			localMode = m;
			cache.setMode(this.pointer, localMode);
		}, enumerable:true}
	)
	this.pointer = cache.create(i);
	this.stackSize = i;
}

CallVM.prototype = js.extendPrototype(Object.defineProperties({
	constructor: CallVM,
}, {
	error: {get: function(){return js.native.callVM.getError(this.pointer)}, enumerable:true},
}), {
	free: function() {js.native.callVM.free(this.pointer); this.pointer = null; return true; },
	push: function() {
		var argv = Array.apply(null, arguments);
		argv.unshift(this.pointer);
		js.native.callVM.push.apply(null, argv)
	},
	call: function(rt, fn) {js.native.callVM.call(this.pointer, rt, fn)},
	reset: function() {js.native.callVM.reset(this.pointer); return true; },
});

function exit(num) {
	var jse = new SharedLibrary(); var cvm = new CallVM(4);
	cvm.push([4], num); cvm.call(4, jse.find('exit'));
}

var print = new Command('echo', '-n');
var printf = new Command('printf');
