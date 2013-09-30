#!bin/jse

var lib = new SharedLibrary("libc.so.6");
var cvm = new CallVM(8);
cvm.push([4], 77); /* 4 == int_t */
cvm.call(4, lib.find("putchar"));


