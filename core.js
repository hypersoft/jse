#!bin/jse

JSNative.Library = function(path) {
	this.path = path;
	this.pointer = JSNative.jsnLoadLibrary(path);	
}

JSNative.Library.prototype = {
	constructor: JSNative.Library,
	unloaded: false,
	symbol: {
		prototype: {}
	},
	toValue: function() {
		return this.pointer;
	},
	toString: function() {
		return this.path;
	},
	unload: function() {
		JSNative.jsnFreeLibrary(this.pointer);
		this.symbol = { prototype: {} };
		this.unloaded = true;
	},
	findSymbol: function(name) {
		if (this.unloaded) return 0;
		if (name in this.symbol) return this.symbol[name];
		var sym = JSNative.jsnFindSymbol(this, name);
		if (sym != 0) this.symbol[name] = sym;
		return this.symbol[name];
	}
}

JSNative.Function = function(library, name, resultType, argumentType) {
	this.library = library;
	this.name = name;
	this.symbol = library.findSymbol(name);
	this.call.type = JSNative.Type(resultType);
	var arg = argumentType.slice(0);
	var o = undefined;
	while ((o = arg.shift()) != undefined) this.call.argumentType.push(JSNative.Type(o));
}

JSNative.Function.prototype = {
	constructor: JSNative.Function,
	library: undefined, name: undefined, symbol: undefined, size: 4096,
	call: { mode: "default", type: "void", argumentType: [] },
	valueOf: function() { return this.symbol; },
};

var libc = new JSNative.Library("libc.so.6");

echo(libc.findSymbol("stdin"));

