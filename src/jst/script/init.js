/*

Hypersoft Systems JST AND Hypersoft System JSE Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list 
   of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this 
   list of conditions and the following disclaimer in the documentation and/or other 
   materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGE.

*/

Object.hideProperties = function(object, list) {
	var desc, index, name, type = typeof object;
	if (type !== 'object') throw new TypeError(type+" is not an object").fromCaller(-1);
	else if (!Object.isExtensible(object)) throw new TypeError("object is not extensible").fromCaller(-1);
	for (index in list) { name = String(list[index]);
		if (object.propertyIsEnumerable(name)) {
			desc = Object.getOwnPropertyDescriptor(object, name);
			if (desc.configurable === false) continue;
			else desc.enumerable = false;
			Object.defineProperty(object, name, desc);
		}
	}
	return object;
}

Object.maskPrototype = function(object) {
	return Object.hideProperties(object, [
		'valueOf', 'toString', 'hasOwnProperty', 'constructor', 'toLocaleString',
		'toJSON', 'propertyIsEnumerable', 'isPrototypeOf', 'toObject'
	]);
}

Object.setPrototypeOf = function(object, proto) {
	return sys_set_prototype(object, proto);
}

Flags = function(array, prototype){
	if (array.length >= 31) throw new RangeError("flags exceed integer bits").fromCaller(-1);
	if (prototype === undefined) for (index in array) this[array[index]] = (1 << index);
	else {
		if (typeof prototype != 'object') throw new TypeError("flag prototype != object").fromCaller(-1);
		Object.defineProperty(this, "*prototype", {value:prototype, enumerable:false, writeable:false});
		for (index in array) {
			this[array[index]] = Object.create(prototype, {
				value:{value:(1 << index), writeable:false, configurable:false},
				name:{value:array[index], writeable:false, configurable:false},
				group:{value:this, writeable:false, configurable:false},
			});
		}
	}
}, Flags.prototype = Object.create({}, {
	constructor: {value:Flags, enumerable:false, configurable:false},
	flag:{value:function(name){
		var value = 0, prototype = this["*prototype"] || null;
		if (name in this) throw new ReferenceError().fromCaller(-1);
		if (arguments.length < 2) throw new Error().fromCaller(-1);
		for (i = 1; i < arguments.length; i++) {
			if (typeof arguments[i] === 'string') {
				if (arguments[i] in this === false) throw new ReferenceError("unknown flag name '"+arguments[i]+"' at argument "+i).fromCaller(-1);
				value |= Number(this[arguments[i]])
			} else {
				var number = parseInt(arguments[i]);
				if (isNaN(number)) throw new TypeError("argument "+i+" is not a integer or string");
				if (number > Flags.max || number < Flags.min) throw new RangeError("numeric argument at index "+i+" exceeds value bounds").fromCaller(-1);
				value |= Number(arguments[i]);
			}
		}
		return Object.create(prototype, {
			value:{value:value, writeable:false, configurable:false},
			name:{value:name, writeable:false, configurable:false},
		});
	}, enumerable:false, writeable:false, configurable:false},
	toString: {value:function(head, sum, sep){
		if (head === "C") {
			if (sum === "macros") {
				return this.toString("#define "+sep, " ", "\n");
			}
		}
		if (arguments.length == 3) 	{
			var string = ""
			for(name in this) string += (head + name + sum + Number(this[name]) + sep);
			return string;
		}
		return Object.prototype.toString.call(this);
	}, enumerable:false}
}), Flags.max = (1 << 30), Flags.min = 1;

function Type(model) {

	if (! this instanceof Type)
		throw new TypeError("constructor not called as new").fromCaller(-1);

	if (model === undefined) model = {};
	Object.defineProperty(this, "data", {
		value: new Type.Data(model.data), enumerable:false, writeable:false
	}); delete model.data;

	for (name in model) {
		if (name in this == false)
			throw new TypeError("property name "+name+" is not a valid type property").fromCaller(-1);
		try {this[name] = model[name]} catch(e) {throw e.fromCaller(-1)}
	}

}

Type.duplicate = function(type){
	if (!type instanceof Type) throw new TypeError().fromCaller(-1);
	return new Type({data:type.data});
}

Type.format = {
	'undefined':function(){return this.label},
};

Type.encode = {
	'undefined':function(){return this.flags || 0},
}

Type.Data = function(data){
	var array = new Type.Array2d();
	if (data) { data = Object.create(data);
		data.array = Object.create((data.array)?data.array:array);
		return data;
	}
	this.array = array;
},  Type.Data.prototype = {
	name:undefined, alias:undefined, autoWidth:true, width:undefined,
	reference:undefined, struct:undefined, union:undefined, constant:undefined,
	array:undefined, value:undefined, utf:undefined, integer:undefined, signed:undefined,
	dynamic:undefined, byteOrder:undefined
};

