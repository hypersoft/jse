#!bin/jse

var echo = new Command('echo', '-E');
var print = new Command('echo', '-En');
var printf = new Command('printf', '--');

// exceptions will hurt you unless you develop an allocation stack to keep track of pointers
// and catch those exceptions!

var int = jse.proto.int;
var native = jse.call.native;

var utf8 = js.native.toUTF8("I am hailing you from jse->puts!");
var puts = new Procedure(jse, 'puts', native, [int, int]);

echo('Hello', js.user.name+'.'); puts(utf8);

js.native.free(utf8);

exit(0);

