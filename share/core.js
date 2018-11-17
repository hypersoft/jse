
run.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
};

loadPlugin("MachineType.jso");

MachineType.unexpectedTypeWidth = "unexpected type width: ";

// Lookup tables. Entries stored by MachineTypeCode
MachineType.max = {};
MachineType.min = {};
MachineType.format = {};

MachineType.cache = function(type) {
	var 
		code = MachineType.cache.code(type), 
		name = MachineType.cache.typeName(type);
	Object.defineProperties(type, {
		code: {value:code},
		name: {value:name},
		bits: {value:MachineType.cache.bits(type)}
	});
	if (MachineType.max[code] === undefined) {
		MachineType.max[code] = MachineType.cache.max(type);
	}
	if (MachineType.min[code] === undefined) {
		MachineType.min[code] = MachineType.cache.min(type);
	}
	if (MachineType.format[code] === undefined) {
		MachineType.format[code] = MachineType.cache.format(type);
	}
	if (MachineType[name] === undefined) {
		MachineType[name] = type;
	}
}

MachineType.cache.code = function(type){
	var x = type.width;
	if (type.vararg) return MachineType.VARARG;
	if (type.pointer) x |= MachineType.POINTER;
	if (type.constant) x |= MachineType.CONSTANT;
	if (type.signed) x |= MachineType.SIGNED;
	else if (type.floating) x |= MachineType.FLOAT;
	else if (type.utf) x |= MachineType.UTF;
	else if (type.boolean) x |= MachineType.BOOLEAN;
	return x;
}

MachineType.cache.typeName = function(type){
	if (type.vararg) return '...';
	var n = [];
	var code = (type.utf)?"utf":"int";
	if (type.constant) n.push('const');
	if (type.signed) n.push('signed');
	if (type.width === 0) {
		n.push('void');
	} else if (type.width === 1) {
		if (! type.boolean) n.push(code+'8');
		else n.push('bool');
	} else if (type.width === 2) {
		n.push(code+'16');
	} else if (type.width === 4) {
		if (type.floating) n.push('float');
		else n.push(code+'32');
	} else if (type.width === 8) {
		if (type.floating) n.push('double');
		else n.push(code+'64');
	}
	if (type.pointer) n.push('*');
	return n.join(' ');
}

MachineType.cache.bits = function(type){
	if (type.width === 0 && ! type.pointer) return 0;
	var width = (type.width || MachineType.ptrSize);
	if (width & (1|2|4|8)) return width << 3;
	return undefined;
};

MachineType.cache.max = function(obj) {
	if (obj.floating) return undefined;
	var size = obj.bits;
	if (obj.signed) size--;
	return MachineType.flag(size + 1) - 1;
};

MachineType.cache.min = function(obj) {
	if (obj.floating) return undefined;
	if (obj.signed) return -(obj.max + 1);
	return 0;
}

MachineType.cache.format = function(obj) {
	if (obj.vararg) return 'e'.charCodeAt(0);
	if (obj.pointer) return 'p'.charCodeAt(0);
	if (obj.width === 1) {
		return ((obj.unsigned)?"C":"c").charCodeAt(0);
	} else if (obj.width === 2) {
		return ((obj.unsigned)?"S":"s").charCodeAt(0);
	} else if (obj.width === 4) {
		if (obj.floating) return "f".charCodeAt(0);
		return ((obj.unsigned)?"J":"j").charCodeAt(0);
	} else if (obj.width === 8) {
		if (obj.floating) return "d".charCodeAt(0);
		return ((obj.unsigned)?"L":"l").charCodeAt(0);
	} else if (obj.width === 0) return "v".charCodeAt(0);
}

MachineType.leftShift = function (num, bits) {
	return num * Math.pow(2,bits);
}

MachineType.rightShift = function (num, bits) {
	return num / Math.pow(2,bits);
}

MachineType.flag = function(N) {
	return MachineType.leftShift(1, (N - 1));
}

MachineType.flagged = function(C, F) {
	return ((C & F) == F);
}

