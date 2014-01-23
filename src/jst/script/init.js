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

Object.getPropertyOwner = function (object, property) {
	if (object.hasOwnProperty(property)) return object;
	var o;
	while (o = Object.getProtoypeOf(o)) 
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
	return sys_set_prototype(object, proto);
};

(function init(global){
	
	/* create a JavaScript Accessor */
	var Accessor = function Accessor(object, name, get, set, enumerable, configurable) {
		var descriptor = {};
			if (typeof object !== 'object')
				throw Exception("object argument is not an object");
			else if (!Object.isExtensible(object))
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
		Object.defineProperty(object, name, descriptor);
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
			var enm, wrtb, cfg, id, f1, f2;
			if (typeof item[0] === 'string') {
				id = item.shift();
				if (typeof item[0] === 'function') {
					f1 = item.shift();
					if (typeof item[0] === 'function') {
						f2 = item.shift();
						enm = (item[0] !== undefined) ? item[0] : e,
						cfg = (item[1] !== undefined) ? item[1] : c;
						Accessor(object, id, f1, f2, e, c);
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

	/* create a "fully qualified" JavaScript constructor prototype */
	Constructor = function Constructor(c, proto, methods, props) {
		if (proto === undefined) proto = Object.create({});
		else proto = Object.create(proto);
		if (methods) Properties(c, methods, true);
		if (props) Properties(proto, props);
		Property(proto, "constructor", c, false);
		Property(c, "prototype", proto, false);
		return c;
	};

	var List = Constructor(
		function List(){return arguments.callee.fromArray(arguments);},
		Array.prototype,
		/* constructor methods */ Array (
			[function toString(){
				return this.name;
			}, false],
			[function fromArray(array){
				return Object.create(this.prototype).apply(array);
			}],
			[function fromObjectKeys(object){
				return Object.create(this.prototype).apply(Object.keys(object));
			}],
			[function fromObjectNames(object){
				return Object.create(this.prototype).apply(
					Object.getOwnPropertyNames(object)
				);
			}]
		),
		/* prototype properties */ Array (
			[function invoke(object, method){
					if (arguments.length === 1) {
						method = object, object = null;
					} else if (object instanceof Object === false) throw Exception(
						"List prototype: invoke: expected object argument"
					);
					if (typeof method !== 'function') throw Exception(
						"List prototype: invoke: expected function argument"
					)
				try {
					return method.apply(object, this);
				} catch(e) {
					throw Exception(e.constructor, "List prototype: invoke: "+ e.message);
				}
			}, false],
			[function apply(array){
				if (array && array.length) this.push.apply(this, array);
				return this;
			}, false],
			[function toObject(self){
				if (self == undefined) self = {};
				self = Object.create(self);
				for (var index in this) self[this[index]] = undefined;
				return self;
			}, false],
			[function join(split){
				if (split === undefined) {
					return '['+Array.prototype.join.call(this, ", ")+']';
				}
				return Array.prototype.join.apply(this, arguments);
			}, false],
			[function hasValue(value){
				return this.indexOf(value) !== -1;
			}, false]
		)
	);

	return Properties(global, [
		[Accessor],	[Accessors], [Property], [Properties], [Constructor], [List]
	],	true, false, false);
	
})(sys.global);

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
					"Unknown flag '"+name+"'"
				);
				return this[name];
			} else if (arguments[0].constructor === Array) {
				var result = 0, names = arguments[0];
				for(var index in names) {
					name = names[index];
					if (! name in this) throw new ReferenceError(
						"Unknown flag '"+name+"'"
					).fromCaller(-1);
					result |= this[name];
				}
				return result;
			}
		} else if (arguments.length > 1) {
			if (name in this) throw new ReferenceError(
				"Flag "+name+" already defined"
			).fromCaller(-1);
			for (i = 1; i < arguments.length; i++) {
				if (typeof arguments[i] === 'string') {
					if (arguments[i] in this === false)
						throw new ReferenceError(
							"Unknown flag '"+ arguments[i] +"' at argument " + i
						).fromCaller(-1);
					value |= Number(this[arguments[i]]);
				} else {
					var number = parseInt(arguments[i]);
					if (isNaN(number)) throw new TypeError(
						"Argument "+i+" is not a integer or string"
					);
					if (number !== 0 &&
						(number > Flags.max || number < Flags.min)
					) throw new RangeError(
						"Numeric argument at index "+i+" exceeds value bounds"
					).fromCaller(-1);
					value |= Number(arguments[i]);
				}
			}
			return (this[name] = Object.create(prototype, {
				value:{value:value, writeable:false, configurable:false},
				name:{value:name, writeable:false, configurable:false}
			}));
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

//sys.class = function(name, construct, exec, prototype) {
//
//	var f = eval("(function "+name+"() {" +
//	"	if (this instanceof arguments.callee) {" +
//	"		return Object.setPrototypeOf(" +
//	"			arguments.callee.construct.apply(null, arguments)," +
//	"			arguments.callee.prototype" +
//	"		);" +
//	"	} else {" +
//	"		return Object.setPrototypeOf(" +
//	"			arguments.callee.exec.apply(null, arguments)," +
//	"			arguments.callee.prototype" +
//	"		);" +
//	"	}" +
//	"})");
//
//	var enterface = Object.defineProperties(f, {
//		construct:{
//			value:construct,
//			enumerable:false, writeable:false, configurable:false
//		},
//		exec:{
//			value:exec, enumerable:false, writeable:false, configurable:false
//		},
//		prototype:{value:prototype,enumerable:false,configurable:false}
//	}).bind(this);
//
//	prototype.constructor = enterface;
//
//	return enterface;
//};
//
//sys.env = sys.class('env', sys_env_constructor, sys_env_function, {});
