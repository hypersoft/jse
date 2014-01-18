#!bin/jse

Error.debug = "on";

var echo = new sys.command("echo");

var sys_type_from_object = function() {
	
};

var sys_type_name = function() {
	var name = [];
	if ("boolean" in this) name.push("boolean");
	return name.join(" ");
};

var sys_type_native = function() {
	var name = [];
	if ("constant" in this) name.push("const");
	if ("boolean" in this) name.push("bool");
	if ("reference" in this) name.push(this.reference, '*', this.reference.native);
	else if ("array" in this) name.push("value"+this.array);
	else name.push("value");
	return name.join(" ");
};

var bool = Type("bool");
bool.array = [15,0]
var boolPtr = Type("bool *");
boolPtr.reference = bool;

for (name in boolPtr) echo(name, '=', boolPtr[name])