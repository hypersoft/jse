// initialize global utilities

sys.user = sys.userOf();

// sys.env
(function(keys, read, write, erase){

sys.env = sys.object.create({
	name: 'env', keys: 'keys',
	init:function(data, prototype){data.keys = keys()},
	get: function(data, item){ return read(item) },
	set:function(data, item, value){
		return (write(item, value) == 0) ? true : false
	},
	delete:function(data, item){
		return (erase(item) == 0) ? true : false
	},
});

delete sys._env_keys; delete sys._env_delete;
delete sys._env_read; delete sys._env_write;

})(sys._env_keys, sys._env_read, sys._env_write, sys._env_delete);

sys.command = function(command) {
	this.argv = Array.apply(null, arguments);
	var bound = sys.exec.bind(this);
	bound.capture = this;
	bound.argv = this.argv;
	bound.toString = function(){return this.argv[0]};
	return bound;
}

sys.object.property = function method(host, value, accessor, permissions) {

	if (! Object.isExtensible(host)) throw new TypeError (
		"expected extensible object, found non-extensible "+typeof host
	);  else if (! accessor instanceof Function) throw new TypeError (
		"expected accessor function, found "+typeof accessor
	);
	if (permissions == undefined) permissions = [true, true];
	else if (! permissions instanceof Array) permissions = [
		permissions, (arguments.length > 4) ? arguments[4]:true
	]; /* now */ if (permissions[1] == undefined) permissions[1] == true;
	   /* now */ if (! permissions[0] instanceof Boolean) throw new TypeError (
		"expected boolean enumerable flag, found"+typeof permissions[0]
	); else if (! permissions[1] instanceof Boolean) throw new TypeError (
		"expected boolean configurable flag, found "+typeof permissions[1]
	);

	var call, d = {}, property; for (property in value)
	call = accessor.bind(host, value, property), d[property] = {
		get:call,					set:call,
		enumerable:permissions[0],	configurable:permissions[1]
	};  return Object.defineProperties(host, d);

};

sys.object.config = function(host, perm) {
	if (! Object.isExtensible(host)) throw new TypeError (
		"expected extensible object, found non-extensible "+typeof host
	);	var i; for (i = 2; i < arguments.length; i++) {
		value = arguments[i], pd = Object.getOwnPropertyDescriptor(host, value);
		if (pd && pd.configurable === true) {
			pd.configurable = perm[1]; pd.enumerable = perm[0];
			Object.defineProperty(host, value, pd)
		}
	}
};

sys.object.unlist = function(host) {
	var va = Array.apply(null, arguments);
	va.unshift([false, true]);
	sys.object.config.apply(null, va);
};

sys.object.cloak = function names(host) {
	var arg = Object.getOwnPropertyNames(host);
	sys.object.unlist.apply(null, arg);
};

sys.object.map = function data(host, value, permissions){
	return sys.object.property(host, value, sys.object.map.data, permissions, arguments[3]);
};

sys.object.map.data = function(data, name, value) {
	if (arguments.length == 3) return data[name] = value;
	return data[name];
};

sys.type = function(data) {
	var i, item, result = 0,
	list = data.split('*').join(' pointer ').replace(/\s+/g, ' ').split(' ');
	for (i in list) { if ((item = list[i]) == '') continue;
		else if (item in sys.type.code) result |= sys.type.code[item]; 
		else throw new TypeError(item+" is not a type");
	};  return result;
};  sys.type.code = Object.freeze({
	const: 1, signed: 2, int: 4, struct: 8,
	union: 16, utf: 32, void: 64, bool: 128,
	char: 256, short: 512, long: 1024, size: 2048,
	pointer: 4096, int64: 8192, float: 16384, double: 32768,
	value: 65536, string: 131072
}); sys.type.width = {};

sys.object.config([false, false], sys.type, 'width', 'code');

sys.engine.toString = function toString(){
	return sys.engine.vendor + " JSE " +sys.engine.codename+ " v" + sys.engine.version
};  sys.object.map(sys.engine, {get time(){return Date.now() - sys.date}}, [true, false]);

