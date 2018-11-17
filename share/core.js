
run.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
};

loadPlugin("MachineType.jso");

MachineType.create = function(representation){
	for (name in representation) {
		if (this[name] !== undefined) {
			Object.defineProperty(this, name, {value: representation[name], writable:false});
		}
	}
	var width = this.width;
	if (width !== 0 && width !== 1 && width !== 2 && width !== 4 && width !== 8)
		throw new TypeError("invalid machine type width");

	return this;
};

Object.defineProperties(MachineType.prototype, {
	constructor:{value: MachineType}, pointer:{value:false},
	width:{value:0}, constant:{value:false},
	signed:{value:false}, floating:{value:false}, vararg:{value:false},
	utf:{value:false},
	unsigned: {get: function(){
		return this.signed === false;
	}},
	lengthOf: {value: function(count){
		return this.width * count;
	}},
	unitsOf: {value:function(bytes){
		bytes -= bytes % this.width;
		if (bytes < this.width) return 0;
		var data = bytes / this.width;
		return data;
	}},
	bits: {get: function(){
		var width = this.width;
		if (width & (1|2|4|8)) return width << 3;
		return undefined;
	}},
	valueOf: {value:function(){
		var x = this.width;
		if (this.vararg) return 128;
		if (x === 0) return x;
		if (this.constant) x |= 256;
		if (this.pointer) x |= 512;
		else if (this.signed) x |= 1024;
		else if (this.floating) x |= 2048;
		else if (this.utf) x |= 4096;
		return x;
	}},
	toString: {value:function(){
		return this.name;
	}},
	toConst: {value:function(){
		var c = Object.create(this);
		Object.defineProperty(c, "constant", {value:true});
		return c;
	}},
	toPointer: {value:function(){
		var c = Object.create(this);
		Object.defineProperty(c, "pointer", {value:true});
		return c;
	}},
	format: {get(){
		if (this.vararg) return 'e'.charCodeAt(0);
		if (this.pointer) return 'p'.charCodeAt(0);
		if (this.width === 1) {
			return ((this.unsigned)?"C":"c").charCodeAt(0);
		} else if (this.width === 2) {
			return ((this.unsigned)?"S":"s").charCodeAt(0);
		} else if (this.width === 4) {
			if (this.floating) return "f".charCodeAt(0);
			return ((this.unsigned)?"J":"j").charCodeAt(0);
		} else if (this.width === 8) {
			if (this.floating) return "d".charCodeAt(0);
			return ((this.unsigned)?"L":"l").charCodeAt(0);
		} else if (this.width === 0) return "v".charCodeAt(0);
	}},
	name: {get(){
		if (this.vararg) return '...';
		var n = [];
		if (this.constant) n.push('const');
		if (this.signed) n.push('signed');
		if (this.width === 1) {
			n.push('char');
		} else if (this.width === 2) {
			n.push('short');
		} else if (this.width === 4) {
			if (this.floating) n.push('float');
			else if (this.pointer) n.push('void');
			else n.push('int');
		} else if (this.width === 8) {
			if (this.floating) n.push('double');
			else if (this.pointer) n.push('void');
			else n.push('long int');
		}
		if (this.pointer) n.push('*');
		return n.join(' ');
	}},
	readAddress:{value:machineTypeRead},
	writeAddress:{value:machineTypeWrite},
	bindAddress:{value:function(vector, o, id){
		var type = this;
		Object.defineProperty(o, id, {
			get:function(){
				return type.readAddress(vector);
			},
			set:function(value){
				type.writeAddress(vector, value);
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

	Int8: {value: new MachineType({width:1, signed:true})},
	Int16: {value: new MachineType({width:2, signed:true})},
	Int32: {value: new MachineType({width:4, signed:true})},
	Int64: {value: new MachineType({width:8, signed:true})},

	UInt8: {value: new MachineType({width:1})},
	UInt16: {value: new MachineType({width:2})},
	UInt32: {value: new MachineType({width:4})},
	UInt64: {value: new MachineType({width:8})},

	Void: {value: new MachineType({width:0})},
	Float: {value: new MachineType({width:4, floating:true})},
	Double: {value: new MachineType({width:8, floating:true})},
	Pointer: {value: new MachineType({width:MachineType.width, pointer:true})},
	VarArg:{value: new MachineType({vararg:true})}

});

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
	o.type = UInt16;
	o.type.utf = true;
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

loadPlugin("Address.jso");

Address.create = function create(type, length, vector) {
	if (type !== undefined) this.type = type;
	if (vector !== undefined) {
		this.vector = vector;
		if (length !== undefined) this.length = length;
	} else {
		if (length !== undefined) this.length = length;
	}
	return this;
};

Address.prototype = Object.defineProperties({}, {

	constructor: {value:Address, writable:false, enumerable:false, configurable:false},
	vector: {value: 0, writable:true},
	length: {value: 0, writable:true},
	type: {value: Void, writable:true},

	valueOf: {value: function valueOf(){return this.vector;}},
	resizable: {get: function(){return (this.vector === 0 || this.allocated);}},

	bytes: {get: function(){return this.type.width * this.length;}},
	misaligned: {get: function(){return this.vector % this.type.width;}},
	aligned: {get: function(){
		var misaligned = this.misaligned;
		if (misaligned === 0) return this;
		var o = new Address(this.type, this.length, this.vector + (this.type.width - misaligned), this.writable);
		return o;
	}},
	offsetOf: {value: function(element){
		return this.type.offsetOf(element);
	}},
	addressOf: {value: function(element){
		if (element === undefined) element = 0;
		var bytes = this.offsetOf(element), o = new Address(
			this.type, this.length - element, this.vector + bytes
		);
		return o;
	}},
	write: {value: function write(value, length){

		var lengthOf = (length === undefined)?this.length:length;
		if (lengthOf === 0) return 0;
		else if (lengthOf > this.length) lengthOf = this.length;

		if (value.toSerial === undefined) value = [value];
		value = value.toSerial(lengthOf);
		if (lengthOf > value.length) lengthOf = value.length;

		var index = 0;
		for (index; index < lengthOf; index++) this[index] = value[index];
		return index;

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
		if (this.type.utf) {
			String.fromCharCode.apply(String, this);
		}
		if (this.type.floating) return Array.prototype.toString.apply(this, arguments);
		return this.vector;
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
