JSNative.api.typeVoid = 2,
JSNative.api.typeBoolean = 4,
JSNative.api.typeChar = 8,
JSNative.api.typeShort = 16,
JSNative.api.typeInt = 32,
JSNative.api.typeLong = 64,
JSNative.api.typeLongLong = 128,
JSNative.api.typeFloat = 256,
JSNative.api.typeDouble = 512,
JSNative.api.typePointer = 1024,
JSNative.api.typeArray = 2048,
JSNative.api.typeUnsigned = 4096,
JSNative.api.typeEllipsis = 8192;

/* JS NATIVE TYPE */ (function JSNativeTypeClass() {

function getUnsignedType() {
	var unsigned;
	if ((unsigned = JSNative.Type[this.name+' unsigned'])) return unsigned;
	if ((unsigned = JSNative.Type['unsigned '+this.name])) return unsigned;
	return JSNative.Type[this.type | JSNative.api.typeUnsigned]
}

function getSignedType() {
	var signed;
	if ((signed = JSNative.Type[this.name+' signed'])) return signed;
	if ((signed = JSNative.Type['signed '+this.name])) return signed;
	return JSNative.Type[this & ~JSNative.api.typeUnsigned]
}

function getArrayType() {
	var array;
	if ((array = JSNative.Type[this.name+'[]'])) return array;
	if ((array = JSNative.Type[this.name+' **'])) return array;
	return JSNative.Type[this | JSNative.api.typeArray]
}

function getTypeNames() {
	var names = [];
	for (name in JSNative.Type) if (isNaN(name) && JSNative.Type[name].type == this.type) names.push(name)
	return names;
}

function getPointerType() {
	var pointer;
	if ((pointer = JSNative.Type[this.name+' *'])) return pointer;
	if ((pointer = JSNative.Type[this.name+'*'])) return pointer;
	return JSNative.Type['void'].pointer;
};

function isIntegerType(code) { return (code & (8|16|32|64|128|1024))}

var classConstruct = function(code, name){

	code = Number(code);

	if ((code & JSNative.api.typeEllipsis) != 0) { // That's an error
		throw new InvokeError("TypeError", "invalid type code "+code+" (ellipsis)");		
	}

	// Validate unsigned flag
	if ((code & (JSNative.api.typeUnsigned)) != 0) {
		if ((code & (JSNative.api.typeVoid)) || code == JSNative.api.typeUnsigned)
		throw new InvokeError("TypeError", "invalid type code "+code+" (unsigned void)");		
		if ((code & (JSNative.api.typeBoolean)))
		throw new InvokeError("TypeError", "invalid type code "+code+" (unsigned boolean)");		
		if ((code & (JSNative.api.typeFloat)))
		throw new InvokeError("TypeError", "invalid type code "+code+" (unsigned float)");		
		if ((code & (JSNative.api.typeDouble)))
		throw new InvokeError("TypeError", "invalid type code "+code+" (unsigned double)");		
	}

	this.type = code, this.name = name;

	// type size is a constant defined on the 'type definition' for optimal performance
	Object.defineProperty(this, 'size', {value: JSNative.api.getTypeSize(this), enumerable:true})

	// The first definition of a code is its "native representation"
	// don't overwrite codes that have been written.
	if (code in this.constructor); else this.constructor[code] = this;

	// The name of a type is its alias
	// don't overwrite an alias that has been written
	if (name in this.constructor) {
		throw new InvokeError("TypeError", "type name '"+name+"' is already registered");
	} else {
		this.constructor[name] = this;
	}

	if ((code & (JSNative.api.typePointer | JSNative.api.typeArray) ) == 0) {
		var ptrType = new JSNative.Type(code | JSNative.api.typePointer, name+' *');
		var arrType = new JSNative.Type(code | JSNative.api.typeArray, name+'[]');
		var ptrArrayType = new JSNative.Type(code | JSNative.api.typePointer | JSNative.api.typeArray, name+' *[]')
		var ptrPointerType = new JSNative.Type(code | JSNative.api.typePointer | JSNative.api.typeArray, name+' **')
		if (code & (JSNative.api.typeShort | JSNative.api.typeLong | JSNative.api.typeLongLong)) {
			JSNative.Type["int "+name] = this;
			JSNative.Type[name+" int"] = this;
			JSNative.Type["int "+name+' *'] = ptrType;
			JSNative.Type[name+" int *"] = ptrType;
			JSNative.Type["int "+name+'[]'] = arrType;
			JSNative.Type[name+" int[]"] = arrType;
			JSNative.Type["int "+name+' *[]'] = ptrArrayType;
			JSNative.Type[name+" int *[]"] = ptrArrayType;
			JSNative.Type[name+" int **"] = ptrPointerType;
		} else {
			JSNative.Type[name+" **"] = ptrPointerType;
		}
	}

	// is it an integer type?
	Object.defineProperty(this, "integer", {value: isIntegerType(code), enumerable:true})

	if (this.integer) {
		if (name.match(/(un)?signed|\*$|\[\]$/));
		else {
			var unsignedName = "unsigned "+name
			if (unsignedName in JSNative.Type); else new JSNative.Type(code | JSNative.api.typeUnsigned, unsignedName);

			unsignedName = name+" unsigned"
			if (unsignedName in JSNative.Type); else new JSNative.Type(code | JSNative.api.typeUnsigned, unsignedName);
		}
		if (name.match(/(un)?signed|\*$|\[\]$/));
		else {
			var signedName = "signed "+name
			if (signedName in JSNative.Type); else new JSNative.Type(code, signedName);

			signedName = name+" signed"
			if (signedName in JSNative.Type); else new JSNative.Type(code, signedName);
		}
		Object.defineProperty(this, 'unsigned', {get:getUnsignedType, enumerable:true})
		Object.defineProperty(this, 'signed', {get:getSignedType, enumerable:true})
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
		isUnsigned:function(typeId){return (this[typeId] & JSNative.api.typeUnsigned)}
	}
)

Object.defineProperties(JSNative.Type.prototype, {
	pointer:{get:getPointerType, enumerable:true},
	array:{get:getArrayType, enumerable:true},
	names:{get:getTypeNames, enumerable:true}
})

})();

// Initialize Native Types
new JSNative.Type(JSNative.api.typeVoid,		"void")
new JSNative.Type(JSNative.api.typeBoolean,		"bool")
new JSNative.Type(JSNative.api.typeChar,		"char")
new JSNative.Type(JSNative.api.typeShort,		"short")
new JSNative.Type(JSNative.api.typeInt,			"int")
new JSNative.Type(JSNative.api.typeLong,		"long")
new JSNative.Type(JSNative.api.typeLongLong,	"long long")
new JSNative.Type(JSNative.api.typeFloat,		"float")
new JSNative.Type(JSNative.api.typeDouble,		"double")

