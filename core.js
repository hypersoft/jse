#!bin/jse


var libc = new JSNative.Library("libc.so.6");

var puts = JSNative.jsnFindSymbol(libc, "puts")//libc.findSymbol("puts");

echo(puts)
var vm = JSNative.jsnNewCallVM(4096);

var output = new JSNative.Array("char", "Hello world!");
JSNative.jsnArgPointer(vm, output);
JSNative.jsnCallInt(vm, puts);

//for (name in libc) echo(name)


