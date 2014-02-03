/*

Hypersoft Systems JST AND Hypersoft Systems JSE
Copyright (c) 2014, Triston J. Taylor

All rights reserved.

*/

/* Flags ==================================================================== */

function Flags(names) {
	if (!Array.prototype.isPrototypeOf(names)) throw Exception(
		TypeError, "expected array of names"
	);
	if (names.length >= 31) throw Exception(RangeError, "Flags exceed 32-bits");
	for (var index in names) {
		var name = names[index];
		if (name in this) throw Exception(ReferenceError,
			"Duplicate names in flag specification: "+name+
			" is already defined at index "+index
		);
		this[name] = Flags.map[index];
	}
}

Flags.prototype = Properties({}, [
	['constructor', Flags],	
	[function create(alias, data, enumerable){
		Property(this, alias, Number(data), enumerable, false);
		return this[alias];
	}],

	[function listNamesOf(integer){
		var list = [];
		for (var name in this) if (integer & this[name]) list.push(name);
		return list;
	}],
	[function indexOf(name){
		if (name in this) return Flags.map.indexOf(this[name]);
		return 0;
	}],
	[function flagsOf(){
		var flag = 0;
		for (var index in arguments) {
			if (arguments[index] in this) flag |= Number(this[arguments[index]]);
		}
		return flag;
	}]
],  false);

Flags.map = [];
for (var i = 0; i < 31; i++) Flags.map[i] = (1 << i);
Object.freeze(Flags.map);

/* now Access yourself */
Flags;

/* ========================================================================== */
