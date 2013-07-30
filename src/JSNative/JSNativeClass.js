
(function(){

var declareNameSpace = function(name, value) {return eval(name+' = value')};

var Class = function Class(name, instanceMethods, classMethods){

	var flags = JSNative.api.classFlags(classMethods);
	var constructorClass = (flags & JSNative.api.classConstruct)?"Constructor":(flags & JSNative.api.classInvoke)?"Function":name

	var thisClass =	declareNameSpace(name,
		JSNative.api.createClass(constructorClass, flags)
	);	JSNative.api.setObjectPrototype(thisClass, JSNative.Class.prototype)

	var prototype = JSNative.api.createClass(name, JSNative.api.classFlags(instanceMethods));
	delete prototype.name; extendPrototype(prototype, instanceMethods);
	Object.defineProperty(prototype, 'constructor', {value:thisClass});

	/*
		Class prototypes are read-only, they hold the class instance reference in private.
		methods and properties of the class's prototype are "open season"
	*/

	Object.defineProperties(thisClass, {prototype:{get: function() {return prototype}}})

	// Not sure why but classMethods need to be bound to the class
	for (name in classMethods) thisClass[name] = classMethods[name].bind(thisClass);

	return thisClass; //extend(thisClass, classMethods)

}

JSNative.Class = Class.bind()

Object.defineProperty(JSNative.Class, "prototype", {
	get:function(){return Class.prototype},
	set:function(v){Class.prototype=Object.defineProperty(v, 'constructor', {value:JSNative.Class})}
});

JSNative.Class.prototype = {};

})();


