
var cmd = new Command("echo", "-n");

cmd.capture.output = true;
cmd.capture.time = true;
cmd.capture.error = true; // sometimes not a good idea, you will get the return status !

var ret = cmd("boing", "boing", "boing");

echo("Rabbits go " + ret.toString() + "...");

// So what exactly is in 'ret' ?
echo(JSON.stringify(ret, undefined, '...'));

//startTime && endTime are frozen date objects.