Type.Array2d = function(rows, cols) {
	this.rows = rows, this.cols = cols;
}, Type.Array2d.prototype = {
	rows:undefined, cols:undefined,
	toString:function(){
		var val = "";
		if (this.rows) {
			val = "array["+this.rows+"]"
			if (this.cols != undefined) val += "["+this.cols+"]"
		}
		return val;
	},
	valueOf:function(){
		var val = 0;
		if (this.rows) {
			val = this.rows
			if (this.cols != undefined) val *= this.cols
		}
		return val;
	}
}

Type.limit = function(type, invalid){
	return "cannot change type " + type.label + " to " + invalid + " type"
}

Type.limit.min = function(type, integer) {
	if (Boolean(integer)) {
		if (Boolean(type.unsigned)) return 0;
		var min = 0
		if (type.width == 1) min = -0x80;
		else if (type.width == 2) min = -0x8000;
		else if (type.width == 4) min = -0x80000000;
		else if (type.width == 8) min = -0x8000000000000000;
		return min;
	}
	return Number.MIN_VALUE;
}

Type.limit.max = function(type, integer){
	if (Boolean(integer)) {
		var max = 0
		if (type.width == 1) max = (type.unsigned == true) ? 0xFF : 0x7F
		else if (type.width == 2) max = (type.unsigned == true) ? 0xFFFF : 0x7FFF
		else if (type.width == 4) max = (type.unsigned == true) ? 0xFFFFFFFF : 0x7FFFFFFF
		else if (type.width == 8) max = (type.unsigned == true) ? 0xffffffffffffffff : 0x7fffffffffffffff
		return max;
	}
	return Number.MAX_VALUE;
}

Type.Width = function(number, integer) {
	if (!this instanceof Type.Width) throw new TypeError();
	Object.defineProperty(this, "data", {
		value:{
			width:number, min:null, max:null, bits:null
		},  enumerable:false
	});
	this.data.max = Type.limit.max(this.data, integer), this.data.min = Type.limit.min(this.data, integer);
	this.data.bits = number * 8;
},  Type.Width.prototype = Object.hideProperties({
	get max(){return this.data.max}, get min(){return this.data.min},
	get bits(){return this.data.bits},
	constructor: Type.Width,
	valueOf: function(){return this.data.width},
	toString: function(){return this.data.width}
},  ['constructor', 'valueOf', 'toString']);

Type.align = function(address, type) {
	if (isNaN(address)) throw new TypeError().fromCaller(-1);
	var width = (type instanceof Type) ? Number(type.width) : sys.memory.alignment;
	if (isNaN(width)) throw new TypeError().fromCaller(-1);
	var unaligned = (address % width)
	if (unaligned != 0) {
		return Number(address + (width - unaligned));
	} else return address;
}

