
new JSNative.Type.Alias(0, "void");
new JSNative.Type.Alias(10, "bool");

new JSNative.Type.Alias(20, "char");
new JSNative.Type.Alias(20, "signed char");
new JSNative.Type.Alias(20, "char signed");
new JSNative.Type.Alias(21, "unsigned char");
new JSNative.Type.Alias(21, "char unsigned");

new JSNative.Type.Alias(30, "short");
new JSNative.Type.Alias(30, "signed short");
new JSNative.Type.Alias(30, "short signed");
new JSNative.Type.Alias(31, "unsigned short");
new JSNative.Type.Alias(31, "short unsigned");

new JSNative.Type.Alias(40, "int");
new JSNative.Type.Alias(40, "signed int");
new JSNative.Type.Alias(40, "int signed");
new JSNative.Type.Alias(41, "unsigned int");
new JSNative.Type.Alias(41, "int unsigned");

new JSNative.Type.Alias(50, "long");
new JSNative.Type.Alias(50, "signed long");
new JSNative.Type.Alias(50, "long signed");
new JSNative.Type.Alias(51, "unsigned long");
new JSNative.Type.Alias(51, "long unsigned");

new JSNative.Type.Alias(60, "long long");
new JSNative.Type.Alias(60, "signed long long");
new JSNative.Type.Alias(60, "long long signed");
new JSNative.Type.Alias(61, "unsigned long long");
new JSNative.Type.Alias(61, "long long unsigned");

new JSNative.Type.Alias(70, "float");
new JSNative.Type.Alias(80, "double");

new JSNative.Type.Alias(90, "pointer");
new JSNative.Type.Alias(90, "void *");

JSNative.Prototype = function(name, object) {
	if ( this instanceof JSNative.Prototype ) {
		JSNative.Prototype[name] = Object.create({}, object);
		return JSNative.Prototype[name];
	}
	if (name in JSNative.Prototype) {
		return Object.create(JSNative.Prototype[name]);
	} else throw new ReferenceError("JSNative.Prototype: unknown prototype predicate");
}
JSNative.Library = function(path) {
	if (path in JSNative.Library) return JSNative.Library[path];
	if (this instanceof JSNative.Library) {
		this.path = path;
		this.pointer  = JSNative.jsnLoadLibrary(path);
		JSNative.Library[path] = this;
	}
}
JSNative.Library.prototype = new JSNative.Prototype("JSNative.Library", {
	constructor: { value: JSNative.Library },
	valueOf: { value: function() { return Number(this.pointer); } },
	toString: { value: function() { return String(this.path); } },
	pointer: { value: 0, writeable: true, enumerable: true },
	path: { value: "", writeable: true, enumerable: true },
	symbol: { value: {}, writeable:true, enumerable:true },
	unload: { value: function() {
		if (this.pointer != 0) JSNative.jsnFreeLibrary(this.pointer);
		this.pointer = 0;
		this.symbol = {};
	}, enumerable: true },
	findSymbol: { value: function(name) {
		if (name in this.symbol) return this.symbol[name];
		if (this.pointer != 0) {
			var sym = JSNative.jsnFindSymbol(this, name); sym.name = name;
			this.symbol[name] = sym;
			return sym;
		}
	}, enumerable:true }
});

