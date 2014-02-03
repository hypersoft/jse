/*

Hypersoft Systems JST AND Hypersoft Systems JSE
Copyright (c) 2014, Triston J. Taylor

All rights reserved.

*/

// fatal return codes, modify these on the newly constructed
// object for custom exit error codes.
// 1 is left for custom errors in cases where you need a 1 or 0 exit status.
Error.prototype.code = 2;
SyntaxError.prototype.code = 3;
TypeError.prototype.code = 4;
RangeError.prototype.code = 5;
ReferenceError.prototype.code = 6;
EvalError.prototype.code = 7;
URIError.prototype.code = 8;


Exception = (function(Error) {
	
	var Exception = function(c, m) {

		if (typeof c === 'string') {
			m = c, c = Error;
		}

		if (!m) m = "";
		if (!c) c = Error;

		var error;
		try {
			if (typeof c === 'function') throw new c(m);
			error = c;
		} catch(e) {
			error = e;
			var stack = e.stack.split('\n');
			stack.shift();
			error.stack = stack.join('\n');
		}

		var ex = Object.create(error);
		if (String(m).length !== 0) ex.message = String(m);

		if (typeof error.stack === 'string') {
			var stack = error.stack.split('\n');
			var root = stack[0].replace('@', ':').split(':');
			if (root.length !== 2) {
				root[root.length - 1] = "column "+root[root.length - 1];
				root[root.length - 2] = "line "+root[root.length - 2];
			}
			ex.head = root.join(': '), ex.sourceURL = root[1],
			ex.line = root[2];
		}

		return Object.freeze(ex);

	};

	Exception.prototype = Error.prototype;

	Exception.stackString = function(e) {
		var stack = e.stack.replace(/@/g, ':').split('\n');
		var string = [];
		while (x = stack.shift()) {
			var data = x.split(":");
			string.push(data[0].replace(
				"global code", "source")+ ": "+data[1]+
				((data[2] !== undefined) ? ": line "+data[2] : "")+
				((data[3] !== undefined) ? ": column "+data[3] : "" ));
		}
		if (string.length) return string.join('\n');
		else return undefined;
	};

	return Exception;
	
})(Error);


// This must be defined here in this file, as if something goes wrong, the error
// reporter won't operate as intended without this definition.

sys.error = {
	get number(){
		return sys.error_number();
	},
	set number(v){
		return sys.error_number(v);
	},
	get message(){
		return sys.error_message();
	},
	clear: function clear(){
		return ! sys.error_number(0);
	},
	toString: function toString(){
		return sys.error_message(sys.error_number());
	},
	valueOf: function valueOf(){
		return sys.error_number();
	}
};