#!bin/jse

Error.debug = "on";

var echo = new sys.command("echo");

var env = new sys.env();

echo(sys.engine.arch);

