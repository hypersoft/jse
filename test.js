#!bin/jse

Error.debug = "on";

var echo = new sys.command("echo");

var bool = Type("bool", 0);
bool.width = 1;

echo("bool is integer: "+bool.integer);

var single = Type("float", 0);

single.float = 1;

echo("Single Precision Float");
for (name in single) echo("  "+name+": "+single[name]);
