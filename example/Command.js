
var cmd = new Command("echo", "-n");

cmd.capture.output = true;

var ret = cmd("boing", "boing", "boing");

echo("Rabbits go", String(ret) + "...");

// So what exactly is in 'ret' ?
echo(JSON.stringify(ret, undefined, '...'));
