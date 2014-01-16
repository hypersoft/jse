#!bin/jse

Error.debug = "on";

var echo = new sys.command("echo");

var char = Type("char", 1);
echo(Number(char));
for (name in char) echo(name);