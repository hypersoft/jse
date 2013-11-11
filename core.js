#!bin/jse

var utf8p = native.type.utf8.constant.pointer;
var int = native.type.int;

// We need some pointers to setup as 'type qualified values'
var vendor = utf8p.value(js.vendor);
var codename = utf8p.value(js.codename);
var version = utf8p.value(js.version);

// we can demonstrate automatic utf8 pointer conversion of String objects
var auto = "Featuring JavaScriptCore"

// Number types are auto converted to 'int' but we don't show them here.

var printf = Procedure(native.engine, int, "printf", [utf8p, '...'])

printf("%s JSE %s %s %s\n", vendor, codename, version, auto);

