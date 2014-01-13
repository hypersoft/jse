/*

Hypersoft Systems JST AND Hypersoft System JSE Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list 
   of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this 
   list of conditions and the following disclaimer in the documentation and/or other 
   materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGE.

*/


// fatal return codes, modify these on the newly constructed
// object for custom exit error codes.
// 1 is left for custom errors in cases where you need a 1 or 0 exit status.
Error.prototype.reverse = 0;

Error.prototype.code = 2;
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
		}
	}

	this.stack = stack.join('\n');
	return this;

}

// This must be defined here in this file, as if something goes wrong, the error
// reporter won't operate as intended without this definition.

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

