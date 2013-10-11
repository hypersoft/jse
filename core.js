#!bin/jse

// exceptions will hurt you unless you develop an allocation stack to keep track of pointers
// and catch those exceptions!

// Up next..
// NativeArray
// Allocator
// NativeFunction

var lib = new SharedLibrary(); // no name gets symbols from JSE ...

var cvm = new CallVM(8);
var utf8 = js.native.toUTF8("%ls\n");
var utf32 = js.native.toUTF32("Hello World");

cvm.push([4, 4], utf8, utf32);      /* 4 == int_t */
cvm.call(4, lib.find("printf"));

// yeah.. so clean that shit up...
js.native.free(utf8, utf32, cvm);