MachineType.create = function(representation) {

	for (name in representation) {
		if (this[name] !== undefined) {
			Object.defineProperty(this, name, {value: representation[name], writable:false});
		}
	}

	var width = this.width;
	if (width !== 0 && width !== 1 && width !== 2 && width !== 4 && width !== 8)
		throw new TypeError(MachineType.unexpectedTypeWidth + width);

	MachineType.cache(this);

	return this;
};
//
Object.defineProperties(MachineType.prototype, {
	constructor:{value: MachineType}, pointer:{value:false},
	width:{value:0}, constant:{value:false},
	signed:{value:false}, floating:{value:false}, vararg:{value:false},
	utf:{value:false}, boolean:{value:false},
	unsigned: {get: function(){
		return this.signed === false;
	}},
	max: {get(){
		return MachineType.max[this.code];
	}},
	min: {get(){
		return MachineType.min[this.code];
	}},
	sizeOf: {value: function(count){
		if (this.width === 0 && ! this.pointer)
			throw new TypeError(MachineType.unexpectedTypeWidth+ this.width);
		return (this.width || MachineType.ptrSize) * count;
	}},
	unitsOf: {value:function(bytes){
		if (this.width === 0 && ! this.pointer)
			throw new TypeError(MachineType.unexpectedTypeWidth + this.width);
		var width = (this.width || MachineType.ptrSize);
		bytes -= bytes % width ;
		if (bytes < width) return 0;
		var data = bytes / width;
		return data;
	}},
	valueOf: {value:function(){
		return this.code;
	}},
	toString: {value:function(){
		return this.name;
	}},
	toConst: {value:function(){
		if (this.width === 0 && ! this.pointer)
			throw new TypeError(MachineType.unexpectedTypeWidth + this.width);
		if (this.constant) return this;
		var c = Object.create(this);
		Object.defineProperty(c, "constant", {value:true});
		MachineType.cache(c);
		return c;
	}},
	toPointer: {value:function(){
		var c = Object.create(this);
		Object.defineProperty(c, "pointer", {value:true});
		MachineType.cache(c);
		return c;
	}},
	toUtf: {value:function(){
		if (this.width === 0 && ! this.pointer)
			throw new TypeError(MachineType.unexpectedTypeWidth + this.width);
		if (this.boolean)
			throw new TypeError("cannot convert boolean type to utf type");
		if (this.utf) return this;
		var c = Object.create(this);
		Object.defineProperty(c, "utf", {value:true});
		Object.defineProperty(c, "signed", {value:false});
		MachineType.cache(c);
		return c;
	}},
	format: {get(){
		return MachineType.format[this.code]
	}},
	readAddress:{value:MachineType.read},
	writeAddress:{value:MachineType.write},
	bindAddress:{value:function(pointer, o, id){
		var type = this;
		Object.defineProperty(o, id, {
			get:function(){
				return type.readAddress(pointer);
			},
			set:function(value){
				type.writeAddress(pointer, value);
			}
		});
	}},
	value:{value:function(number){
		var o = new Number(number);
		o.type = this;
		return o;
	}}
});

Object.defineProperties(this, {

	Void: {value: new MachineType({width:0})},
	VarArg:{value: new MachineType({width:MachineType.ptrSize, vararg:true})},

	Int8: {value: new MachineType({width:1, signed:true})},
	Int16: {value: new MachineType({width:2, signed:true})},
	Int32: {value: new MachineType({width:4, signed:true})},
	Int64: {value: new MachineType({width:8, signed:true})},

	UInt8: {value: new MachineType({width:1})},
	UInt16: {value: new MachineType({width:2})},
	UInt32: {value: new MachineType({width:4})},
	UInt64: {value: new MachineType({width:8})},

	UIntPtr: {value: new MachineType({width:MachineType.ptrSize})},
	IntSize: {value: new MachineType({width:MachineType.intSize})},

	Float: {value: new MachineType({width:4, floating:true})},
	Double: {value: new MachineType({width:8, floating:true})},

	Bool: {value: new MachineType({width: 1, boolean: true})}

});

Pointer = Void.toPointer();

Object.defineProperty(Array.prototype, "toSerial", {value: function toSerial(max){
	if (this.serialized) return this;
	if (this.length === 1 && this[0].toSerial) return this[0].toSerial(max);
	var o = [];
	o.max = max || 0;
	toSerial.parse.call(o, this);
	delete o.max;
	o.serialized = true;
	return o;
}, enumerable: false});

Array.prototype.toSerial.parse = function parse(value, index){
	var host = typeof value, max = this.max;
	if (index === undefined) index = 0;
	if ((max > 0)?index >= max:0) return 0;
	if (value === null) {
		this[index] = 0;
		return 1;
	} else if (value === undefined) {
		this[index] = value;
		return 1;
	} else if (host === 'number') {
		this[index] = value;
		return 1;
	} else if (host === 'string') {
		var element = 0, length = value.length;
		for (index; ((max > 0)?index < max:1) && element < length;)
			this[index++] = value.charCodeAt(element++);
		return element;
	} else if (host === 'object') {
		if (value.length) {
			var element = 0, length = value.length;
			for (index; ((max > 0)?index < max:1) && element < length;) {
				index += parse.call(this, value[element++], index);
			}
		} else {
			var number = Number(value);
			if (isNaN(number) === false) {
				this[index++] = number;
			} else index += parse.call(this, String(value), index);
		}
		return index;
	}
	throw new TypeError("unhandled type : " + host + " at serial index " + index);
};

