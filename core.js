#!bin/jse

var type = JSNative.Type;
var libc = new JSNative.Library("libc.so.6");

libc.findSymbol("puts");
libc.findSymbol("printf");

var puts = new JSNative.Call("default", type("int"), libc.puts, type("char *"));
var printf = new JSNative.Call("ellipsis", type("int"), libc.printf, type("char *"));

var i = new JSNative.Value("int", 0);

i.value = puts("Hello world");

printf("%s %i\n", "Hello World", i);

// test a value cast
echo(i.pointer.cast("int"))

// test an array cast
echo(i.pointer.cast("int", 1)[0])

