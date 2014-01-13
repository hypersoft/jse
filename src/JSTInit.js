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
	return sys_prototype(object, proto);
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


sys.command = function(command) {
	this.argv = Array.apply(null, arguments);
	var bound = sys_execute.bind(this);
	bound.capture = this;
	bound.argv = this.argv;
	bound.toString = function(){return this.argv[0]};
	return bound;
}

sys.global.exit = sys_exit;

//sys.user = sys.userOf()

// initialize global utilities

//sys.object.property = function method(host, value, accessor, permissions) {

//	if (! Object.isExtensible(host)) throw new TypeError (
//		"expected extensible object, found non-extensible "+typeof host
//	);  else if (! accessor instanceof Function) throw new TypeError (
//		"expected accessor function, found "+typeof accessor
//	);
//	if (permissions == undefined) permissions = [true, true];
//	else if (! permissions instanceof Array) permissions = [
//		permissions, (arguments.length > 4) ? arguments[4]:true
//	]; /* now */ if (permissions[1] == undefined) permissions[1] == true;
//	   /* now */ if (! permissions[0] instanceof Boolean) throw new TypeError (
//		"expected boolean enumerable flag, found"+typeof permissions[0]
//	); else if (! permissions[1] instanceof Boolean) throw new TypeError (
//		"expected boolean configurable flag, found "+typeof permissions[1]
//	);

//	var call, d = {}, property; for (property in value)
//	call = accessor.bind(host, value, property), d[property] = {
//		get:call,					set:call,
//		enumerable:permissions[0],	configurable:permissions[1]
//	};  return Object.defineProperties(host, d);

//};

//sys.object.config = function(perm, host) {
//	if (! Object.isExtensible(host)) throw new TypeError (
//		"expected extensible object, found non-extensible "+typeof host
//	);	var i; for (i = 2; i < arguments.length; i++) {
//		value = arguments[i], pd = Object.getOwnPropertyDescriptor(host, value);
//		if (pd && pd.configurable === true) {
//			pd.configurable = perm[1]; pd.enumerable = perm[0];
//			Object.defineProperty(host, value, pd)
//		}
//	}
//};

//sys.object.map = function data(host, value, permissions){
//	return sys.object.property(host, value, sys.object.map.data, permissions, arguments[3]);
//};

//sys.object.map.data = function(data, name, value) {
//	if (arguments.length == 3) return data[name] = value;
//	return data[name];
//};

//sys.type = function(data) {
//	var i, item, result = 0,
//	list = data.split('*').join(' pointer ').replace(/\s+/g, ' ').split(' ');
//	for (i in list) { if ((item = list[i]) == '') continue;
//		else if (item in sys.type.code) result |= sys.type.code[item]; 
//		else throw new TypeError(item+" is not a system type");
//	};  return result;
//};  sys.type.code = Object.freeze({
//	const: 1, signed: 2, int: 4, struct: 8,
//	union: 16, utf: 32, void: 64, bool: 128,
//	char: 256, short: 512, long: 1024, size: 2048,
//	pointer: 4096, int64: 8192, float: 16384, double: 32768,
//	value: 65536, string: 131072
//}); sys.type.width = {};

//sys.object.config([false, false], sys.type, 'width', 'code');

//sys.engine.toString = function toString(){
//	return sys.engine.vendor + " JSE " +sys.engine.codename+ " v" + sys.engine.version
//};  sys.object.map(sys.engine, {get time(){return Date.now() - sys.date}}, [true, false]);


//// initalize sys.io
//(function(sys_io_path) {

//	sys.object.map(sys.io, {
//		get path(){return sys_io_path()}, set path(v){return sys_io_path(v)},
//	}, [true, false]); delete sys._io_path;

