#!bin/jse

// define struct interface
sys.memory.interface = function(name, proto, aligned) {
	var lastOffset = 0;
	Object.defineProperties(this, {
		__name__:{
			value: name,
			enumerable:false, configurable:false, writeable:false
		},
		__width__:{
			value: sys.memory.interface.align(lastOffset),
			enumerable:false, configurable:false, writeable:false
		},
		__value__:{
			value: sys.type.struct,
			enumerable:false, configurable:false, writeable:false
		},
	});
	for (name in proto) {
		this[name] = proto[name]
		this[name].offset = (aligned === false) ?
			lastOffset : sys.memory.interface.align(lastOffset, proto[name]);
		lastOffset += proto[name].width;
	}
}

sys.memory.interface.prototype = coreType.prototype;

// struct interface member offset calculator
sys.memory.interface.align = function(address, type) {
	var width = (Object.isSystemType(type)) ? sys.type.sizeOf(type) : sys.memory.alignment;
	var unaligned = (address % width)
	if (unaligned != 0) {
		return Number(address + (width - unaligned));
	} else return address;
}

// struct/type width accessor
sys.type.sizeOf = function(o) {
	if (o && o.__width__) return o.__width__;
	return o.width || sys.type[o].width;
}


var iface = new sys.memory.interface(
	'simple', {a: sys.type.char, b: sys.type.double}
);


