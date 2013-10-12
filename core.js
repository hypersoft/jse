#!bin/jse

// exceptions will hurt you unless you develop an allocation stack to keep track of pointers
// and catch those exceptions!

var jse = new SharedLibrary();
var utf8 = js.native.toUTF8("Hello "+js.user.name+", from jse->puts!");
var puts = js.native.callVM.exec.bind(jse.find("puts"), 0, [4, 4]);
var fflush = js.native.callVM.exec.bind(jse.find('fflush'), 0, [4, 4]);

puts(utf8); fflush(0);

js.native.free(utf8);

exit(0);

