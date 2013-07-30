#!bin/jse

// Some utility functions ..
function classFlags(methods) {
	var name, flags;
	for (name in methods) {
		if (name == 'classInitialize') { flags |= JSNative.api.classInitialize; continue }
		if (name == 'classConstruct') { flags |= JSNative.api.classConstruct; continue }
		if (name == 'classInvoke') { flags |= JSNative.api.classInvoke; continue }
		if (name == 'classGet') { flags |= JSNative.api.classGet; continue }
		if (name == 'classSet') { flags |= JSNative.api.classSet; continue }
		if (name == 'classDelete') { flags |= JSNative.api.classDelete; continue }
		if (name == 'classConvert') { flags |= JSNative.api.classConvert; continue }
		if (name == 'classEnumerate') { flags |= JSNative.api.classEnumerate; continue }
		if (name == 'classHasProperty') { flags |= JSNative.api.classHasProperty; continue }
		if (name == 'classInstanceOf') { flags |= JSNative.api.classInstanceOf; continue }
		if (name == 'classAccessor') { flags |= JSNative.api.classAccessor; continue }
	};
	return flags;
}


function extend(o, p) {
	for(prop in p) {
		o[prop] = p[prop];
	}
	return o;
}

function Class(name, instanceMethods, classMethods){

var declareNameSpace = function(name, value) {return eval(name+' = value')}

	var thisClass =	declareNameSpace(
		name, JSNative.api.createClass(name, classFlags(classMethods))
	)

	var prototype;

	JSNative.api.setObjectPrototype(thisClass, Class.prototype)

	Object.defineProperties(thisClass, {
		prototype:{
			set: function(v) {
				prototype = Object.defineProperty(v, "constructor", { value: this })
			},
			get: function() {return prototype}
		}
	})

	thisClass.prototype = extend(
		JSNative.api.createClass(name, classFlags(instanceMethods)),
		instanceMethods
	); delete thisClass.prototype.name;

	return extend(thisClass, classMethods)

}

Class.prototype = Object.defineProperties({}, {
	constructor:{value:Class},
})

new Class(
	"foo",
	{ /* class instance methods (constructor prototype) */
		classInitialize:function(){echo('instance initialize')},
		classInvoke:function(){echo('instance invoked')},
	},
	{ /* class methods (constructor methods) */
		classConstruct:function(){echo('constructor called')},
		classInvoke:function(){echo('class invoked')},
	}
)

echo(new foo())