//	// i/o character separators
//	sys.object.property(sys.io, {lfs: '\n', ifs: ' \t\n', ofs: ' '},
//	function separator(data, name, value){
//		if (arguments.length == 3) {
//			if (! value instanceof String) throw new TypeError(
//				"expected string argument, found"+typeof v
//			); return data[name] = value;
//		}
//		return data[name];
//	},[true, false]);

//	// sys.io.stream
//	(function(sys_io_stream_print, sys_io_stream_pointer){
//		
//		// declare
//		sys.io.stream = {
//			get stdin(){return sys_io_stream_pointer(0)},
//			get stdout(){return sys_io_stream_pointer(1)},
//			get stderr(){return sys_io_stream_pointer(2)}
//		};

//		sys.object.map(sys.global, sys.io.stream);

//		sys.io.stream.print = sys_io_stream_print; 
//		sys.io.stream.print.line = function line() {
//			if (arguments.length == 1)
//			return sys.io.stream.print(arguments[0], sys.io.lfs);
//			var argv = Array.apply(null, arguments); argv.push(sys.io.lfs);
//			return sys.io.stream.print.apply(sys.io.stream, argv);
//		};

//	})(sys._io_stream_print, sys._io_stream_pointer);
//	delete sys._io_stream_print, delete sys._io_stream_pointer;

//	sys.io.print = function print() {
//		if (arguments.length == 1)
//		return sys.io.stream.print(sys.io.stream.stdout, arguments[0]);
//		var argv = Array.apply(null, arguments); argv.unshift(sys.io.stream.stdout);
//		return sys.io.stream.print.apply(sys.io.stream, argv);
//	}

//	sys.io.print.line = function line() {
//		if (arguments.length == 1)
//		return sys.io.stream.print(sys.io.stream.stdout, arguments[0], sys.io.lfs);
//		var argv = Array.apply(null, arguments);
//		argv.unshift(sys.io.stream.stdout), argv.push(sys.io.lfs);
//		return sys.io.stream.print.apply(sys.io.stream, argv);
//	};

//	sys.io.print.error = function error() {
//		if (arguments.length == 1)
//		return sys.io.stream.print(sys.io.stream.stderr, arguments[0]);
//		var argv = Array.apply(null, arguments); argv.unshift(sys.io.stream.stderr);
//		return sys.io.stream.print.apply(sys.io.stream, argv);
//	}

//	sys.io.print.error.line = function line() {
//		if (arguments.length == 1)
//		return sys.io.stream.print(sys.io.stream.stderr, arguments[0], sys.io.lfs );
//		var argv = Array.apply(null, arguments);
//		argv.unshift(sys.io.stream.stderr), argv.push(sys.io.lfs);
//		return sys.io.stream.print.apply(sys.io.stream, argv);
//	};

//	sys.error.print = sys.io.print.error,
//	sys.global.io = sys.io, sys.global.print = sys.io.print;

//})(sys._io_path);


