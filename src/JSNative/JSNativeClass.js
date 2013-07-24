
// conversion functions return this constant
api.conversionFailure = Object.freeze({converted:false});

api.classConstructor = 2, api.classFunction = 4, api.classGet = 8,
api.classSet = 16, api.classConvert = 32, api.classManualPrototype = 64;
api.classAccessor = api.classSet | api.classGet;

JSNative = { api:api };

JSNative.api.prototype = {

	methodList:function methodList() {
		var list = {};
		for (name in this)
			if (classOf(this[name]) == Function.name) list[ this[name].name ] = null;
		return list;
	},
	hideProperties:function hideProperties(keys) {
		if (!keys) keys = this
		for (name in keys) {
			try { 
				Object.defineProperty(this, name, {
					value:this[name], writeable:true,enumerable:false,configurable:true
				})
			} catch(e) { continue; }
		}
		return this
	},
	setClassPrototype:function setClassPrototype(prototype) {
		var prototype = Object.defineProperty(Object.create(prototype), "constructor", {value:this}
		);
	},
	embedProperty:function embedProperty(name, object, key, relay) {
		if (! relay ) relay = new JSNative.api.prototype.smartProperty();
		if (! key ) key = name;
		return Object.defineProperty(this, name, {
			get:function(){return relay.get.call(object, key)},
			set:function(v){return relay.set.call(object, key, value)},
			enumerable:true,configurable:true
		})
	},
	embedValuesOf:function embedValuesOf() {
		var argv = []; argv.push.apply(argv, arguments);
		var object = argv.shift(); var arg;
		while((arg = argv.shift())) {
			if (arg.constructor != JSNative.api.prototype.smartProperty) {
				if (classOf(arg) == String.name) arg = new JSNative.api.prototype.smartProperty(arg);
				else if (Array.prototype.isPrototypeOf(arg)) {
					a = [object]; a.push.apply(a, arg);
					JSNative.api.prototype.embedValuesOf.apply(this, a);
				}
				else for (name in arg) JSNative.api.prototype.embedProperty.call(this, name, object, name)
			} else JSNative.api.prototype.embedProperty.call(this, arg.name, object, arg.name, arg)
		}
		return this;
	},
	applyValuesOf:function applyValuesOf(object, keys) {
		if (!keys) keys = object;
		for (name in keys) this[name] = object[name];
	},
	smartProperty:Object.defineProperty(
	function smartProperty(name, get, set) {
		if (name) this.name = name; if (get) this.get = get; if (set) this.set = set;
	}, "prototype", {value:{
		name:'default',
		get:function(name) { return this[name]; },	// default read
		set:function(name, value) { return }		// default read only
	} /* constant */ }),

}; JSNative.api.prototype.hideProperties();

