/*

Hypersoft Systems JST AND Hypersoft Systems JSE
Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

Object.hideProperties = function(object, list) {
	var desc, index, name, type = typeof object;
	if (type !== 'object') throw new TypeError(type+" is not an object"
	).fromCaller(-1);
	else if (!Object.isExtensible(object)) throw new TypeError(
		"object is not extensible"
	).fromCaller(-1);
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

Object.maskPrototype = function(object) {
	return Object.hideProperties(object, [
		'valueOf', 'toString', 'hasOwnProperty', 'constructor',
		'toLocaleString', 'toJSON', 'propertyIsEnumerable', 'isPrototypeOf',
		'toObject'
	]);
};

/* create a JavaScript Accessor */
var Accessor = function(object, name, get, set, enumerable, configurable) {
	var descriptor = {};
	try {
		if (name === undefined) {
			throw new TypeError("Property name is undefined");
		}
		if (typeof get !== 'function') {
			throw new TypeError("Property get must be a function");
		} else if (typeof set !== 'function') {
			throw new TypeError("Property set must be a function");
		}
		if (get) descriptor.get = get;
		if (set) descriptor.set = set;
		if (enumerable === true) descriptor.enumerable = true;
		if (configurable === true) descriptor.configurable = true;
		Object.defineProperty(object, name, descriptor);
	} catch(e) { throw e.fromCaller(-1); }
	return object;
};

var Accessors = function(object, list, enumerable, configurable) {
	try {
		if (!Array.prototype.isPrototypeOf(list)) {
			throw new TypeError("Property list must be an array");
		}
		for (var index in list) {
			var item = list[index];
			Accessor(
				object, item.name, item.get, item.set,
				enumerable, configurable
			);
		};
	} catch(e) { throw e.fromCaller(-1); }
	return object;
};

var Property = function(
	object, name, value, enumerable, writable, configurable
) {
	var descriptor = {value: value};
	try {
		if (name === undefined) {
			throw new TypeError("Property name is undefined")
		}
		if (enumerable === true) descriptor.enumerable = true;
		if (writable === true) descriptor.writable = true;
		if (configurable === true) descriptor.configurable = true;
		Object.defineProperty(object, name, descriptor);
	} catch(e) { throw e.fromCaller(-1); }
	return object;
};

var Properties = function(object, list, enumerable, writable, configurable) {
	try {
		if (!Array.prototype.isPrototypeOf(list)) {
			throw new TypeError("Property list must be an array");
		}
		for (var index in list) {
			var item = list[index];
			Property(
				object, item.name, item.value,
				enumerable, writable, configurable
			);
		};
	} catch(e) { throw e.fromCaller(-1); }
	return object;
};

/* create a "fully qualified" JavaScript constructor prototype */
var Prototype = function(constructor, prototype) {
	try {
		if (prototype === undefined) prototype = Object.create({});
		return Property(
			prototype, "constructor", constructor,
			false, false, false
		);
	} catch(e) { throw e.fromCaller(-1); }
};

Object.setPrototypeOf = function(object, proto) {
	return sys_set_prototype(object, proto);
};

