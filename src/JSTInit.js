// initialize global utilities

sys.engine.toString = function toString(){
	return sys.engine.vendor + " JSE " +sys.engine.codename+ " v" + sys.engine.version
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
	var property; for (property in value) accessor = accessor.bind(host, value, property),
	Object.defineProperty(host,		property, {
		get:accessor,				set:accessor,
		enumerable:permissions[0],	configurable:permissions[1]
	}); return host;

};  sys.object.map = function data(host, value, permissions){
	return sys.object.property(host, value, sys.object.map.auto, permissions, arguments[3]);
};  sys.object.map.auto = function property(data, name, value) {
	if (arguments.length == 3) return data[name] = value;
	return data[name];
};


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

	// sys.io.stream
	(function(prop, sys_io_stream_print, sys_io_stream_pointer){

		// declare
		sys.io.stream = {
			print:sys_io_stream_print,
			get stdin(){return sys_io_stream_pointer(0)},
			get stdout(){return sys_io_stream_pointer(1)},
			get stderr(){return sys_io_stream_pointer(2)}
		};

		sys.io.stream.print.line = function line() {
			if (arguments.length == 1)
			return sys.io.stream.print(arguments[0], "\n");
			var argv = Array.apply(null, arguments); argv.push('\n');
			return sys.io.stream.print.apply(sys.io.stream, argv);
		};
		
	})(sys.object.property, sys._io_stream_print, sys._io_stream_pointer);
	delete sys._io_stream_print, delete sys._io_stream_pointer;

	sys.object.map(sys.io, {
		get path(){return sys_io_path()},
		set path(v){return sys_io_path(v)},
	}, [true, false]); delete sys._io_path;

	sys.io.print = function print() {
		if (arguments.length == 1)
		return sys.io.stream.print(sys.io.stream.stdout, arguments[0]);
		var argv = Array.apply(null, arguments); argv.unshift(sys.io.stream.stdout);
		return sys.io.stream.print.apply(sys.io.stream, argv);
	}

	sys.io.print.line = function line() {
		if (arguments.length == 1)
		return sys.io.stream.print(sys.io.stream.stdout, arguments[0], "\n");
		var argv = Array.apply(null, arguments);
		argv.unshift(sys.io.stream.stdout), argv.push('\n');
		return sys.io.stream.print.apply(sys.io, argv);
	};

	sys.io.print.error = function error() {
		if (arguments.length == 1)
		return sys.io.stream.print(sys.io.stream.stderr, arguments[0]);
		var argv = Array.apply(null, arguments); argv.unshift(sys.io.stream.stderr);
		return sys.io.stream.print.apply(sys.io.stream, argv);
	}

	sys.io.print.error.line = function line() {
		if (arguments.length == 1)
		return sys.io.stream.print(sys.io.stream.stderr, arguments[0], "\n");
		var argv = Array.apply(null, arguments);
		argv.unshift(sys.io.stream.stderr), argv.push('\n');
		return sys.io.stream.print.apply(sys.io, argv);
	};

	sys.error.print = sys.io.print.error,
	sys.global.io = sys.io, sys.global.print = sys.io.print;

})(sys._io_path);

