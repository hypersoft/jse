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

var o = {
	name: "SuperObject",
	classFlags: classFlags('classInvoke', 'classConstruct', 'classGet'),
}

b = JSNative.api.createClass(o);

echo(b.name)
b.prototype = {
	wow:true
}

b.classInvoke =  function() {echo('invoked')};
b.classConstruct =  function() {echo('constructed')};
b.classGet = function(name){echo('name requested:', name); return null};

a = function(){}
a.prototype = new b();
c = new a();

echo(c.wow)
