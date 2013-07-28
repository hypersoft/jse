#!bin/jse

// Some utility functions ..
function classFlags() {
	var name, flags;
	for (name in arguments) {
		if (arguments[name] == 'classInitialize') { flags |= JSNative.api.classInitialize; continue }
		if (arguments[name] == 'classConstruct') { flags |= JSNative.api.classConstruct; continue }
		if (arguments[name] == 'classInvoke') { flags |= JSNative.api.classInvoke; continue }
		if (arguments[name] == 'classGet') { flags |= JSNative.api.classGet; continue }
		if (arguments[name] == 'classSet') { flags |= JSNative.api.classSet; continue }
		if (arguments[name] == 'classDelete') { flags |= JSNative.api.classDelete; continue }
		if (arguments[name] == 'classConvert') { flags |= JSNative.api.classConvert; continue }
		if (arguments[name] == 'classEnumerate') { flags |= JSNative.api.classEnumerate; continue }
		if (arguments[name] == 'classInstanceOf') { flags |= JSNative.api.classInstanceOf; continue }
	};
	return flags;
}

function declareNameSpace(name, value) {
	$declareNameSpace = value;
	var nameSpace = eval(name+' = $declareNameSpace');
	delete $declareNameSpace;
	return nameSpace;
}

// Example "Class"
(function ListInit(className) {

var List = JSNative.api.createClass({name: className, classFlags: classFlags('classConstruct')});

declareNameSpace(className, List);

List.classInstance = JSNative.api.createClass({
	name: className,
	classFlags: classFlags('classGet', 'classInvoke', 'classConvert') 
})

List.classInstance.classGet = function(name) {
	echo("instance get", name); return null;
}

List.classInstance.classInvoke = function() {
	echo("instance invoked");
}

List.classInstance.classConvert = function(constructor) { // default toString and valueOf handler
	// This procedure can be passed ANY constructor as a reference to the desired type
	if (constructor === String) {
		// return toString value
	}
	if (constructor === Number) {
		// return valueOf value
	}
	return JSNative.api.failedToConvert;
}

// The prototype constructor (List) has the classInstance interface
List.prototype = Object.defineProperties({}, {constructor:{value:List}});

List.classConstruct = function() {
	echo("constructor invoked");
}

})("JSNative.List");

a = new JSNative.List();

// The result of this process, is an object with special behaviors that are acted upon,
// independent of the object content, so long as the object's prototype points to a valid
// "special behavior" interface, and that interface has the requested property interface defined.

a();

echo(a);
