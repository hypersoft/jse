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
		if (arguments[name] == 'classAccessor') { flags |= JSNative.api.classAccessor; continue }
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

// List is a native class interface which flags classConstruct
var List = JSNative.api.createClass({name: className, classFlags: classFlags('classConstruct')});

// This creates the namespace by className (demonstration)
declareNameSpace(className, List);

// This classInstance constructor (List) has a native class interface property [classInstance].
List.classInstance = JSNative.api.createClass({
	name: className,
	classFlags: classFlags('classAccessor', 'classConvert', 'classInitialize', 'classEnumerate') 
})

// This is called when an instance of List is initialized
List.classInstance.classInitialize = function() {
	echo("instance initialized");
	Object.defineProperty(this, "__private__list__item__", {value:{}});
}

// This is called when an instance of List is enumerated
List.classInstance.classEnumerate = function() {
	echo("instance enumerate");
	var names = [];
	for (name in this.__private__list__item__) names.push(name);
	return names;
}

// This is called when an instance of List is queried for a property name
List.classInstance.classGet = function(name) {
	if (name == '__private__list__item__') return null;
//	echo("instance get", name);
	return this.__private__list__item__[name];
}

// This is called when an instance of List recieves a request to set a property name to a value
List.classInstance.classSet = function(name, value) {
	echo("instance set", name);
	this.__private__list__item__[name] = value;
	return true;
}

// This is called when an instance of List is queried for a type conversion; usually JS constructor
// however, this 'native class instance method' can be invoked on the class instance,
// with any caller defined constructor function. This enables class to class conversion,
// for class instances who possess the intelligence to perform the requested conversion.
List.classInstance.classConvert = function(constructor) {

	// As with all classInstance interface procedures, the value of 'this' is the target object

	if (constructor === String) {
		echo("instance string conversion requested");
		var names = [];
		for (name in this.__private__list__item__) names.push(name);
		return names.join(', ');
	}
	if (constructor === Number) {
		echo("instance number conversion requested");
		// return valueOf value
	}

	// This instance supports no custom constructors, a test and warning or error upon 
	// this condition would be implementation defined.

	// If constructor is not String constructor (could be valueOf) just use the default
	// JS Interpretation
	return JSNative.api.failedToConvert;
}

// This classInstance constructor (List) has a native class interface property [classInstance].
// 'new' shall provide this interface, through the constructed object's prototype,
// as a member of the constructor property (a known classInstance provider.)
List.prototype = Object.defineProperties({}, {constructor:{value:List}});

// List is a native class interface which flags classConstruct, fulfill the class contract
List.classConstruct = function() {
	// the value of 'this' is the new object, with it's prototype set to List.prototype
	echo("constructor invoked");
	for (var i = 0; i<arguments.length; i++) {
		this[arguments[i]] = arguments[i];
	}
}

})("JSNative.List");

a = new JSNative.List('bear', 'pear', 'wear', 'tear', 'toString');

// The result of this process, is an object with special behaviors that are acted upon,
// independent of the object content, so long as the object's prototype points to a valid
// "special behavior" interface, and that interface has the requested property interface defined.


for (name in a) echo(name);

echo(a);

