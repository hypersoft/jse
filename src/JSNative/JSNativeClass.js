/* A JSNative Class is just a fancy constructor with some backend support */

JSNative.Class = function Class(name, prototype, methods, parent) {

	this.name = name; this.parent = parent;

	for (name in methods) {
		if (name == 'initialize') { this.flags |= JSNative.api.classInitialize; continue }
		if (name == 'construct') { this.flags |= JSNative.api.classConstruct; continue }
		if (name == 'invoke') { this.flags |= JSNative.api.classInvoke; continue }
		if (name == 'get') { this.flags |= JSNative.api.classGet; continue }
		if (name == 'set') { this.flags |= JSNative.api.classSet; continue }
		if (name == 'delete') { this.flags |= JSNative.api.classDelete; continue }
		if (name == 'convert') { this.flags |= JSNative.api.classConvert; continue }
		if (name == 'instanceof') { this.flags |= JSNative.api.classInstanceOf; continue }
	};

	try { native = JSNative.api.createClass(this) }
	catch(e) { throw new InvokeError("new JSNative.Class", e); }

	native.name = name;
	native.flags = this.flags;
	native.prototype = prototype;
	native.constructor = JSNative.Class;
	Object.defineProperties(native, methods);

	return native;

}

JSNative.Class.prototype = {};
