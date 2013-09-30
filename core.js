#!bin/jse

var lib = new SharedLibrary("libc.so.6");
var cvm = new CallVM(8);
var utf8 = js.native.toUTF8("Hello World");
cvm.push([4], utf8); /* 4 == int_t */
cvm.call(4, lib.find("puts"));


