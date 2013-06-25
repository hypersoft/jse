#!bin/jse

try {

JSNative.defineAlias("long long", "int 64");
puts("unsigned int64: " + JSNative.typeCode("unsigned int64"));
} catch (e) { puts(e); };
