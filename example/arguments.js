
var echo = new Command("echo", "-E");
echo("Argument count:", js.run.argc);

js.type['utf8 *'] = js.type.utf8 | js.type.pointer;

var printf = new Procedure(js.engine, "printf", js.call.ellipsis, ["int"]);

printf.parameters("utf8 *", "int", "utf8 *");

for (i = 0; i < js.run.argc; i++)
	printf("argv[%i] = '%s'\n", i, js.native.address.read(js.run.argv + (i*4), js.type.pointer));

