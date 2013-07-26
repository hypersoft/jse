/* A JSNative Class is just a fancy constructor with some backend support */

(function BootStrap() {

JSNative.Class = function(){}; // createClass needs an object prototype

var construct = function construct(name, prototype, methods, parent) {

	this.name = name, this.prototype = prototype; if (parent) this.parent = parent;

	for (name in methods) {
		if (name == 'initialize') { this.flags |= JSNative.api.classInitialize; continue }
		if (name == 'construct') { this.flags |= JSNative.api.classConstruct; continue }
		if (name == 'invoke') { this.flags |= JSNative.api.classInvoke; continue }
		if (name == 'get') { this.flags |= JSNative.api.classGet; continue }
		if (name == 'set') { this.flags |= JSNative.api.classSet; continue }
		if (name == 'delete') { this.flags |= JSNative.api.classDelete; continue }
		if (name == 'convert') { this.flags |= JSNative.api.classConvert; continue }
		if (name == 'enumerate') { this.flags |= JSNative.api.classEnumerate; continue }
		if (name == 'instanceof') { this.flags |= JSNative.api.classInstanceOf; continue }
	};

	try { native = JSNative.api.createClass(this) }
	catch(e) { throw new InvokeError("new JSNative.Class", e); }

	Object.defineProperties(native, {
		name: {value: this.name}, flags:{value: this.flags},
		prototype: {
			get:function get() {return prototype},
			set:function set(value) {
				prototype = Object.create(value);
				Object.defineProperty(prototype, "constructor", {value:this});
			}
		},
		constructor: {value: this.parent},
	});

	native.prototype = prototype;
	Object.defineProperties(native, methods);

	return native;

}

var self = { name:"JSNative.Class", flags:JSNative.api.classConstruct }

JSNative.Class = JSNative.api.createClass(self);

Object.defineProperties(JSNative.Class, {
	name: {value: self.name}, flags:{value: self.flags},
	prototype: {value: {constructor:JSNative.Class}, writeable:true},
	constructor: {value: JSNative.Class},
	construct: {value: construct}
});

})();


