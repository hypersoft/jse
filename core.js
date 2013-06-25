#!bin/jse

try {

JSNative.defineAlias("long long", "int64");
puts("unsigned int64: " + JSNative.typeCode("unsigned int64"));
} catch (e) { puts(e); };
