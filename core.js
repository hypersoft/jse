#!bin/jse


var libc = new JSNative.Library("libc.so.6");

for (name in libc) echo(name)


