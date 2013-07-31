#!bin/jse

var char = JSNative.Type['char'];

echo("typedef char * string")
string = new JSNative.Type(char.pointer, "string")

echo("string ===", string.names[0])
echo("string[] ===", string.array)
echo("string ** ===", string.pointer.pointer)
echo("string *** ===", string.pointer.pointer.pointer)

echo(JSNative.Type.void.pointer.array)
