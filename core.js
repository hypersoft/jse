#!bin/jse

new JSNative.Class(
	"foo",
	{ /* class instance methods (constructor prototype) */
		classInitialize:function(){echo('instance initialize')},
		classInvoke:function(){echo('instance invoked')},
	},
	{ /* class methods (constructor methods) */
		classConstruct:function(){echo('constructor called')},
		classInvoke:function(){echo('class invoked')},
	}
)

b = new foo();


