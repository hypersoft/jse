#!bin/jse

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
	get allocationSize() { return Type.sizeOf.allocationSize[this.code]; },
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

