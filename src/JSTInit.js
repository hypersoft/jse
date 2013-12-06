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

sys.object.marshal = function marshal(destination, source, names, accessor, config) {
	// accessor callback prototype: function accessor(source, name, value){}
	// destination becomes "this", if value is not supplied, mode is get.
	// if the set operation completes, return the value set.
	var type = typeof destination; if (type != 'object' && type != 'function' && type != 'string')
	throw new TypeError("cannot create property accessor for "+type);
	if (names.constructor != Array) throw new TypeError(
		"expected property name array, found "+typeof name
	);  if (typeof accessor != 'function') throw new TypeError(
		"expected accessor function, found "+typeof accessor
	);  if (arguments.length < 4 || config == undefined) config = [true, true];
	if (config == true || config == false || config == undefined) config = [Boolean(config), true]
	if (config[1] == undefined) config[1] = true;
	var rw = accessor, descriptor = {}, property;
	for (property in names) { property = names[property];
		rwp = rw.bind(destination, source, property);
		descriptor[property] = { get: rwp, set: rwp,
			enumerable: (config.enumerable)?config.enumerable:config[0],
			configurable: (config.configurable)?config.configurable:config[1]
		};  Object.defineProperties(destination, descriptor);
	};  return destination;
};

(function bitmap(){

	// accessor!
	function map(source, name, value) {
		// source.keys == the names in the bitmap
		// source.bits == the maximum number of bits in the bitmap
		var pValue = source.proper, index = source.keys.indexOf(name);
		if (index == -1) return null; else if (arguments.length == 3) {
			if (Boolean(value) === true) pValue |= index; else pValue &= ~index;
			return (source.proper = pValue);
		};  return Boolean(value & index);
	}

	function parse(v){
		if (arguments.length == 0) throw new ReferenceError("expected array, object, or number");
		else if(typeof v === 'string') {
			if (v === 'flags') {
				var number, i, flags, argv = Array.apply(null, arguments); argv.shift();
				for (i = 0, flags = 0; i < argv.length; i++, flags |= (1<<number))
				if (typeof argv[i] == 'number'){ number = argv[i];
					if (number > this.keys.length || number < 0)
					throw new RangeError("bitmap index out of bounds");
					continue;
				} else if (typeof argv[i] == 'string'){	number = this.keys.indexOf(argv[i]);
					if (number == -1) throw new ReferenceError("unknown bitmap key: "+argv[i]);
					continue;
				};  return flags;
			} else if (v === 'max') return this.max;
			else if (v === 'length') return this.keys.length;
			else if (v === 'defines') {
				var source = ""; for (name in this.keys)
					source += "#define jst_type_"+this.keys[name]+" "+(1<<name)+"L\n";
				return source;
			};  throw new ReferenceError("expected flags, length, max or defines: found: "+v);
		} else if (v.constructor == Array) {
			var val, flag, index, pValue = this.proper;
			for (index in v) { if (index >= this.keys.length)
				throw new RangeError("bitmap index out of bounds");
				flag = (1<<index), val = v[index];
				if (val == undefined || val == null) continue;
				else if (typeof val == 'boolean' || typeof val == 'number') {
					if (Boolean(val) === true) pValue |= flag; else pValue &= ~flag;
				} else throw new TypeError("expected number or boolean, found: "+typeof val);
			};  return this.proper = pValue;
		} else if (v.constructor == Object) {
			var ndex, index, pValue = this.proper;
			for (index in v) { if ((ndex = this.keys.indexOf(index)) == -1)
				throw new ReferenceError("'"+v[index]+"' is not a valid map key");
				else if (Boolean(v[index]) === true) pValue |= (1 << ndex);
				else pValue &= ~(1 << ndex);
			};  return this.proper = pValue;
		} else if (typeof v == 'number')
			if (v > this.max || v < 0) throw new RangeError("bitmap value out of bounds");
			else return this.proper = v;
	};

	function stringify(){
		var c, i, o={}, n, v = this.proper;
		for (i in this.keys) { n = this.keys[i];
			if (c = Boolean(v & (1<<i)) === true) o[n] = c;
		};  return JSON.stringify(o);
	}

	function numerize(){return this.proper}

	function construct(data, accessor) {
		if (typeof accessor != 'function') data.proper = parseInt(accessor);
		else sys.object.property(data, 'proper', accessor, [true, false]);
		var t = data.proper; if (t > data.max || t < 0)
		throw new RangeError("initial bitmap value out of bounds");
		var cartograph, access = parse.bind(data);
		cartograph = sys.object.marshal(access, data, data.keys, map, [true, true]);
		sys.object.extend(access, {valueOf:numerize.bind(data), toString:stringify.bind(data)},
			[false, false]
		);  return Object.freeze(cartograph);
	}

	// constructor!
	sys.object.bitmap = function(bits, keys) {
		if (this.constructor != sys.object.bitmap)
		throw new ReferenceError("constructor called without new");
		else if (bits > 32) throw new RangeError("bitmap overflow "+(bits - 32)+" bits");
		else if (keys.length > (bits))
		throw new RangeError("bitmap length overflow "+(keys.length-bits)+" keys");
		var data = {constructor:sys.object.bitmap, bits:bits,
			keys:keys, proper:0, clip:(31 - bits), max: (1 << (bits)) - 1
		},  constructor = construct.bind(this, data);
 		return constructor;
	}, sys.object.bitmap.prototype = sys.object.extend({}, {
		constructor:sys.object.bitmap
	}, [false, false, false]);

})();

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


