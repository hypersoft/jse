#!bin/jse

var utf8p = native.type.utf8.constant.pointer;
var int = native.type.int;

var printf = Procedure(native.engine, int, "printf", [utf8p, '...'])

printf("%s JSE %s v%s\n", js.vendor, js.codename, js.version);

