
JSNative.List = function List() {
	if (arguments.length == 1) {
		if (classOf(arguments[0]) == Array.name) {
			var array = arguments[0];
			for (index in array) this[array[index]] = null;
		} else if (classOf(arguments[0]) == Function.name) {
			this[arguments[0].name] = null;
		} else if (classOf(arguments[0]) == Object.name) {
			var object = arguments[0];
			for (name in object) this[name] = null;
		} else {
			this[String(arguments[0])] = null;
		}
	} else for (name in arguments) JSNative.List.call(this, arguments[name]);
}
JSNative.List.prototype.add = function add(list, name) {
	if (this === JSNative.List) return;
	list[name] = null;
}
JSNative.List.prototype.remove = function remove(list) {
	if (this === JSNative.List) return;
	var argv = []; argv.push.apply(argv, arguments); argv.shift();
	for (name in argv) delete list[argv[name]];
}
JSNative.List.prototype.valueOf = function(list) {
	if (this === JSNative.List) return null;
	return null;
}
JSNative.List.prototype.toString = function(list) {
	if (this === JSNative.List) return null;
	var o = []; for (name in list) o.push(name); return o.join(delim)
}
JSNative.List.prototype.filter = function(filterFunction, filterData) {
	var list = Object.create(this);
	for (name in list) filterFunction.call(list, name, filterData)
	return list;
}
JSNative.List.prototype.combine = function(names, values) {
	var keys = [], vals = [];
	for (name in names) keys.push(name);
	for (name in values) vals.push(name);
	var result = {};
	for (var i = 0, overflow = keys.length, underflow = vals.length; i<overflow && i<underflow; i++) result[keys[i]] = vals[i];
	return result;
}

JSNative.List.prototype.exports = new JSNative.List('combine','toString','valueOf','remove','add');

//delete api;


