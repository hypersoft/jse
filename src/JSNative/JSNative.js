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
	Object.defineProperties(self, properties);
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

JSNative.Enumeration = new JSNative.Class("JSNative.Enumeration",
	function invoke(section, query) {
		var o = JSNative.Enumeration.register[section];
		if (o === undefined || query === undefined) return o;
		return o[query];
	},
	function construct(name, kvp) {
		for (value in kvp) {
			if (value == 'length' || value == 'name' || value == 'value') continue;
			this.push(value, kvp[value]);
		}
		Object.defineProperty(JSNative.Enumeration.register, name, { value: this, enumerable: true});
	},
	Object.defineProperties(
		{ // Prototype
			autoEnum: 0, length: 0
		},
		{ // Methods
			push: { value: function(name, value) {
				if (value === undefined) value = this.valueAccumulator();
				var o = new JSNative.api.createClass("JSNative.Enumeration",
				Object.defineProperties(
					{ name: undefined, value: undefined }, // prototype
					{ // prototype methods
						"valueOf": {value: JSNative.Enumeration.valueOfUnit },
						"toString": {value: JSNative.Enumeration.valueOfUnit }
					}
				));
				Object.defineProperties(o, {value: { value: value }, name: {value: name}});
				Object.defineProperty(this, name, { value: o, enumerable:true });
				Object.defineProperty(this, value, { value: o.name, enumerable:true });
				this.length++;
			} },
			valueAccumulator: {value: function valueAccumulator() { return (this.autoEnum++ << 1); } },
			toString: { value: function(name) { if (this.length == 0) return undefined;
					var s = [];
					for (name in this) if (isNaN(name) && name != "autoEnum" && name != 'length' && name != 'name' && name != 'value') s.push(name+':'+this[name]); return '{\n'+s.join(', ')+'\n}';
			} },
		}
	),
	{
		register: {value: {}, enumerable:true},
		valueOfUnit: {value: function valueOfUnit() { return this.value; } },
	}
);

