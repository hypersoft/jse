NativeBootStrap = function Class(name, callAsFunction, callAsConstructor, prototype, properties) {

	var api = (classOf(this) === "GlobalObject")?this.api:JSNative.api;

	var item;

	var self = { 
		callAsFunction: callAsFunction, 
		callAsConstructor: callAsConstructor,
		prototype: Object.create(prototype),
		getProperty: function(object, name) { return null; },
		setProperty: function(object, name, val) { return false; },
		convert: function(object, type) { 
			return api.conversionFailure;
		},
	};

	self.construct = function(){};

	self.construct = function() { 
		if (this.callAsFunction === self.callAsFunction) return self.callAsFunction.apply(this, arguments);
		var o = new JSNative.api.createClass(name);
		Object.defineProperty(o, 'constructor', { value: self });
		var init = self.callAsConstructor.apply(o, arguments);
		if (classOf(init) in api.classRegister) return init;
		if (init !== undefined) {
			throw new InvokeError('new '+name, "invalid return type ("+classOf(init)+") from ("+name+") constructor");
			return null;
		}
		return o;
	};

	Object.defineProperty(self.prototype, 'constructor', { value: self.construct.bind(self) });
	for (item in self) self.prototype.constructor[item] = self[item]; self = self.prototype.constructor;
	Object.defineProperty(self, 'className', { value: name, enumerable:true });
	api.registerClass(name, self);
	return self;

}

Object.defineProperty(this, "JSNative", { value: NativeBootStrap('JSNative',
	function() { echo('hello world'); },
	function() { this.foo = "davy jones"; },
	{}, {}
), enumerable:true });

Object.defineProperties(JSNative, {
	api: {value: api, enumerable: true},
	Class: {value: NativeBootStrap, enumerable: true },
}); delete api, NativeBootStrap; // strap up...

JSNative.Type = new JSNative.Class("JSNative.Type",
	function invoke() { echo('JSNative.Type', 'invoked'); },
	function construct() { echo('JSNative.Type', 'construct'); },
	{}, {}
);