// initialize sys.error
(function error(sys_error_number, sys_error_message){

	sys.error = {
		get number(){return sys_error_number()},
		set number(v){return sys_error_number(v)},
		get message(){return sys_error_message()},
		clear: function clear(){return ! (sys.error.number = 0)},
		trace: function stack(e) {
			var p = e || new Error(); var stack = p.stack.split('\n').reverse()
			stack.toString = function(){
				return 'stack trace: { '+stack.join(' } --> { ').split('@').join(': ')+' }';
			}; return stack;
		},
		toString: function toString(){return sys_error_message(sys_error_number())},
		valueOf: function valueOf(){return sys_error_number()},
	};

})(sys._error_number, sys._error_message);

// initalize sys.io
(function(sys_io_path) {

	sys.object.map(sys.io, {
		get path(){return sys_io_path()}, set path(v){return sys_io_path(v)},
	}, [true, false]); delete sys._io_path;

	// i/o character separators
	sys.object.property(sys.io, {lfs: '\n', ifs: ' \t\n', ofs: ' '},
	function separator(data, name, value){
		if (arguments.length == 3) {
			if (! value instanceof String) throw new TypeError(
				"expected string argument, found"+typeof v
			); return data[name] = value;
		}
		return data[name];
	},[true, false]);

	// sys.io.stream
	(function(sys_io_stream_print, sys_io_stream_pointer){
		
		// declare
		sys.io.stream = {
			get stdin(){return sys_io_stream_pointer(0)},
			get stdout(){return sys_io_stream_pointer(1)},
			get stderr(){return sys_io_stream_pointer(2)}
		};

		sys.object.map(sys.global, sys.io.stream);

		sys.io.stream.print = sys_io_stream_print; 
		sys.io.stream.print.line = function line() {
			if (arguments.length == 1)
			return sys.io.stream.print(arguments[0], sys.io.lfs);
			var argv = Array.apply(null, arguments); argv.push(sys.io.lfs);
			return sys.io.stream.print.apply(sys.io.stream, argv);
		};

	})(sys._io_stream_print, sys._io_stream_pointer);
	delete sys._io_stream_print, delete sys._io_stream_pointer;

	sys.io.print = function print() {
		if (arguments.length == 1)
		return sys.io.stream.print(sys.io.stream.stdout, arguments[0]);
		var argv = Array.apply(null, arguments); argv.unshift(sys.io.stream.stdout);
		return sys.io.stream.print.apply(sys.io.stream, argv);
	}

	sys.io.print.line = function line() {
		if (arguments.length == 1)
		return sys.io.stream.print(sys.io.stream.stdout, arguments[0], sys.io.lfs);
		var argv = Array.apply(null, arguments);
		argv.unshift(sys.io.stream.stdout), argv.push(sys.io.lfs);
		return sys.io.stream.print.apply(sys.io.stream, argv);
	};

	sys.io.print.error = function error() {
		if (arguments.length == 1)
		return sys.io.stream.print(sys.io.stream.stderr, arguments[0]);
		var argv = Array.apply(null, arguments); argv.unshift(sys.io.stream.stderr);
		return sys.io.stream.print.apply(sys.io.stream, argv);
	}

	sys.io.print.error.line = function line() {
		if (arguments.length == 1)
		return sys.io.stream.print(sys.io.stream.stderr, arguments[0], sys.io.lfs );
		var argv = Array.apply(null, arguments);
		argv.unshift(sys.io.stream.stderr), argv.push(sys.io.lfs);
		return sys.io.stream.print.apply(sys.io.stream, argv);
	};

	sys.error.print = sys.io.print.error,
	sys.global.io = sys.io, sys.global.print = sys.io.print;

})(sys._io_path);

sys.global.exit = sys.exit;

