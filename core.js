#!bin/jse

var echo = new Command('echo', '-E');
var print = new Command('echo', '-En');
var printf = new Command('printf', '--');

// exceptions will hurt you unless you develop an allocation stack to keep track of pointers
// and catch those exceptions!

var int = js.type.int;
var native = js.call.native;

var puts = new Procedure(js.engine, 'puts', native, [int, js.type.address(js.type.utf8)]);

print("Hello to you "), puts(js.user.name);

exit(0);

