#!bin/jse

alloc = new JSNative.Allocator();

v = new JSNative.Array("char", "Mr. Wizzard");

echo(v);

v[0] = "S";

b = v[0];
alloc.release();

echo("Hell"+ b.container.deallocated +"o");

