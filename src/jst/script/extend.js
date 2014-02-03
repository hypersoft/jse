/*

Hypersoft Systems JST AND Hypersoft Systems JSE
Copyright (c) 2014, Triston J. Taylor

All rights reserved.

*/

function Class(interface) {
	var instance, wrapper, typeCheck = typeof interface;
	if (typeCheck !== 'object' ) throw new TypeError(
		typeCheck + " is not a valid Class interface"
	); else if (interface.name === undefined) throw new TypeError(
		"Class interface has no name"
	);
	interface = Object.create(interface),
	instance = sys.class_define(interface);
	wrapper = eval("(\n\
function "+interface.name+"() {\n\
	if (this instanceof arguments.callee)\n\
	return Object.setPrototypeOf(instance.create.apply(\n\
		instance, arguments\n\
	), arguments.callee.prototype);\n\
	else return interface.call.apply(this, arguments);\n\
})");
	interface.constructor = wrapper;
	return wrapper;
};

Object.getPropertyOwner = function (object, property) {
	if (object.hasOwnProperty(property)) return object;
	var o;
	while ((o = Object.getProtoypeOf(o))) 
		if (o.hasOwnProperty(property)) return o;
	return null;
};

Object.hideProperties = function(object, list) {
	var desc, index, name, type = typeof object;
	if (type !== 'object') throw Exception(type+" is not an object")
	else if (!Object.isExtensible(object)) throw Exception(
		"object is not extensible"
	);
	for (index in list) { name = String(list[index]);
		if (object.propertyIsEnumerable(name)) {
			desc = Object.getOwnPropertyDescriptor(object, name);
			if (desc.configurable === false) continue;
			else desc.enumerable = false;
			Object.defineProperty(object, name, desc);
		}
	}
	return object;
};

Object.setPrototypeOf = function(object, proto) {
	return sys.set_prototype(object, proto);
};

/* create a JavaScript Accessor */
var Accessor = function Accessor(object, name, get, set, enumerable, configurable) {
	var descriptor = {};
	
	if (!Object.isExtensible(object))
		throw Exception("object argument is not extensible");
	else if (name === undefined)
		throw Exception("name argument is undefined");
	else if (get && typeof get !== 'function') 
		throw Exception("Property get must be a function");
	else if (set && typeof set !== 'function') 
		throw Exception("Property set must be a function");

	if (get) descriptor.get = get;
	if (set !== undefined) descriptor.set = set;
	if (enumerable === true) descriptor.enumerable = true;
	if (configurable === true) descriptor.configurable = true;
	Object.defineProperty(object, name, descriptor);
	return object;
};

var Accessors = function Accessors(object, list, enumerable, configurable) {
	if (!Array.prototype.isPrototypeOf(list)) throw Exception(
		"Property list must be an array"
	);
	for (var index in list) {
		var item = list[index], len = item.length;
		if (!Array.prototype.isPrototypeOf(item)) throw Exception(
			"Accessor list property index " +index+ " is not an array"
		);
		var enm = (len > 3) ? item[3] : enumerable,
		cfg = (len > 4) ? item[4] : configurable;
		Accessor(object, item[0], item[1], item[2], enm, cfg);
	};
	return object;
};

var Property = function Property(
	object, name, value, enumerable, writable, configurable
) {
	var descriptor = {value: value};
	if (enumerable !== undefined) descriptor.enumerable = Boolean(enumerable);
	if (writable !== undefined) descriptor.writable = Boolean(writable);
	if (configurable !== undefined) descriptor.configurable = Boolean(configurable);
	try {
	Object.defineProperty(object, name, descriptor);
} catch(e) { throw new e.constructor(e.message+": "+name)}
	return object;
};

var Properties = function Properties(object, list, e, w, c) {
	if (typeof object !== 'object' && typeof object !== 'function') {
		throw Exception("Property object must be an object");
	}
	if (list instanceof Array === false) {
		throw Exception("Property list must be an array");
	}
	for (var index in list) {
		var item = list[index];
		if (item instanceof Array === false) throw Exception(
			"Property list item " +index+ " is not an array"
		);
		if (typeof item[0] === 'function') {
			if ('name' in item[0]) {
				item.unshift(item[0].name);
			}	else throw Exception(
				"Attempting to set property "+index+
				" as anonymous function property"
			);
		}
		var enm, wrtb, cfg, id, f1, f2, type = typeof item[0];
		if (type === 'string') {
			id = item.shift();
			type = typeof item[0];
			if (typeof item[0] === 'function' || (item[0] === null && typeof item[1] === 'function')) {
				f1 = item.shift();
				type = typeof item[0];
				if (type === 'function' || item[0] === null) {
					f2 = item.shift();
					enm = (item[0] !== undefined) ? item[0] : e,
					cfg = (item[1] !== undefined) ? item[1] : c;
					Accessor(object, id, f1, f2 || undefined, e, c);
					continue;
				}
			} else f1 = item.shift();
			enm = (item[0] !== undefined) ? item[0] : e,
			wrtb = (item[1] !== undefined) ? item[1] : w,
			cfg = (item[2] !== undefined) ? item[2] : c;
			Property(object, id, f1, enm, wrtb, cfg);
		}
	};
	return object;
};

var Prototype = function(constructor, prototype, properties) {
	if (prototype === undefined) prototype = null;
	if (properties === undefined) properties = [];
	properties.unshift(['constructor', constructor, false, false, false]);
	return Properties(Object.create(prototype), properties, false);
};

/* create a "fully qualified" JavaScript constructor prototype */
Constructor = function Constructor(constructor, properties, prototype) {
	if (properties !== undefined) Properties(constructor, properties);
	if (prototype !== undefined) {
	if (prototype.constructor !== constructor) {
			prototype = Object.create(prototype);
			Property(prototype, 'constructor', constructor, false, false, false);
		}
		Property(constructor, 'prototype', prototype, false, true, false);
	}
	return constructor;
};

Property(Object.prototype, 'isObjectOf', function(constructor){
	if (typeof constructor !== 'function') return false;
	if (typeof constructor.prototype !== 'object') return false;
	return constructor.prototype.isPrototypeOf(this);	
}, false);

Property(String.prototype, 'repeat', function(n){
    n = n || 0; return Array(n+2).join(this);
}, false);

Property(this, 'global', this, true, false, false);