Flags = function(array, prototype){
	var index;
	if (array.length >= 31) throw new RangeError("flags exceed integer bits"
	).fromCaller(-1);
	if (prototype === undefined)
		for (index in array) this[array[index]] = (1 << index);
	else {
		if (typeof prototype !== 'object') throw new TypeError(
			"flag prototype != object"
		).fromCaller(-1);
		Object.defineProperty(this, "*prototype", {
			value:prototype, enumerable:false, writeable:false
		});
		for (index in array) {
			this[array[index]] = Object.create(prototype, {
				value:{value:(1 << index), writeable:false, configurable:false},
				name:{value:array[index], writeable:false, configurable:false},
				group:{value:this, writeable:false, configurable:false}
			});
		}
	}
}, Flags.prototype = Object.create({}, {
	constructor: {value:Flags, enumerable:false, configurable:false},
	flag:{value:function(name){
		var value = 0, prototype = this["*prototype"] || null;
		if (arguments.length === 1) {
			if (typeof arguments[0] === 'string') {
				name = arguments[0];
				if (! name in this) throw new ReferenceError(
					"couldn't find flag '"+name+"'"
				);
				return this[name];
			} else if (arguments[0].constructor === Array) {
				var result = 0, names = arguments[0];
				for(var index in names) {
					name = names[index];
					if (! name in this) throw new ReferenceError(
						"couldn't find flag '"+name+"'"
					).fromCaller(-1);
					result |= this[name];
				}
				return result;
			}
		} else if (arguments.length > 1) {
			if (name in this) throw new ReferenceError(
				"flag "+name+" already defined"
			).fromCaller(-1);
			for (i = 1; i < arguments.length; i++) {
				if (typeof arguments[i] === 'string') {
					if (arguments[i] in this === false) throw new ReferenceError(
						"unknown flag name '"+arguments[i]+"' at argument "+i
					).fromCaller(-1);
					value |= Number(this[arguments[i]]);
				} else {
					var number = parseInt(arguments[i]);
					if (isNaN(number)) throw new TypeError(
						"argument "+i+" is not a integer or string"
					);
					if (number > Flags.max || number < Flags.min) throw new
					RangeError(
						"numeric argument at index "+i+" exceeds value bounds"
					).fromCaller(-1);
					value |= Number(arguments[i]);
				}
			}
			return Object.create(prototype, {
				value:{value:value, writeable:false, configurable:false},
				name:{value:name, writeable:false, configurable:false}
			});
		}
	}, enumerable:false, writeable:false, configurable:false},
	toString: {value:function(head, sum, sep){
		if (head === "C") {
			if (sum === "macros") {
				return this.toString("#define "+sep, " ", "\n");
			}
		}
		if (arguments.length === 3) 	{
			var string = "";
			for(name in this) string += (
				head + name + sum + Number(this[name]) + sep
			);
			return string;
		}
		return Object.prototype.toString.call(this);
	}, enumerable:false}
}), Flags.max = (1 << 30), Flags.min = 1;

Type = sys_type;

Type.flags = new Flags(

	[
		"1", "2", "4", "8", "void", "constant", "integer", "signed", "utf",
		"value", "string", "reference", "array", "struct", "union", "dynamic"
	],

	Object.create(null, {
		toString: {
			value:function(){
				return this.name;
			},
			enumerable:false
		},
		valueOf: {
			value:function(){
				return this.value;
			},
			enumerable:false
		}
	})

);

// ATM: not being used: see src/type.c: jst_type_get: array
Object.defineProperty(sys.global, "sys_type_array_prototype", {
	value: Object.create(Array.prototype, {
		toString: {value: function() {
		return '['+this.join(", ")+']';
	}, enumerable:false, writable:false, configurable:false }
	}), enumerable:false, writable:false, configurable:false
});

// Super ugly hack, for rediculous bullshit! Probably broke XE some scripts...
Array.prototype.toString = function() {
		return '['+this.join(", ")+']';
};

sys.command = function() {
	this.argv = Array.apply(null, arguments);
	var bound = sys_execute.bind(this);
	bound.capture = this;
	bound.argv = this.argv;
	bound.toString = function(){
		return this.argv[0];
	};
	return bound;
};

sys.include = sys_include;
sys.global.exit = sys_exit;

sys.class = function(name, construct, exec, prototype) {

	var f = eval("(function "+name+"() {" +
	"	if (this instanceof arguments.callee) {" +
	"		return Object.setPrototypeOf(" +
	"			arguments.callee.construct.apply(null, arguments)," +
	"			arguments.callee.prototype" +
	"		);" +
	"	} else {" +
	"		return Object.setPrototypeOf(" +
	"			arguments.callee.exec.apply(null, arguments)," +
	"			arguments.callee.prototype" +
	"		);" +
	"	}" +
	"})");

	var enterface = Object.defineProperties(f, {
		construct:{
			value:construct,
			enumerable:false, writeable:false, configurable:false
		},
		exec:{
			value:exec, enumerable:false, writeable:false, configurable:false
		},
		prototype:{value:prototype,enumerable:false,configurable:false}
	}).bind(this);

	prototype.constructor = enterface;

	return enterface;
};

sys.env = sys.class('env', sys_env_constructor, sys_env_function, {});
