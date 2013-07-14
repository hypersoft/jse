#!bin/jse

JSNative.Prototype = function(name, methodObject) {
	if (name in JSNative.Prototype) {
		return Object.create(JSNative.Prototype[name], methodObject);
	}
	throw new ReferenceError("JSNative.Prototype: prototype unknown");
}

JSNative.Prototype.accept = function (name, objectPrototype) {
	this[name] = objectPrototype;
}

JSNative.Prototype.accept("JSNativeLibrary", {
	constructor: JSNative.Library,
	unloaded: false,
	symbol: { prototype: {} },
	valueOf: function() { return Number(this.pointer); },
	toString: function() { return this.path; },
	unload: function() {
		JSNative.jsnFreeLibrary(this.pointer);
		this.symbol = { prototype: {} };
		this.unloaded = true;
	},
	findSymbol: function(name) {
		if (this.unloaded) return 0;
		if (name in this.symbol) return this.symbol[name];
		return (this.symbol[name] = JSNative.jsnFindSymbol(this, name));
	}
});

JSNative.Prototype.accept("JSNativeFunction", {
	constructor: JSNative.Function,
	library: undefined, name: undefined, symbol: undefined, size: 4096,
	call: { mode: "default", type: "void", argumentType: [] },
	valueOf: function() { return Number(this.symbol); },
});

JSNative.Library = function(path) {
	if (path in JSNative.Library) return JSNative.Library[path];
	var library = JSNative.Prototype("JSNativeLibrary");
	library.path = path;
	JSNative.Library[path] = (library.pointer = JSNative.jsnLoadLibrary(path));
	return library;
}


JSNative.Function = function(library, name, resultType, argumentType) {
	var o = undefined;
	var func = JSNative.Prototype("JSNativeFunction");
	func.library = library; func.name = name;
	func.symbol = library.findSymbol(name);
	func.call.type = JSNative.Type(resultType);
	var arg = argumentType.slice(0);
	while ((o = arg.shift()) != undefined) func.call.argumentType.push(JSNative.Type(o));
	return func;
}

JSNative.Guard = function() { return Object.create(this); }

var libc = new JSNative.Library("libc.so.6");

echo(Number(libc));

