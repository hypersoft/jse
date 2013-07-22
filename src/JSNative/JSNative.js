api = {};


// conversion functions return this constant
api.conversionFailure = Object.freeze({converted:false});

var Class = (function( /* name, prototype, methods */ ) {

//#! if the name of api changes, it only needs to be changed here for this scriptlet
var $api = api, SuperConstructor = function(){};

function isConstructorOf(o) { return Boolean(this === o.constructor); }
function hasMethod(name) { return Function.prototype.isPrototypeOf(this[name]); }

var Class = Object.defineProperties(function Class(name, prototype, methods) {
	var api = $api;
	if ( ! Class.prototype.isPrototypeOf(this)) throw new TypeError("Class must be called in a new context");

	if (name in Class.registry) {
		throw new InvokeError("new Class()", "'"+name+"' is already a defined class");
	}	

	if ( prototype && ! Object.prototype.isPrototypeOf(prototype) ) {
		throw new InvokeError("new Class()", "argument 3: expected type object");
	}

	var constructor = {}
	var object = this;
	function invoke() {

		if (constructor.isConstructorOf(this)) {
			if (constructor.hasMethod('create')) {
				var o = new api.createClass(name);
				var deviation = constructor.create.apply(o, arguments);
				if (deviation) return deviation;
				return o;
			} else throw new InvokeError('new '+constructor.name+'()', "create method is not a valid constructor");
		} else if (this === object) {
			if (constructor.hasMethod('global')) {
				var deviation = constructor.global.apply(this, arguments);
				if (deviation) return deviation;
				return;
			} else throw new InvokeError(constructor.name+'()', 'global method is not a valid function');
		}

		throw new InvokeError(constructor.name+'()', 'this call is an unknown invocation method');

	}
	// eval-x-late-it
	invoke = eval('('+invoke.toString().replace('function invoke', 'function '+name)+')');
	constructor = invoke.bind(this);

	Object.defineProperty(constructor, 'prototype', {
		get: function() { return invoke.prototype; },
		set: function(v) {  var p = Object.create(v); p.constructor = constructor; invoke.prototype = p; },
		enumerable:true
	});

	if (prototype) constructor.prototype = Object.create(prototype);
	if (methods) Object.defineProperties(constructor, methods);

	Object.defineProperties(constructor,
		{
			constructor: { value: SuperConstructor },
			isConstructorOf: { value: isConstructorOf.bind(constructor) },
			hasMethod: { value: hasMethod.bind(constructor) },
		}
	)

	api.registerClass(name, constructor);
	return constructor;

}, { registry:{value:{ Class:undefined,Arguments:undefined,Array:undefined,Boolean:undefined,Date:undefined,Error:undefined,EvalError:undefined,Function:undefined,Global:undefined,JSON:undefined,Math:undefined,Number:undefined,Object:undefined,RangeError:undefined,ReferenceError:undefined,RegExp:undefined,String:undefined,SyntaxError:undefined,TypeError:undefined,URIError:undefined}} });

return (SuperConstructor = Object.defineProperties(Class.bind(), {
	"$registry": { get: function() {return Class.registry}, configurable:true },
	"nameIsDefined": { value: (function(name) {return Boolean(name in Class.registry)}).bind() },
	"constructorOf": { value: (function(name) { return Class.registry[name].constructor;}).bind() },
}));

})();