// Anything that needs initialized postscript
sys.postScript = function() {

//	var coreType = function coreType(name) {
//		this.name = name,
//		this.value = sys.type.code[name],
//		this.width = sys.type.width[name];
//	};  coreType.prototype = {
//		constructor: sys.type,
//		valueOf:function(){return this.__value__ || this.value},
//		toString:function(){return this.__name__ || this.name}
//	};

//	sys.object.config([false, false], coreType.prototype, 'constructor', 'valueOf', 'toString');
//	for (name in sys.type.code) sys.type[name] = new coreType(name);
//	sys.type.prototype = coreType.prototype;

//	Object.isPointer = function isPointer(test){
//		return test instanceof sys.memory.pointer;
//	}

//	Object.isSystemType = function isSystemType(test){
//		return test instanceof sys.type;
//	}

//	sys.io.stream.buffer.void = function buffer(stream) {
//		return sys.io.stream.buffer(stream, 0, 0, 0);
//	}

//	sys.io.stream.buffer.lines = function buffer(stream) {
//		return sys.io.stream.buffer(stream, 0, 1, 0);
//	}

//	sys.io.stream.buffer.block = function buffer(stream, block) {
//		if (block.constructor != sys.memory.block) throw new TypeError(
//			"sys.io.stream.buffer.block: argument 2: expected sys.memory.block"
//		);
//		return sys.io.stream.buffer(stream, block, 2, block.size);
//	}

//	sys.memory.block.prototype = {
//		constructor: sys.memory.block,
//		valueOf:function(){return this.pointer.address},
//		get free(){return this.pointer.free},
//		clear:function(){sys.memory.clear(this)},
//		get address(){return this.pointer.address},
//		set address(v){this.pointer.address = v},
//		get allocated(){return this.pointer.allocated},
//		set allocated(v){this.pointer.allocated = v},
//		get type(){return this.pointer.type},
//		set type(v){this.pointer.type = v},
//		get width(){return this.pointer.type.width},
//		get size(){return this.pointer.type.width * this.length},
//		get length(){return this.__length__},
//		set length(newLength){
//			if (this.allocated) {
//				var x = sys.memory.resize(this.pointer.address, this.width, newLength);
//				this.pointer.address = x;
//			}
//			this.__length__ = newLength;
//		},
//	}

//	sys.memory.pointer = function(address, type, allocated) {

//		if (!Object.isPointer(this))
//			throw new TypeError("sys.memory.pointer: not called as constructor");

//		if (Object.isSystemType(address)) {
//			allocated = type, type = address;
//			if (allocated == true) address = sys.memory.allocate(type, 1);
//			else address = 0;
//		}

//		this.address = address;
//		this.type = type;
//		this.allocated = Boolean(allocated);

//	}

//	sys.memory.pointer.prototype = {

//		constructor: sys.memory.pointer, allocated: false,

//		__address__: null,
//		set address(value){
//			var v = parseInt(value); if (isNaN(v))
//			throw new TypeError("sys.memory.pointer: set address: property value is not an integer");
//			this.__address__ = v;
//		},
//		get address() {
//			return this.__address__;
//		},

//		__type__: null,
//		set type(value){
//			if (!Object.isSystemType(value))
//				throw new TypeError("sys.memory.pointer: set type: property value is not a system type");
//			this.__type__ = value;
//		},
//		get type(){
//			return this.__type__;
//		},

//		get free(){
//			if (this.allocated != true) return undefined;
//			var object = this;
//			return function free() {
//				sys.memory.free(object);
//				object.address = 0, object.allocated = false;
//			}
//		},

//		valueOf:function(){return this.address},

//		get value(){ // return this pointer content
//			if (this.address === 0) throw new ReferenceError("sys.memory.pointer: cannot dereference null pointer");
//			return sys.memory.read(this.address, this.type);
//		},
//		set value(v){ // set this pointer content
//			if (this.address === 0) throw new ReferenceError("sys.memory.pointer: cannot set null pointer contents");
//			sys.memory.write(this.address, this.type, v);
//		},

//		toBlock:function(length, allocated){
//			length = (arguments.length > 0)? parseInt(length): 1; if (isNaN(length))
//				throw new TypeError("sys.memory.pointer: toBlock: length argument is not an integer");
//			if (arguments.length > 1) this.allocated = allocated;
//			return sys.memory.block(this, length);
//		},

//		// points for style

//		get '*'(){ // return this pointer content
//			return this.value;
//		},
//		set '*'(v){ // set this pointer content
//			this.value = v;
//		},

//	}

//	sys.string.read.line = function(pointer) {
//		var lineAddress = sys.string.indexOf(pointer, "\n");
//		var length = lineAddress - pointer;
//		return sys.string.read(pointer, length);
//	}

//	sys.string.read.field = function(pointer, char) {
//		var lineAddress = sys.string.indexOf(pointer, char);
//		var length = lineAddress - pointer;
//		return sys.string.read(pointer, length);
//	}

	delete sys.postScript;

}


