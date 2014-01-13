#!bin/jse

Error.debug = "on"

var echo = new sys.command("echo");

function SystemClass(name, construct, exec, prototype) {

var f = function() {
	if (this instanceof arguments.callee) {
		return Object.setPrototypeOf(arguments.callee.construct.apply(null, arguments), arguments.callee.prototype);
	} else {
		return Object.setPrototypeOf(argument.callee.exec.apply(null, arguments), arguments.callee.prototype);
	}
}
	var enterface = Object.defineProperties(f, {
		construct:{value:construct, enumerable:false, writeable:false, configurable:false},
		exec:{value:exec, enumerable:false, writeable:false, configurable:false},
		prototype:{value:prototype,enumerable:false,configurable:false}
	}).bind(this);
	enterface.name = name;
	return enterface;
}

sys.env = SystemClass("env", sys_env_constructor, sys_env_function, {});

echo(sys.env.name)

