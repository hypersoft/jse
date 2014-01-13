
// fatal return codes, modify these on the newly constructed
// object for custom exit error codes.
// 1 is left for custom errors in cases where you need a 1 or 0 exit status.
Error.prototype.code = 2;
Error.prototype.reverse = 0;
SyntaxError.prototype.code = 3;
TypeError.prototype.code = 4;
RangeError.prototype.code = 5;
ReferenceError.prototype.code = 6;
EvalError.prototype.code = 7;
URIError.prototype.code = 8;

// modify the error info detailing the fault of this error..
// optionally "arrest" a number of stack frames
Error.prototype.fromCaller = function(arrest) {

	arrest = parseInt(arrest);
	if (isNaN(arrest)) arrest = 0;
	if (arrest < 0) arrest = -arrest; // accept negatives

	var stack = this.stack.split('\n');
	if (stack.length == 1) return this; // just pretend this didn't happen...
	var data = stack[this.reverse+1].split('@');
	var source = data[1].split(':');
	this.sourceURL = source[0], this.line = source[1];

	if (!Boolean(arrest)) {
		data = stack[this.reverse].split('@');
		data[0] += ' caller '+this.name
		stack[this.reverse++] = data.join('@')
	} else {
		while(arrest-- && stack.length > 1) {
			stack.shift();
			//if (Boolean(this.reverse)) this.reverse--;
		}
	}

	this.stack = stack.join('\n');
	return this;

}

sys.error = {
	get number(){return sys_error_number()},
	set number(v){return sys_error_number(v)},
	get message(){return sys_error_message()},
	clear: function clear(){return ! sys_error_number(0)},
	trace: function stack(e) {
		var p = e || new Error(); var stack = p.stack.split('\n').reverse()
		stack.toString = function(){
			return 'Stack Trace: { '+stack.join(' } --> { ').split('@').join(': ')+' }';
		}; return stack;
	},
	toString: function toString(){return sys_error_message(sys_error_number())},
	valueOf: function valueOf(){return sys_error_number()},
};

