#!bin/jse

Error.debug = "on";

var echo = new sys.command("echo");

var Member = function(type, name) {};
Member.prototype = {
	constructor: Member
};

var Structure = function(alias, members) {};
Structure.prototype = {
	constructor: Structure
};

var Union = function(alias, members) {};
Union.prototype = {
	constructor: Union
};

var Alias = function() {};
Alias.prototype = {
	constructor: Alias
};

var Type = (function SysInitType(){

	/* predefine property strings for efficiency */
	var float = "float", integer = "integer", array = "array", utf = "utf",
	constant = "constant", dynamic = "dynamic", boolean = "boolean",
	value = "value", signed = "signed", unsigned = "unsigned", alias = "alias",
	reference = "reference", structure = "structure", union = "union";

	var arrayTypeDimensions = function(x, y) {
		this[0] = Math.abs(x); this[1] = Math.abs(y);
		this.length = 2;
		Object.freeze(this);
	};

	Property(arrayTypeDimensions, "prototype",
		Property(Object.create(Array.prototype), "toString",
			function(){
				return '[' + this.join(", ") + ']';
			}, false, false, false
		),
		false, false, false
	);

	var typeSetWidth = function(size) {
		var width = Property(Object(size), "bits",
			((size)?(size << 3):0), true, false
		);
		Property(this, "width", width, false, false);
		this.code |= size;
		return size;
	};

	var typeGetter = function() {
		return this;
	};

	var typeSetter = function() {
		throw new TypeError(
			"Attempting to change the value of a global system type"
		).fromCaller(-1);
	};

	var Type = function() { try {
		var type, prototypeMode = false;
		if (this === Type.prototype.pointer) {
			type = Object.create(Type.prototype);
			prototypeMode = true;
		} else type = this;
		if (type instanceof Type) { // new
			Property(type, "code", 0, false, true);
			type.name = Type.parseIdentifier(arguments[0]);
			if (typeof arguments[1] === 'object') {
				var object = arguments[1];
				for (var name in object) {
					type[name] = Type.parseProperty.call(type, object, name);
				};
				Object.freeze(type);
				if (! prototypeMode) {
					Accessor(
						sys.global, arguments[0],
						typeGetter.bind(type), typeSetter,
						true, false
					);
					if (alias in type) {
						Accessor(
							sys.global, type.alias,
							typeGetter.bind(type), typeSetter,
							true, false
						);
					}
				}
			} else throw new TypeError("Expected arguments: name, object");
			return type;
		} else if (typeof arguments[0] === 'string') {
			var type = sys.global[arguments[0]];
			if (type instanceof Type) return type;
			else return null;
		} else throw new TypeError("Expected argument: string");
	} catch(e) { throw e.fromCaller(-1); }; };

	Property(Type, "prototype", Properties(Prototype(Type), [
		["pointer", function() {
			return Type.call(
				Type.prototype.pointer, this.name+"Pointer", {reference: this}
			);
		}],
		["valueOf", function() {
				return this.code;
		}]
	], false), false);

	Property(Type, "flags", new Flags(

		[	/* do not modify, binary synchronized with jst.h */
			/* void === 0 */
			1, 2, 4, 8,
			"reserved_16", "reserved_32", "reserved_64", "reserved_128",
			"dynamic", "constant", "integer", "signed", "utf", "reference",
			"array", "structure", "union", "varargs", "value"
		],

		Object.create(null, {
			toString: {
				value:function(){
					return this.name;
				},
				enumerable:false
			},
			valueOf: {
				value:function(){
					return this.value;
				},
				enumerable:false
			}
		})

	), false, false, false);

	/* the following null flags are needed to ease loop coding */
	Type.flags.flag("unsigned", 0);
	Type.flags.flag("float", 0);
	Type.flags.flag("boolean", 0);
	Object.hideProperties(Type.flags, [unsigned, float, boolean]);
	Object.freeze(Type.flags);

	Property(Type, "initializers", Object.freeze({
		float		: Object.freeze([Number, String]),
		array		: Object.freeze([Number, Array]),
		utf			: Object.freeze([Number, String]),
		integer		: Object.freeze([Number, Boolean]),
		signed		: Object.freeze([Number, Boolean]),
		unsigned	: Object.freeze([Number, Boolean]),
		constant	: Object.freeze([Boolean]),
		dynamic		: Object.freeze([Boolean]),
		boolean		: Object.freeze([Boolean]),
		value		: Object.freeze([Boolean]),
		varargs		: Object.freeze([Boolean]),
		alias		: Object.freeze([String]),
		reference	: Object.freeze([Type]),
		structure	: Object.freeze([Structure]),
		union		: Object.freeze([Union])
		}), false, false, false
	);

	Property(Type, "restrictions", Object.freeze({
		float : Object.freeze([
			integer, utf, boolean, value, signed, unsigned, reference,
			structure, union
		]),
		integer : Object.freeze([
			float, utf, boolean, value, reference, structure, union
		]),
		array : Object.freeze([value]),
		utf : Object.freeze([
			integer, boolean, value, float, reference, structure, union
		]),
		constant : Object.freeze([value]),
		dynamic : Object.freeze([value]),
		boolean : Object.freeze([
			integer, utf, float, value, signed, unsigned, reference,
			structure, union
		]),
		value : Object.freeze([
			integer, utf, boolean, float, signed, unsigned, reference,
			structure, union, constant, dynamic
		]),
		varargs : Object.freeze([
			integer, utf, boolean, value, signed, unsigned, reference,
			structure, union, constant, dynamic, array, float
		]),
		signed : Object.freeze([
			float, boolean, value, unsigned, reference,
			structure, union
		]),
		unsigned : Object.freeze([
			float, boolean, value, signed, reference,
			structure, union
		]),
		alias : Object.freeze([]),
		reference : Object.freeze([
			integer, utf, boolean, value, signed, unsigned, float,
			structure, union
		]),
		structure : Object.freeze([
			integer, utf, boolean, value, signed, unsigned, reference,
			float, union
		]),
		union : Object.freeze([
			integer, utf, boolean, value, signed, unsigned, reference,
			structure, float
		])
		}), false, false, false
	);

	Property(Type, "parseIdentifier", function(name){ try {
		var validated = name.match(/^[a-z_]+[a-z0-9_]*?$/gi);
		if (validated === null) throw new TypeError(
			"Invalid type name: "+ name
		);
		if (name in sys.global) throw new ReferenceError(
			"Identifier " + name + " is a global " +
			((sys.global[name] instanceof Type) ? "system type" : "identifier")
		);
		return name;
	} catch(e) { throw e.fromCaller(-1); }; }, false, false, false);

	Property(Type, "parseProperty", function(object, name) { try {

		var specs = Type.initializers[name], constructor = undefined;
		var value = object[name];

		if (specs === undefined || specs.constructor !== Array)
			throw new TypeError(
				"Type Property " + name + " is not a valid type initializer"
			);

		for (var index in specs) {
			if (value.constructor === specs[index]) {
				constructor = specs[index]; break;
			}
		}

		if (constructor === undefined) throw new ReferenceError(
			"Illegal value type for type property " + name
		);

		if (constructor === Boolean && value !== true) {
			throw new ReferenceError(
				"Boolean value of type property " + name + " is not true"
			);
		}

		var restrictions = Type.restrictions[name];
		for (var index in restrictions) {
			var restricted = restrictions[index];
			if (restricted in object) throw new TypeError(
				"Type " + name +
				" cannot be defined with " + restricted + " type property"
			);
		}

		this.code |= Type.flags[name];

		if (name === boolean) {
			typeSetWidth.call(this, sys.memory.boolean);
		} else if (name === signed || name === unsigned) {
			if (constructor === Number){
				if (value !== 1 && value !== 2 && value !== 4 && value !== 8)
					throw new TypeError(
						"Numeric integer value " + name +
						" specificand is not 1, 2, 4, or 8"
					);
				var ucs = (utf in object);
				if (integer in object || ucs) throw new ReferenceError(
					"Numeric integer value " + name +
					" defined for type declaring " +
					((ucs) ? "utf" : "integer")
				);
				this.integer = typeSetWidth.call(this, value), value = true;
			} else if (! (integer in object || utf in object)) {
				this.integer = typeSetWidth.call(this, sys.memory.align);
			}
		}
		else if (name === alias) Type.parseIdentifier(name, Type.cache);
		else if (name === float) {
			if (constructor === Number && value !== 4 && value !== 8) {
				throw new TypeError(
					"Floating point integer specificand is not 4 or 8"
				);
			} else {
				if (value === 'single') value = 4;
				else if (value === 'double') value = 8;
				else throw new TypeError(
					"Floating point string specificand is not single or double"
				);
			}
			typeSetWidth.call(this, value);
		} else if (name === integer) {
			if (! (unsigned in object || signed in object))
				this.signed = true;
			if (constructor === Number) {
				if (value !== 1 && value !== 2 && value !== 4 && value !== 8)
					throw new TypeError(
						"Integer numeric specificand is not 1, 2, 4, or 8"
					);
			} else value = sys.memory.align;
			typeSetWidth.call(this, value);
		} else if (name === array) {
			var length, x, y;
			if (constructor === Number) {
				x = parseInt(value), y = 0;
			} else {
				length = value.length;
				if (length === 0) {
					x = y = 0;
				} else if (length === 1) {
					x = parseInt(value[0]), y = 0;
				} else if (length === 2) {
					x = parseInt(value[0]), y = parseInt(value[1]);
				} else throw new RangeError(
					"Array exceeds multi-dimensional capacity"
				);
			}
			if (isNaN(x)) throw new TypeError(
				"Primary integer array specificand is not a number"
			); else if (isNaN(y)) throw new TypeError(
				"Secondary integer array specifcand is not a number"
			);
			value = new arrayTypeDimensions(x, y);
		} else if (name === utf) {
			if (! (unsigned in object || signed in object))
				this.unsigned = true;
			if (constructor === Number) {
				if (value !== 8 && value !== 16 && value !== 32)
					throw new TypeError(
						"UTF integer specifcand is not 8, 16, or 32 bits"
					);
				this.integer = typeSetWidth.call(this, value);
			} else {
				var ucsx = value.match(/(?:ucs|UCS)-?(1|2|4)/);
				if (ucsx !== null) this.integer = Number(ucsx[1]);
				else {
					var utfx = value.match(/(?:utf|UTF)-?(8|16|32)/);
					if (utfx !== null) this.integer = (utfx[1] >> 3);
					else throw new TypeError(
						"Unknown UTF string specifcand: " + value
					);
				}
				typeSetWidth.call(this, this.integer);
			}
			value = true;
		}

		return value;

	} catch(e) { throw e.fromCaller(-1); }; }, false, false, false);

	return Type;

})();

var Integer = function(value, width, signed) {};

Integer.bits = function(width) {
	return (width * 8);
};

Integer.min = function(width, signed) { /* blatant machine programming */
	if (!signed) return 0;
	var bits = Integer.bits(width) - 1;
	var o = Properties(Object(Math.pow(2, bits) * -1),
		[
			["width", width],
			["signed", signed]
		], false, false
	);
	return o;
};
Integer.max = function(width, signed) {
	var bits = Integer.bits(width);
	if (signed) bits--;
	var o = Properties(Object(Math.pow(2, bits) - 1),
		[
			["width", width],
			["signed", signed]
		], false, false
	);
	return o;
};

var printType = function(type) {
	var output = [];
	for (name in type) output.push(name + ' = ' + type[name] + ';');
	echo(output.join('\n'));
};

new Type("utf8", {utf: "ucs-1", alias: "gchar", array: 4});
new Type("voidt", {});
echo(sys_uint8_max, Integer.max(1));