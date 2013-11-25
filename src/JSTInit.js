
(function JSTInit() {

var js = {};

js.trace = function(e) {
	var p = e || new Error();
	var stack = p.stack.split('\n').reverse()
	return 'stack trace: { '+stack.join(' } --> { ').split('@').join(': ')+' }';
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
if (o == undefined) throw new ReferenceError("cannot extend undefined")
for(prop in p) {
var property = p[prop];
if (typeof property == 'function') Object.defineProperty(o, prop, {value:property,enumerable:false,configurable:true,writeable:true});
else o[prop] = property;
}
return o;
}

js.extendHidden = function extendHidden(o, p) {
if (o == undefined) throw new ReferenceError("cannot extend undefined")
for(prop in p) {
	Object.defineProperty(o, prop, {value:p[prop],enumerable:false,configurable:true,writeable:true});
}
return o;
}

js.extendPrototype(Number.prototype, {
	toHex: function toHex() { return '0x' + this.toString(16); }
})

return js;
})();

