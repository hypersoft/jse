
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
});

js.classOf = function classOf(o) {
if (o === null) return "Null"; if (o === undefined) return "Undefined";
return Object.prototype.toString.call(o).slice(8,-1);
}

js.extend = function extend(o, p) {
for (prop in p) o[prop] = p[prop];
return o;
}

js.extendPrototype = function extendPrototype(o, p) {
for(prop in p) {
var property = p[prop];
if (typeof property == 'function') Object.defineProperty(o, prop, {value:property,enumerable:false,configurable:true,writeable:true});
else o[prop] = property;
}
return o;
}

js.extendPrototype(Number.prototype, {
	toHex: function toHex() { return '0x' + this.toString(16); }
})

return js;
})();

