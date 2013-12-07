// initialize global utilities

sys.error.trace = function(e) {
	var p = e || new Error(); var stack = p.stack.split('\n').reverse()
	stack.toString = function(){
		return 'stack trace: { '+stack.join(' } --> { ').split('@').join(': ')+' }';
	}; return stack;
};

// copy object property values to object with optional config profile
sys.object.extend = function extend(destination, values, config) {
	var type = typeof destination; if (type != 'object' && type != 'function' && type != 'string')
	throw new TypeError("cannot extend "+type);
	if (arguments.length != 3 || config == undefined) config = [true,true,true];
	var record, descriptor = {};
	for (name in values) descriptor[name] = { value: values[name],
		enumerable: (config.enumerable)?config.enumerable:config[0],
		writeable: (config.writeable)?config.writeable:config[1],
		configurable: (config.configurable)?config.configurable:config[2]
	};  return Object.defineProperties(destination, descriptor);
};

// define a property callback, as property method of destination
sys.object.accessor = function accessor(destination, name, accessor, config) {
	// accessor callback prototype: function accessor(name, value){}
	// config may be: undefined, bool, or [bool, bool] to signify enumerable, and configurable
	// all undefined values are taken to be true
	var type = typeof destination; if (type != 'object' && type != 'function' && type != 'string')
	throw new TypeError("cannot create property accessor for "+type);
	if (typeof name != 'string') throw new TypeError(
		"expected property name, found "+typeof name
	);  if (typeof accessor != 'function') throw new TypeError(
		"expected accessor function, found "+typeof accessor
	);  if (arguments.length < 4 || config == undefined) config = [true, true];
	if (config == true || config == false || config == undefined) config = [Boolean(config), true]
	if (config[1] == undefined) config[1] = true;
	var rw = accessor.bind(destination, name), descriptor = {};
	descriptor[name] = { get: rw, set: rw,
		enumerable: (config.enumerable)?config.enumerable:config[0],
		configurable: (config.configurable)?config.configurable:config[1]
	};  return Object.defineProperties(destination, descriptor);
};

// this version does not forward the property name.. hence it is a well known property
sys.object.property = function property(destination, name, accessor, config) {
	// accessor callback prototype: function property(value){}
	// config may be: undefined, bool, or [bool, bool] to signify enumerable, and configurable
	// all undefined values are taken to be true
	var type = typeof destination; if (type != 'object' && type != 'function' && type != 'string')
	throw new TypeError("cannot create property accessor for "+type);
	if (typeof name != 'string') throw new TypeError(
		"expected property name, found "+typeof name
	); if (typeof accessor != 'function') throw new TypeError(
		"expected accessor function, found "+typeof accessor
	); if (arguments.length < 4 || config == undefined) config = [true, true];
	if (config == true || config == false || config == undefined) config = [Boolean(config), true]
	if (config[1] == undefined) config[1] = true;
	var rw = accessor.bind(destination), descriptor = {};
	descriptor[name] = { get: rw, set: rw,
		enumerable: (config.enumerable)?config.enumerable:config[0],
		configurable: (config.configurable)?config.configurable:config[1]
	};  return Object.defineProperties(destination, descriptor);
};

// initalize sys.io
(function(sys_io_path) {

	// sys.io.stream
	(function(sys_io_stream_print, sys_io_stream_pointer){

		// declare
		sys.io.stream = {print:sys_io_stream_print};

		sys.io.stream.print.line = function line() {
			if (arguments.length == 1)
			return sys.io.stream.print(arguments[0], "\n");
			var argv = Array.apply(null, arguments); argv.push('\n');
			return sys.io.stream.print.apply(sys.io, argv);
		};

		// reference
		var stdin = sys_io_stream_pointer.bind(null, 0),
			stdout = sys_io_stream_pointer.bind(null, 1),
			stderr = sys_io_stream_pointer.bind(null, 2);

		// write
		sys.object.property(sys.io.stream, 'stdin', stdin, [true, false]);
		sys.object.property(sys.io.stream, 'stdout', stdout, [true, false]);
		sys.object.property(sys.io.stream, 'stderr', stderr, [true, false]);

		// distribute
		sys.object.property(sys.global, 'stdin', stdin, [true, false]);
		sys.object.property(sys.global, 'stdout', stdout, [true, false]);
		sys.object.property(sys.global, 'stderr', stderr, [true, false]);

	})(sys._io_stream_print, sys._io_stream_pointer);
	delete sys._io_stream_print, delete sys._io_stream_pointer;

	sys.object.accessor(sys.io, 'path', function path(name, value) {
		if (arguments.length == 2) {
			if (sys_io_path(value) == 0) return value;
			throw new Error("unable to change system path: "+String(value)+": "+sys.error.message);
		}
		return sys_io_path();
	}, [true, false]);

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

	sys.global.io = sys.io, sys.global.print = sys.io.print;

})(sys._io_path);
delete sys._io_path;

// initialize sys.error
(function error(sys_error_number, sys_error_message){

	sys.error.print = sys.io.print.error;
	sys.object.property(sys.error, 'number', function number(value) {
		if (value != undefined) return (sys_error_number(value));
		return (sys_error_number());
	}, [true, false]); delete sys._error_number;

	sys.error.clear = function clear(){return ! (sys.error.number = 0)};

	var dual_error_message = function message(i){return (sys_error_message(i))};
	dual_error_message.toString = function toString(){return sys_error_message(sys.error.number)};
	dual_error_message.valueOf = function valueOf(){return sys.error.number};

	sys.object.property(sys.error, 'message', function message(value) {
		if (value != undefined) throw new ReferenceError("attempt to set read only property");
		return dual_error_message;
	}, [true, false]); delete sys._error_message;

})(sys._error_number, sys._error_message);


