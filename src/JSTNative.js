
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
	constructor: SharedLibrary, retainers: 1,
	retain: function() {this.retainers++},
	release: function() {
		this.retainers--;
		if (this.retainers == 0) {
			js.native.library.free(this.pointer);
			delete js.native.lib[this.name];
			for (name in this) delete this[name];
			return true;
		} else return false;
	},
	find: function(s) {
		if (s in this) return this[s];
		else return this[s] = js.native.library.findSymbol(this.pointer, s);
	},
	valueOf: function() { return this.pointer },
	toString: function() { return this.name },
});

function CallVM(i) {
	var localMode = 0;
	Object.defineProperty(this, "mode",
		{get: function() { return localMode; }, set: function(m) {
			localMode = m;
			js.native.callVM.setMode(this.pointer, localMode);
		}, enumerable:true}
	)
	this.pointer = js.native.callVM.create(i);
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


