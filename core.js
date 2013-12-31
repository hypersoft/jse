#!bin/jse

function Type(model) {

	if (! this instanceof Type)
		throw new TypeError("constructor not called as new").fromCaller(-1);

	Object.defineProperty(this, "data", {value: new Type.Data(), enumerable:false});
	for (name in model) {
		if (!Boolean(name in this)) throw new TypeError("property name "+name+" is not a type property").fromCaller(-1);
		try {
			this[name] = model[name];
		} catch(e) {throw e.fromCaller(-1)}
	}

}

Type.Data = function(){}

Type.Data.prototype = {
	name:undefined, alias:undefined, autoWidth:true, width:undefined,
	reference:undefined, struct:undefined, constant:undefined,
	value:undefined, utf:undefined, integer:undefined, signed:undefined,
	max:0, min:0
}

Type.limit = function(type, invalid){
	return "cannot change type "+type.label+" to type "+invalid
}

Type.limit.min = function(type) {
	if (Boolean(type.integer)) {
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

Type.limit.max = function(type){
	if (Boolean(type.integer)) {
		var max = 0
		if (type.width == 1) max = (type.unsigned == true) ? 0xFF : 0x7F
		else if (type.width == 2) max = (type.unsigned == true) ? 0xFFFF : 0x7FFF
		else if (type.width == 4) max = (type.unsigned == true) ? 0xFFFFFFFF : 0x7FFFFFFF
		else if (type.width == 8) max = (type.unsigned == true) ? 0xffffffffffffffff : 0x7fffffffffffffff
		return max;
	}
	return Number.MAX_VALUE;
}

Type.prototype = Object.create({
	get max(){return this.data.max}, get min(){return this.data.min},
	get label(){
		var bits = (this.width*8)+" bit";
		if (this.float) return String(this.float)+" precision float";
		else if (this.boolean) return "boolean";
		else if (this.utf) return bits+" character";
		else if (this.value) return "JavaScript value";
		else if (this.reference) return "pointer to "+this.reference.label;
		else if (this.integer && this.width) return (this.signed)?"signed ":"unsigned "+bits+" integer";
		return "void";
	},
	get integer() {
		var self = this, type = this.data;
		return {
			valueOf: function(){return type.integer},			
			toString: function(){return String(type.integer)},
			get signed(){return self.signed;},
			set signed(v){try {self.signed = v} catch(e) {throw e.fromCaller(-1)}},
			get unsigned(){return self.unsigned},
			set unsigned(v){try {self.signed = !Boolean(v)} catch(e) {throw e.fromCaller(-1)}},
		};
	},
	set integer(v) {
		if (this.data.integer === undefined) this.data.integer = Boolean(v)
		if (Boolean(this.data.autoWidth)) this.data.width = 4;
	},
	get signed() {return Boolean(this.data.signed)},
	set signed(v) {
		v = Boolean(v);
		if (this.data.integer === undefined) this.integer = true;
		else if (!Boolean(this.data.integer))
			throw new TypeError(Type.limit(this, (v)?"signed":"unsigned")).fromCaller(-1)				
		else if (this.data.signed != undefined)
			throw new TypeError(Type.limit(this, (v)?"signed":"unsigned")).fromCaller(-1)					
		this.data.signed = v,
		this.data.max = Type.limit.max(this),
		this.data.min = Type.limit.min(this);
	},
	get unsigned() {
		return Boolean(this.data.signed === false);
	},
	set unsigned(v) {
		try {this.signed = !Boolean(v)} catch(e) {throw e.fromCaller(-1)}
	},
	get reference(){return this.data.reference || Boolean(false)},
	set reference(v){
		if (v === true || v === false) this.data.reference = v;
		else if(!v instanceof Type) throw new ReferenceError();
		else this.data.reference = v;
		this.integer = true, this.data.width = 4, this.data.autoWidth = false;
	},
	get utf(){return this.data.utf || Boolean(false)},
	set utf(v){
		if(!v == 8 || !v == 16 || !v == 32) throw new TypeError().fromCaller(-1);
		if (this.data.utf != undefined || this.data.autoWidth != true)
			throw new TypeError(Type.limit(this, v+" bit utf")).fromCaller(-1);
		this.width = v/8;
		if (v == 8 && this.data.signed === undefined) this.signed = true
		else this.signed = false;
		this.data.utf = v
	},
	get width(){return this.data.width || 0},
	set width(v){
		if(!v == 1 || !v == 2 || !v == 4 || !v == 8) throw new TypeError().fromCaller(-1);
		if (this.data.width != undefined && !Boolean(this.data.autoWidth))
			throw new TypeError(Type.limit(this, (v*8)+" bits")).fromCaller(-1)
		this.data.autoWidth = Boolean(false), this.data.width = v,
		this.data.max = Type.limit.max(this),
		this.data.min = Type.limit.min(this);
	},
	get float(){
		type = this;
		if (type.data.integer) return Boolean(false);
		return {
			valueOf: function(){
				if (type.width == 4) return 1; // single precision float
				else if (type.width == 8) return 2; // double precision float
				return null;
			},
			toString: function(){
				if (type.width == 4) return 'single'; // single precision float
				else if (type.width == 8) return 'double'; // double precision float
				print.line(type.width);
				return null;
			}
		};
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
		var current = "", bits = (this.width * 8) + " bit"
		if (this.constant) current = "constant ";
		if (this.float) current += String(this.float);
		else if (Boolean(this.integer)) {
			if (this.utf) current = "utf "+this.utf
			else if (this.unsigned) current += "unsigned "+bits+" integer"
			else if (this.signed) current += "signed "+bits+" integer"
			else if (this.boolean) current += "boolean"
			else current += bits+" integer"
			if (this.reference) current += " pointer"
			if (this.reference instanceof Type) current += " to "+this.reference
		}
		return current.replace(/\s+/, " ");
	}, enumerable:false}
});

utf8 = new Type({utf:16,width:1});
utf32 = new Type({integer:true,width:1});
x = new Type({reference: new Type({float:2})});
y = new Type({reference: utf32});

print.line(x.name)
