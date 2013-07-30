#!bin/jse

(function JSNativeTypeClass() {

var classConstruct = function(code, name){
	this.type = code, this.name = name;
	// type size is a constant defined on the 'type definition' for optimal performance
	Object.defineProperty(this, 'size', {value: JSNative.api.getTypeSize(this), enumerable:true})
	// The first definition of a code is its "native representation"
	// don't overwrite codes that have been written.
	if (! code in this.constructor) this.constructor[code] = this;
	// The name of a type is its alias
	// don't overwrite an alias that has been written
	if (name in this.constructor) {
		throw new InvokeError("TypeError", "type name '"+name+"' is already registered");
	} else {
		this.constructor[name] = this;
	}
}

var classInvoke = function(code) {return this.Type[code]}

new JSNative.Class
(
	"JSNative.Type",
	{ /* class instance methods (constructor prototype) */
		toString: function() {return this.name},
		valueOf: function() {return this.type}
	},
	{ /* class methods (constructor methods) */
		classConstruct:classConstruct,
		classInvoke:classInvoke,
		sizeOf: function(typeId) {return this[typeId].size},
	}
)

})();

char = new JSNative.Type(8, "char");

echo("sizeOf(char) ===", JSNative.Type.sizeOf("char"))


