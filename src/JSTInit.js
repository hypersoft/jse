
(function JSTInit() {

var jsErrorToString = function() {
	return this.sourceURL+': '+this.function+': line '+this.line+': '+this.name+': '+this.message;
};

var jsCallStackToString	= function toString() {
	return this.file+': '+this.function+': line '+this.line
}

var jsCallStackParse = function parse(c) {
	var o = {};
	var parsed = c.match(/(([^@]+)@)?([^:]+):(.+)$/);
	if (parsed !== null) {
	o.line = parsed.pop();
	o.file = parsed.pop();
	(o.function = parsed.pop())?undefined:(o.function = 'anonymous function');
	} else {
		o.file = c;
	}
	o.toString = jsCallStackToString;
	return o;
}

var js = Object.defineProperties(new Object(), {
	callStack: {get: function() {
		var trace = [];
		try { throw Error('n/a') } catch(err) {
			var info = err.stack.split("\n");
			info.shift(); // remove 'this call'
			for (index in info) trace.unshift(jsCallStackParse(info[index])); // add in reverse order (logical order)
		}
		return trace;
	}, enumerable:true},
	error: {value:function error(title, message) {
		var stack = callStack();
		stack.pop() // get rid of this call
		stack.pop() // top is now at source
		while((invoke = stack.pop()).file == '[native code]');
		var e = new Error(message);
		if (invoke.function != 'global code') e.function = invoke.function;
		e.sourceURL=invoke.file; e.line=invoke.line; e.name=title;
		e.toString = jsErrorToString;
		return e;
	}, enumerable:true},
	run: {value:Object.defineProperty(new Object(), "date", {
		value:new Date(), enumerable:true
	}), enumerable:true},
});
return js;
})();

