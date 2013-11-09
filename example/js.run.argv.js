
echo("Argument count:", js.run.argc);

var ptr = native.type.void.pointer;
var int = native.type.int;
var utf8p = native.type.utf8.pointer;

var printf = new Procedure('jse', int, "printf", [utf8p, int, ptr]);

for (i = 0; i < js.run.argc; i++)
	printf("argv[%i] = '%s'\n", i, native.address.read(js.run.argv + (i*ptr.size), ptr));

