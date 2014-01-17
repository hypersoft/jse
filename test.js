#!bin/jse

Error.debug = "on";

var echo = new sys.command("echo");

var char = Type("char");
char.boolean = true;
char.array = [12, 30];
for (name in char) echo(name, '=', char[name]+';');
