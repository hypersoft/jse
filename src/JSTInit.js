
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

js.Tokenizer = function(getToken) {
	if (getToken) this.callback = getToken,
	this.syntax = new Array();
}

js.Tokenizer.Expression = function(operationClass, elementName, selector) {
	this.expression = operationClass;
	if (elementName) this.name = elementName; // optional, if not present, "elementName == scanned data"
	if (selector) {
		// numbers are a string slice
		if (typeof selector == 'number') this.readAhead = selector
		// strings are a single case indexOf search
		else if (typeof selector == 'string') this.seekAhead = selector
		// regexps are a single char by expression match.
		else if (js.classOf(selector) == 'RegExp') this.scanAhead = selector;
	}
}

js.Tokenizer.prototype = {
	constructor: js.Tokenizer,
	callback:function(){
		this.element = this.getChar(); this.scanned = this.element
	}, scanned:'', element:'', index:0, token:'', source:'',
	syntax:[], readAhead: 1, failed:"Failed to tokenize stream",
	getChar:function getChar() {return this.source[this.index++]},
	unGetChar:function unGetChar() {this.index--},
	advance:function advance(len) {this.index += len},
	rewind:function rewind(len) {this.index += len},
	readString:function peek(len) {return this.source.slice(this.index, (len == 0)?undefined:this.index + len)},
	wordPeek:function wordPeek(count) {
		var sentence = this.source.slice(this.index);
		var wpk = new RegExp('^(\\s*\\w+\\s*){'+count+'}')
		if ((words = wpk.exec(sentence)) != null) return words[0];
	},
	reset:function(source){
		this.scanned = '', this.element = '', this.index = 0, this.token = '',
		this.readAhead = 1, this.source = (source)?source:'';
		delete this.subExpression;
	},
	load:function(source){this.reset(source)},
	recognize:function(regularExpression, name, readAhead){
		this.syntax.push(new js.Tokenizer.Expression(regularExpression, name, readAhead))
	},
	knownToken:function(){
		while (this.source[this.index] && this.source[this.index].match(/\s/) != null) this.index++;
		if (this.source[this.index] === undefined) { this.getToken(); return }
		var source = this.readString(this.readAhead);
		var syntax = this.syntax;
		for (rule in syntax) {
			var name = syntax[rule].name;
			var expression = syntax[rule].expression, match, data;
			var readAhead = syntax[rule].readAhead;
			if (readAhead != undefined) {
				data = this.readString(readAhead);
			} else {
				var seekAhead = syntax[rule].seekAhead;
				if (seekAhead != undefined) {
					var sought = this.source.indexOf(seekAhead, this.index);
					if (sought == -1) continue; // you missed...
					if (sought == this.index) continue; // this ain't no 'token'
					data = this.source.slice(this.index, sought);
				} else {
					var scanAhead = syntax[rule].scanAhead;
					if (scanAhead != undefined) {
						var x, txt;
						for (x = this.index; x <= this.source.length; x++) {
							txt = this.source[x];
							if (txt === undefined) txt = '';
							if (txt.match(scanAhead)) {
								if (x == this.index) break;
								data = this.source.slice(this.index, x);
								break;
							}
						}
					}
				}
			}
			delete this.subExpression;
			if (js.classOf(expression) == 'RegExp') { // when regexp we match data
				if (!data) data = source;
				match = expression.exec(data)
				if (match != null) {
					this.advance((this.scanned = match.shift()).length);
					if (match.length) this.subExpression = match;
					this.element = (name)?name:this.scanned;
					return 1;
				}
			} else if (js.classOf(expression) == 'Object') { // when object we match keys
				if (!data) data = source; var key
				for (key in expression) if (key == data) {
					this.scanned = key; this.advance(this.scanned.length);
					this.element = (name)?name:this.scanned;
					return 1;				
				}
			} else if (js.classOf(expression) == 'Array') { // when array we match index data
				if (!data) data = source; var index
				for (index in expression) if (expression[index] == data) {
					this.scanned = expression[index]; this.advance(this.scanned.length);
					this.element = (name)?name:this.scanned;
					return 1;				
				}
			} // might want to error out here..
		}
		return 0;
	},
	getToken:function() {
		if (this.index >= this.source.length) {
			delete this.subExpression; this.scanned = null, this.element = null; return;
			// leave this.token and this.expression alone, they were set by this.accept
		}
		if (this.syntax.length && this.knownToken()) return;
		this.callback()
	},
	accept:function accept(elementName) {
		if (this.element == elementName) {
			this.token = (this.scanned)?this.scanned:this.element;
			this.expression = this.subExpression;
			this.getToken(); return 1;
		}
		return 0;
	},
	expect:function expect(elementName) {
		if (this.accept(elementName) == 1) return this.token;
		throw new SyntaxError(this.failed+" at column "+this.index+": expected `"+elementName+"', found `"+this.element+"'");
	},
}

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

return js;
})();

