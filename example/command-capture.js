
var cmd = new Command("echo", "-n");

cmd.capture.output = true;

var ret = cmd("boing", "boing", "boing");

var echo = new Command("echo");

echo("Rabbits go", String(ret) + "...");

// So what exactly is in 'ret' ?
echo(JSON.stringify(ret, undefined, '...'));