Object.defineProperty(String.prototype, "toSerial", {value: function toSerial(max) {
	var data = this.slice(0, max), i = 0, o = new Array(data.length);
	for (i; i < data.length; i++) o[i] = data.charCodeAt(i);
	o.type = UInt16.toUtf();
	o.serialized = true;
	return o;
} , enumerable:false});

(function StringFromCharCodePatch(FromCharCode){
	String.fromCharCode = function fromCharCode() {
		if (arguments.length === 1 && arguments[0].constructor === Array) {
			return FromCharCode.apply(String, arguments[0]);
		} else return FromCharCode.apply(String, arguments);
	};
})(String.fromCharCode);

Address.create = function create(type, length, pointer) {
	if (type !== undefined) this.type = type;
	if (pointer !== undefined) {
		this.pointer = pointer;
		if (length !== undefined) this.length = length;
	} else {
		if (length !== undefined) this.length = length;
	}
	return this;
};

Address.prototype = Object.defineProperties({}, {

	constructor: {value:Address, writable:false, enumerable:false, configurable:false},
	pointer: {value: 0, writable:true},
	length: {value: 0, writable:true},
	type: {value: Void, writable:true},

	valueOf: {value: function valueOf(){return this.pointer;}},
	resizable: {get: function(){return (this.pointer === 0 || this.allocated);}},

	bytes: {get: function(){return this.type.width * this.length;}},
	misaligned: {get: function(){return this.pointer % this.type.width;}},
	aligned: {get: function(){
		var misaligned = this.misaligned;
		if (misaligned === 0) return this;
		var o = new Address(this.type, this.length, this.pointer + (this.type.width - misaligned), this.writable);
		return o;
	}},
	offsetOf: {value: function(element){
		return this.type.sizeOf(element);
	}},
	addressOf: {value: function(element){
		if (element === undefined) element = 0;
		var bytes = this.type.sizeOf(element), o = new Address(
			this.type, this.length - element, this.pointer + bytes
		);
		return o;
	}},
	bind: {value: function bind(object, id){
		return this.type.bindAddress(this, object, id);
	}},
	indexOf: {value: function indexOf(value, start){
		var value = [value].toSerial(2);
		if (value.length > 1) throw new RangeError("too many values");
		return Array.prototype.indexOf.call(this, value[0], start);
	}},
	lastIndexOf: {value: function lastIndexOf(value, start){
		var value = [value].toSerial(2);
		if (value.length > 1) throw new RangeError("too many values");
		return Array.prototype.lastIndexOf.call(this, value[0], start);
	}},
	toBuffer: {value: function toBuffer(type, max){
		if (max === undefined) max = this.length;
		var a = this.addressOf(0);
		a.type = type;
		a.length = type.unitsOf(this.bytes);
		if (a.length > max) a.length = max;
		return a;
	}},
	toSerial: {value: function toSerial(max){
		if (max === undefined) max = this.length;
		var a = this.addressOf(0);
		if (a.length > max) a.length = max;
		return a;
	}},
	toString: {value: function toString() {
		var array = (this.length > 1)?"["+this.length+"]":' *';
		return "((" + this.type.toString() + array + ')(' + this.pointer + "))";
	}}
});

Object.defineProperty(Array.prototype, "toBuffer", {value:function toBuffer(type, max){
		var i, o = this.toSerial(max),
			a = new Address(type, o.length),
			l = (a.length = o.length);
		for (i = 0; i < l; i++) a[i] = o[i];
		return a;
	},
	enumerable: false
});

Object.defineProperty(String.prototype, "toBuffer", {
	value: function toBuffer(type, max){
		return this.toSerial(max).toBuffer(type);
	}, enumerable: false
});

loadPlugin("DynCall.jso");

Object.defineProperties(SharedLibrary.prototype, {
	constructor:{value:SharedLibrary}
});

SharedFunction.create = function(){
	var lib, name, type, pointer, arguments;
	if (arguments[0].constructor === SharedLibrary) {
		lib = arguments[0],
		type = arguments[1],
		name = arguments[2],
		arguments = Array.apply(Array, arguments).slice(3);
		pointer = lib[name];
	}
	this.lib = lib;
	this.name = name, this.returns = type, this.pointer = pointer;
	this.arguments = arguments;
	return this;
};

Object.defineProperties(SharedFunction.prototype, {
	constructor:{value:SharedFunction},
	library:{value:new SharedLibrary(""), writable:true},
	pointer:{value:0, writable:true},
	returns:{value:Void, writable:true},
	arguments:{value:[Void], writable:true},
	protocol:{get:function(){
		var a = []; for (i = 0; i < arguments.length; i++) a.push(arguments[i].format);
		a.push(0);
		return String.fromCharCode.apply(String, a);
	}}
});

loadPlugin("Fork.jso");
