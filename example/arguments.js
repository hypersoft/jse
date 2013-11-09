
var echo = new Command("echo", "-E");
echo("Argument count:", js.run.argc);

var printf = new Procedure('jse', 'int', "printf", [native.type.utf8.pointer, 'int', 'pointer']);

for (i = 0; i < js.run.argc; i++)
	printf("argv[%i] = '%s'\n", i, native.address.read(js.run.argv + (i*4), native.type.pointer));

