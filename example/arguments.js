
var echo = new Command("echo", "-E");
echo("Argument count:", js.run.argc);


var printf = new Procedure(js.engine, "printf", js.call.ellipsis, [js.type.int, js.type.utf8 | js.type.pointer]);
printf.parameters(js.type.int, js.type.utf8 | js.type.pointer);

for (i = 0; i < js.run.argc; i++)
	printf("argv[%i] = '%s'\n", i, js.native.address.read(js.run.argv + (i*4), js.type.pointer));

