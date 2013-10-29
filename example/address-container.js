
// print out arguments
var argv = new Address(js.run.argv, "utf8 *");
var i, puts = new Procedure("jse", "puts", "native", ["int", "utf8 *"]);

for (i = 0; i < js.run.argc; i++) puts(argv[i]);

