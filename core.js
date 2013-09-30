#!bin/jse

var libc = js.native.library.load('');
var putc = js.native.library.findSymbol(libc, 'putc');
var cvm = js.native.callVM.create(1024);
js.native.callVM.push(cvm, 4, 77);
if (cvm != 0) js.native.callVM.call(cvm, 12, putc);
