// initialize global utilities

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
	for (i in list) { if ((item = list[i]) in sys.type.code)
		result |= sys.type.code[item]; 
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


