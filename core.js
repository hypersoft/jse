#!bin/jse

(function() {

var classConstruct = function(code, name){
	this.type = code;
	this.name = name;
	this.constructor[code] = this;
	this.constructor[name] = this;
}

var classInvoke = function(code) {
	return this.Type[code];
}

new JSNative.Class
(
	"JSNative.Type",
	{ /* class instance methods (constructor prototype) */
	},
	{ /* class methods (constructor methods) */
		classConstruct:classConstruct,
		classInvoke:classInvoke,
	}
)

Object.defineProperties(JSNative.Type.prototype, {
	size:{
		get:function(){return JSNative.api.getTypeSize(this.type)},
		enumerable:true
	},
	toString:{
		value: function() {return this.name}
	},
	valueOf:{
		value: function() {return this.type}
	}
})

})();

char = new JSNative.Type(8, "char");

echo(JSNative.Type("char"))


