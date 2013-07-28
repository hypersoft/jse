#!bin/jse

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

List.prototype = Object.defineProperties({}, {
	constructor:{value:List},
});

List.classConstruct = function() {
	echo("constructor called");
}

})("JSNative.List");


a = new JSNative.List();

a();

echo(a);
