
var echo = new Command("echo", "-E");

for (name in js.env.keys()) echo("var: " + name);