// Anything that needs initialized postscript
sys.postScript = function() {

	var coreType = function coreType(name) {
		this.name = name;
		this.value = sys.type.code[name];
		this.width = sys.type.width[name];
	};  coreType.prototype = {
		constructor: sys.type,
		valueOf:function(){return this.value},
		toString:function(){return this.name}
	};	for (name in sys.type.code) sys.type[name] = new coreType(name);

	sys.io.stream.buffer.void = function buffer(stream) {
		return sys.io.stream.buffer(stream, 0, 0, 0);
	}

	sys.io.stream.buffer.lines = function buffer(stream) {
		return sys.io.stream.buffer(stream, 0, 1, 0);
	}

	sys.io.stream.buffer.block = function buffer(stream, block) {
		if (block.constructor != sys.memory.block) throw new TypeError(
			"sys.io.stream.buffer.block: argument 2: expected sys.memory.block"
		);
		return sys.io.stream.buffer(stream, block, 2, block.size);
	}

	sys.memory.block.prototype = {
		constructor: sys.memory.block,
		valueOf:function(){return this.pointer.address},
		get free(){
			return this.pointer.free;
		},
		clear:function(){sys.memory.clear(this)},
		get address(){return this.pointer.address},
		set address(v){this.pointer.address = v},
		get allocated(){return this.pointer.allocated},
		set allocated(v){this.pointer.allocated = v},
		get type(){return this.pointer.type},
		set type(v){this.pointer.type = v},
		get width(){return this.pointer.type.width},
		get size(){return this.pointer.type.width * this.length},
		get length(){return this.__length__},
		set length(newLength){
			if (this.allocated) {
				var x = sys.memory.resize(this.pointer.address, this.width, newLength);
				this.pointer.address = x;
			}
			this.__length__ = newLength;
		},
	}

	Object.isPointer = function isPointer(test){
		return test.constructor == sys.memory.pointer;
	}

	Object.isSystemType = function isSystemType(test){
		return test.constructor == sys.type;
	}

	sys.memory.pointer = function(address, type, allocated) {

		if (!Object.isPointer(this))
			throw new TypeError("sys.memory.pointer: not called as constructor");

		if (Object.isSystemType(address)) {
			allocated = type, type = address;
			if (allocated == true) address = sys.memory.allocate(type, 1);
			else address = 0;
		}

		this.address = address;
		this.type = type;
		this.allocated = Boolean(allocated);

	}

	sys.memory.pointer.prototype = {

		constructor: sys.memory.pointer, allocated: false,

		__address__: null,
		set address(value){
			var v = parseInt(value); if (isNaN(v))
			throw new TypeError("sys.memory.pointer: set address: property value is not an integer");
			this.__address__ = v;
		},
		get address() {
			return this.__address__;
		},

		__type__: null,
		set type(value){
			if (!Object.isSystemType(value))
				throw new TypeError("sys.memory.pointer: set type: property value is not a system type");
			this.__type__ = value;
		},
		get type(){
			return this.__type__;
		},

		get free(){
			if (this.allocated != true) return undefined;
			var object = this;
			return function free() {
				sys.memory.free(object);
				object.address = 0, object.allocated = false;
			}
		},

		valueOf:function(){return this.address},

		get value(){ // return this pointer content
			if (this.address === 0) throw new ReferenceError("sys.memory.pointer: cannot dereference null pointer");
			return sys.memory.read(this.address, this.type);
		},
		set value(v){ // set this pointer content
			if (this.address === 0) throw new ReferenceError("sys.memory.pointer: cannot set null pointer contents");
			sys.memory.write(this.address, this.type, v);
		},

		toBlock:function(length, allocated){
			length = (arguments.length > 0)? parseInt(length): 1; if (isNaN(length))
				throw new TypeError("sys.memory.pointer: toBlock: length argument is not an integer");
			if (arguments.length > 1) this.allocated = allocated;
			return sys.memory.block(this, length);
		},

		// points for style

		get '*'(){ // return this pointer content
			return this.value;
		},
		set '*'(v){ // set this pointer content
			this.value = v;
		},

	}

	delete sys.postScript;

}


