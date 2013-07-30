#!bin/jse

(function JSNativeTypeClass() {

function getUnsignedType() {return JSNative.Type['unsigned '+this.name]};
function getPointerType() {return JSNative.Type[this.name+' *']};

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

	// is it an integer type?
	Object.defineProperty(this, "integer", {value: (code & (8|16|32|64|128|1024)), enumerable:true})

	if (this.integer) {
		Object.defineProperty(this, 'unsigned', {get:getUnsignedType, enumerable:true})
		if ((code & JSNative.api.typeUnsigned) == 0 && ! this.unsigned)
		new JSNative.Type(code | JSNative.api.typeUnsigned, "unsigned "+name);
	}

	if (! this.pointer ) {
		if ((code & JSNative.api.typePointer) == 0) 
		new JSNative.Type(code | JSNative.api.typePointer, name+' *');
	}

}

var classInvoke = function(code) {return this.Type[code]}

new JSNative.Class
(
	"JSNative.Type",
	{ /* class instance methods (constructor prototype) */
		toString: function() {return this.name},
		valueOf: function() {return this.type},
	},
	{ /* class methods (constructor methods) */
		classConstruct:classConstruct,
		classInvoke:classInvoke,
		sizeOf: function(typeId) {return this[typeId].size},
		isInteger:function(typeId){return this[typeId].integer},
		isUnsigned:function(typeId){return (this[typeId] & (8|16|32|64|128|1024))}
	}
)

Object.defineProperties(JSNative.Type.prototype, {
	pointer:{get:getPointerType, enumerable:true},
})

})();

char = new JSNative.Type(8, "char");

echo(char.pointer)