Type.prototype = Object.create({
	get array(){return Object.create(this.data.array)},
	set array(v){
		if (this.data.array.rows != undefined) throw new ReferenceError()
		if (typeof v == 'number') {
			this.data.array.rows = v;
		} else if ('rows' in v) {
			this.data.array.rows = v.rows
			if ('cols' in v) {
				if (this.data.array.cols != undefined) throw new ReferenceError()
				this.data.array.cols = v.cols
			}
		}
	},
	get label(){
		var block = this.data.array.toString();
		if (block != "") block = " " + block
		if (this.float) return (this.width == 4)? "single" : "double" + block +" precision float";
		else if (this.boolean) return "boolean" + block;
		else if (this.utf) return this.width.bits + " bit character" + block;
		else if (this.value) return "JavaScript value" + block;
		else if (this.reference) return "pointer" + block + " to " + this.reference.label;
		else if (this.integer && this.width)
			return (this.signed)? "signed " : "unsigned " + this.width.bits + " bit integer" + block;
		return "void";
	},
	get integer() {return Boolean(this.data.integer)},
	set integer(v) {
		if (this.data.integer === undefined) this.data.integer = Boolean(v)
		if (Boolean(this.data.autoWidth)) this.data.width = new Type.Width(4, true);
	},
	get signed() {return Boolean(this.data.signed)},
	set signed(v) {
		var s = "signed", u = "unsigned"; v = Boolean(v);
		if (this.data.integer === undefined) this.integer = true;
		else if (!Boolean(this.data.integer))
			throw new TypeError(Type.limit(this, (v)?s:u)).fromCaller(-1)				
		else if (this.data.signed != undefined)
			throw new TypeError(Type.limit(this, (v)?s:u)).fromCaller(-1)					
		this.data.signed = v;
	},
	get unsigned() {
		return Boolean(this.data.signed === false);
	},
	set unsigned(v) {
		try {this.signed = !Boolean(v)} catch(e) {throw e.fromCaller(-1)}
	},
	get reference(){return this.data.reference || Boolean(false)},
	set reference(v){
		if (v.reference === v) throw new ReferenceError("infinite type recursion detected").fromCaller(-1)
		else if (v === true || v === false) this.data.reference = v;
		else if(!v instanceof Type) throw new ReferenceError();
		else this.data.reference = Type.duplicate(v);
		this.integer = true, this.width = 4;
	},
	get utf(){return this.data.utf || Boolean(false)},
	set utf(v){
		if(v != 8 && v != 16 && v != 32) throw new TypeError().fromCaller(-1);
		if (this.data.utf != undefined || this.data.autoWidth != true)
			throw new TypeError(Type.limit(this, v+" bit utf")).fromCaller(-1);
		if (v == 8 && this.data.signed === undefined) this.signed = true
		else this.signed = false;
		this.data.utf = v, this.width = v/8;
	},
	get width(){return this.data.width || 0},
	set width(v){
		if(!v == 1 || !v == 2 || !v == 4 || !v == 8) throw new TypeError().fromCaller(-1);
		if (this.data.width != undefined && !Boolean(this.data.autoWidth))
			throw new TypeError(Type.limit(this, (v*8)+" bits")).fromCaller(-1)
		this.data.autoWidth = false, this.data.width = new Type.Width(v, this.integer);
	},
	get size(){
		if (!Boolean(this.data.width)) throw new ReferenceError("cannot calculate size of "+this.label).fromCaller(-1);
		var length = Number(this.array) || 1
		return (this.width * length);
	},
	get float(){
		return !this.integer && this.width == 4 || this.width == 8;
	},
	set float(v){
		if (this.width) throw new TypeError(Type.limit(this, "precision floating point")).fromCaller(-1);
		else if (v === 1 || v === 'single' || v === true) this.width = 4;
		else if (v === 2 || v === 'double') this.width = 8;
		else throw new TypeError().fromCaller(-1);
		this.integer = false;
	},
	get boolean() {
		return Boolean(this.integer === true && this.width == 1 && Boolean(this.utf) == false && this.signed === undefined)
	},
	set boolean(v) {
		if (this.width != undefined || this.utf != false)
			throw new TypeError(Type.limit(this, "boolean")).fromCaller(-1);
		else if (!Boolean(v)) throw new TypeError().fromCaller(-1);
		else this.integer = true, this.width = 1;
	},
	get constant(){return Boolean(this.data.constant)},
	set constant(v){
		if (this.data.constant != undefined) throw new TypeError().fromCaller(-1);
		else if (v != true && v != false) throw new TypeError().fromCaller(-1);
		this.data.constant = Boolean(v);
	},
	get value(){return Boolean(this.data.value)},
	set value(v){
		if (this.data.value != undefined) throw new TypeError().fromCaller(-1);
		else if (v != true && v != false) throw new TypeError().fromCaller(-1);
		else if (this.width) throw new TypeError().fromCaller(-1);
		this.data.value = Boolean(v);
	},
}, {
	constructor: {value:Type, enumerable:false},
	toString: {value:function(profile){
		if (profile in Type.format) return Type.format[profile].call(this);
		else throw new ReferenceError("unknown type format: "+profile).fromCaller(-1);
	},  enumerable:false},
	valueOf:{value:function(profile){
		if (profile in Type.encode) return Type.encode[profile].call(this);
		else throw new ReferenceError("unknown type encoder: "+profile).fromCaller(-1);
	},  enumerable:false},
});

Type.flags = new Flags(

	["1", "2", "4", "8", "void", "constant", "integer", "signed", "utf", "value",
	"string", "reference", "array", "struct", "union", "dynamic", "host", "network"],

	Object.create(null, {
		toString: {
			value:function(){return this.name},
			enumerable:false
		},
		valueOf: {
			value:function(){return this.value},
			enumerable:false
		},
	})

);


sys.command = function(command) {
	this.argv = Array.apply(null, arguments);
	var bound = sys_execute.bind(this);
	bound.capture = this;
	bound.argv = this.argv;
	bound.toString = function(){return this.argv[0]};
	return bound;
}

sys.include = sys_include;
sys.global.exit = sys_exit;

sys.class = function(name, construct, exec, prototype) {

	var f = eval("(function "+name+"() {" +
	"	if (this instanceof arguments.callee) {" +
	"		return Object.setPrototypeOf(arguments.callee.construct.apply(null, arguments), arguments.callee.prototype);" +
	"	} else {" +
	"		return Object.setPrototypeOf(arguments.callee.exec.apply(null, arguments), arguments.callee.prototype);" +
	"	}" +
	"})");

	var enterface = Object.defineProperties(f, {
		construct:{value:construct, enumerable:false, writeable:false, configurable:false},
		exec:{value:exec, enumerable:false, writeable:false, configurable:false},
		prototype:{value:prototype,enumerable:false,configurable:false}
	}).bind(this);

	prototype.constructor = enterface;

	return enterface;
}

sys.env = sys.class('env', sys_env_constructor, sys_env_function, {});
