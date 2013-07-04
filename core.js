#!bin/jse

try {

var dword = new JSNative.Type.Alias("long", "dword");

puts(JSNative.Type(dword));

} catch (e) { puts(e); };

