#!bin/jse

var type = JSNative.Type;
var libc = new JSNative.Library("libc.so.6");

libc.findSymbol("puts");
libc.findSymbol("printf");

var puts = new JSNative.Call("default", type("int"), libc.puts, type("void *"));
var printf = new JSNative.Call("ellipsis", type("int"), libc.printf, type("void *"));


puts("Hello world");
printf("%s %i\n", "Hello World", 2, {});

