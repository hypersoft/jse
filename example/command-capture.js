
var cmd = new Command("echo", "-n");

cmd.capture.output = true;

var ret = cmd("boing", "boing", "boing");

var echo = new Command("echo");

echo("Rabbits go", ret);

