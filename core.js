#!bin/jse

JSNative.Enumeration = new JSNative.Class("JSNative.Enumeration",
	function invoke(section, query) {
		var o = JSNative.Enumeration.register[section];
		if (o === undefined || query === undefined) return o;
		return o[query];
	},
	function construct(name, kvp) {
		for (value in kvp) {
			if (value == 'length' || value == 'name' || value == 'value') continue;
			this.push(value, kvp[value]);
		}
		Object.defineProperty(JSNative.Enumeration.register, name, { value: this, enumerable: true});
	},
	Object.defineProperties(
		{ // Prototype
			autoEnum: 0, length: 0
		},
		{ // Methods
			push: { value: function(name, value) {
				if (value === undefined) value = this.valueAccumulator();
				var o = new JSNative.api.createClass("JSNative.Enumeration",
				Object.defineProperties(
					{ name: undefined, value: undefined }, // prototype
					{ // prototype methods
						"valueOf": {value: JSNative.Enumeration.valueOfUnit },
						"toString": {value: JSNative.Enumeration.valueOfUnit }
					}
				));
				Object.defineProperties(o, {value: { value: value }, name: {value: name}});
				Object.defineProperty(this, name, { value: o, enumerable:true });
				Object.defineProperty(this, value, { value: o.name, enumerable:true });
				this.length++;
			} },
			valueAccumulator: {value: function valueAccumulator() { return (this.autoEnum++ << 1); } },
			toString: { value: function(name) { if (this.length == 0) return undefined;
					var s = [];
					for (name in this) if (isNaN(name) && name != "autoEnum" && name != 'length' && name != 'name' && name != 'value') s.push(name+':'+this[name]); return '{\n'+s.join(', ')+'\n}';
			} },
		}
	),
	{
		register: {value: {}, enumerable:true},
		valueOfUnit: {value: function valueOfUnit() { return this.value; } },
	}
);

c = new JSNative.Enumeration("names", [ 'andy', 'lester', 'tiffany' ]);


echo(JSNative.Enumeration('names')[2]);

exit(0);
//JSNative.Class = function(name, functionObject) {};

//JSNative.Class.prototype = {
//	getName: undefined,
//	setName: undefined,
//	delete: undefined,
//	invoke: undefined,
//	construct: undefined,
//	instanceOf: undefined,
//	convertTo: undefined,
//}


//JSNative.api.constructClass = function(classAddress, classPrivateObject) {};
//JSNative.api.getClassPrivateObject = function(classObject) {}; // returns instance copy

//JSNative.api.constructAddress = function(number) {};
//JSNative.api.allocateAddress = function(size) {};
//JSNative.api.reallocateAddress = function(number, size) {};

///* CDECL
//char * NAME[16];
//*/ var NAME = new JSNative("char * array[16]");

///* CDECL
//char * NAME[] = &someOtherValue;
//*/ var NAME = new JSNative("char * array[]").value = JSNative.addressOf(someOtherObject);


//// My set property has to validate my type

JSNative.Value.addressOf = function() {

}

JSNative.Value.get = function() {

}

JSNative.Value.set = function() {

}

function Type() {
	var jsType = typeof argument[0];
	if (jsType  == 'number' || jsType == 'string') {
		return Type[argument[0]];
	}
}

Type.Model = function (name, code, pointer, array, parser) {
	this.code = code, this.name = name,	this.pointer = pointer,	this.array = array;
	this.signed = new Boolean((this.code % 2) || this.code == 70 || this.code == 80);
	this.parse = parser.bind(this);
	Type[code] = Type[name] = Object.freeze(this);
}

Type.sizeOf = function(type) { return Type.sizeOf[type.code]; }
Type.sizeOf.allocationSize = function(type) { return Type.sizeOf.allocationSize[type.code]; }

Type.Model.prototype = {
	isEqualTo: function(that) {
		return (this.code == that.code && this.pointer == that.pointer && this.array == that.array)
	},
	get size() { return Type.sizeOf[this.code]; },
	get allocationSize() { return Type.sizeOf[this.code].allocationSize; },
	parse: function () { return this.valueFrom(arguments) },
	valueOf: function () { return this.code },
	toString: function() { var build = (this.pointer == true)?"*":''+(this.array == true)?"[]":'';
		return this.name + (build != '')?" "+build:build
	},
}

new Type.Model("void", 0, 0, 0, function valueFrom() {
	throw new Error("cannot convert argument"+((arguments.length > 1) ? "s":'')+" to void type");
	return null;
});

Type.parseDeclaration = function(d) {
	this.sourceOf = d.replace(/(\*)/g, ' *').replace(/\[/g, ' [').replace(/\n|\s+/g, ' ').replace(/\]\s/g, ']').trim();
	var stack = this.sourceOf.split(' ');
	function storage() {
		if (stack[0] == "const") this.typeConstant = Boolean(stack.shift());
		typeName.apply(this);
	}
	function typeName() {
		if (stack[0] == "unsigned")	{
			if (this.typeUnsigned == true) {
				throw new SyntaxError("");
				return;
			}
			this.typeUnsigned = true; stack.shift();
			typeName.apply(this); return;
		}
		if ((this.typeOf = stack.shift()) == "int") this.typeInteger = true;
		if (this.typeOf in Type.parseDeclaration.reservedWords) {
			throw new TypeError("Type.parseDeclaration: typeName: " +this.typeOf+" is a reserved word")
		}
		if (this.typeOf.match(/^[_a-zA-Z]+[_a-zA-Z0-9]*$/) == null) {
			throw new TypeError("Type.parseDeclaration: unable to parse type name: " + this.typeOf);
			return
		}
		if (stack[0] == "int")	{
			if (this.typeInteger == true) {
				throw new SyntaxError("");
				return;
			}
			this.typeInteger = true; stack.shift();
		}
		indirection.apply(this);
	}
	function indirection() {
		if (stack[0] == "*") {	stack.shift();
			this.addressOf = true;
		}
		array.apply(this);
	}
	function array() {
		if ( ! stack.length > 0) return;
		if (stack[0] == "*") {	stack.shift();
			this.arrayOf = true;
		}
		if ( ! stack.length > 0) return;
		if (stack[0].indexOf("[") == 0)	{
			this.arrayOf = stack.shift();
		}
	}
	storage.apply(this);
}

Type.parseDeclaration.reservedWords = {
	"const": true, "static": true, "volatile": true, "class": true, "type": true,
	"instanceof": true, "typeof": true
};

var p = new Type.parseDeclaration("void *[]");

for (name in p) echo(name+":", p[name]);

