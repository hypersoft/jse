// Primitive Type Registry //
JSNative.Type = {}; // don't delete;

// The following definitions should be don't delete, read only
JSNative.Type["void"] = 0;
JSNative.Type["bool"] = 10;
JSNative.Type["char"] = 20;
JSNative.Type["short"] = 30;
JSNative.Type["int"] = 40;
JSNative.Type["long"] = 50;
JSNative.Type["long long"] = 60;
JSNative.Type["float"] = 70;
JSNative.Type["double"] = 80;
JSNative.Type["pointer"] = 90;

// Type Alias Registry //
JSNative.Alias = {};

JSNative.typeCodeIsUnsigned = function (typeCode) { return typeCode % 10; }

JSNative.typeCodeIsUnsignable = function (typeCode) { return typeCode < 70 && typeCode > 10; }

JSNative.dereferenceTypeName = function dereferenceTypeName(name) {
	if (name in JSNative.Type) return JSNative.Type[name];
	if (name in JSNative.Alias)	{ var value = JSNative.Alias[name];
		if ( isNaN(parseInt(value)) ) return JSNative.dereferenceTypeName(value);
		return value; 
	}
	throw new Error("JSNative.dereferenceTypeName: " + name + " is not a defined type");
}

JSNative.typeIsUnsignable = function typeIsUnsignable(name) { 
	return JSNative.typeCodeIsUnsignable(JSNative.dereferenceTypeName(name));
}

JSNative.typeCode = function typeCode(prototype) {
	var words = prototype.split(" "); var unsigned = 0;
	if (words[0] == "unsigned") { unsigned = 1; words.shift(); }
	var id = words.join(" "); var typeCode = JSNative.dereferenceTypeName(id);
	if (unsigned == 1) {
		if ( ! JSNative.typeCodeIsUnsigned(typeCode) && JSNative.typeCodeIsUnsignable(typeCode)) {
			return typeCode + 1;
		} else {
			throw new Error("JSNative.typeCode: " + id + " is not an unsignable type");
		}
	} else return typeCode;
}

JSNative.defineAlias = function defineAlias(prototype, id) {
	if ( id.split(" ").length > 1 ) throw new Error("JSNative.defineAlias: " + id + " is not a single word");
	if ( id in JSNative.Alias ) {
		throw new Error("JSNative.defineAlias: " + id + " is already a defined alias");
	}
	JSNative.Alias[id] = JSNative.typeCode(prototype);
}

JSNative.typeCodeToPrototype = function typeCodeToPrototype(val) {

	var unsigned = false;
	var output = "";

	if (val < 120) {
		unsigned = (val % 10);
		if (unsigned == 1) {
			val--; output="unsigned ";
			if (val == 20) output += "char";
			else if (val == 30) output += "short";
			else if (val == 40) output += "int";
			else if (val == 50) output += "long";
			else if (val == 60) output += "long long";
		} else {
			if (val == 10) output = "bool";
			else if (val == 70) output = "float";
			else if (val == 80) output = "double";
			else if (val == 90) output = "pointer";
		}
		return output;
	}

	throw new Error("JSNative.typeCodeToPrototype: " + val + " is not a JSNative type code");
	return undefined;

}

