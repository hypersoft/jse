#!bin/jse

(function NameSpace() {

JSNative.Class = function Class(name, prototype, methods) {

	if (this.constructor != JSNative.Class) { throw new TypeError(); }
	if (classOf(name) != String.name) throw new TypeError("JSNative.Class: argument 1: expected string identifier")

	this.name = name;
	var thisClass = JSNative.api.registerClass.apply(this);
	
	if (prototype) {
		thisClass.prototype = Object.create(prototype);
		thisClass.prototype.constructor = thisClass;		
	}

	if (methods) Object.defineProperties(thisClass, methods);
	return thisClass;

}

JSNative.Class.prototype = Object.create({},
	{
		constructor:{value: JSNative.Class},
	}
)

})();

(function NS() {

var getProperty = function(name) {
	if (name in {autoValue:null,haveEnum:null,'parse':null,'value':null}) return null;
	echo(name)
	return this.value[name];
}
var autoValue = function(v) {
	if (v) return v;
	return (this.auto++ << 1);
}
var parse = function(name, value) {
	if (arguments.length == 1) {
		value = name; for (name in value) parse.call(this, name, this.autoValue(value[name]));
		return;
	} else
	if (arguments.length == 2) {
		value = this.autoValue(value);
		Object.defineProperty(this.value, name,
			{value: new JSNative.Enum.Value(name, value),enumerable:true }
		);  Object.defineProperty(this.value, value,{value:name,enumerable:true});
		return this;
	}
	for(arg in arguments) parse.call(this, arg);

}
var haveEnum = function(name) { return Boolean(this.value[name]); }
var createEnumValue = function(name, value){ this.name = name, this.value = value; }
var createEnum = function(){ this.parse(arguments) };

JSNative.Enum = new JSNative.Class('JSNative.Enum', {
	value:{}, getProperty:getProperty, parse:function(){parse.apply(this, arguments); return this},
	haveEnum:function(){return haveEnum.apply(this,arguments)},
	autoValue:function(){ return autoValue.apply(this, arguments)}
}, {
	auto:{value:0,writeable:true},
	callAsConstructor: {value:createEnum},
	}
);

JSNative.Enum.Value = new JSNative.Class('JSNative.Enum.Value', {
	name:undefined, value:undefined,
	valueOf: function() { return Number(this.value); },
	toString: function() { return this.value; },
}, { callAsConstructor: {value:createEnumValue} }
);

})();

e = new JSNative.Enum();

e.parse("muscle", "man");

echo(e.haveEnum('man'))
