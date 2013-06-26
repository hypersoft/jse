#!bin/jse

try {

// Mockup runtime c interpreter for operational clarity...
puts("typedef unsigned char byte;");
	JSNative.defineAlias("unsigned char", "byte");
	puts(JSNative.typeCodeToPrototype(JSNative.typeCode("byte")) + " (" + JSNative.typeCode("byte") + ")");

// Each native must be constructed.
puts("byte byteArray[32];");
	byteArray = new JSNative("byte", 32);
	puts(byteArray.pointer);

puts("byteArray[0] = 64;");
	byteArray[0] = 64; // zero is the default value
	puts(byteArray[0]);

// The void type provides a fully customizable JSNative without an allocated data backing
puts("void * pVoid = (void *) byteArray;");
	pVoid = new JSNative("void");
	pVoid.pointer = byteArray.pointer; pVoid.count = 1;
	puts(pVoid.pointer);

puts("(unsigned char *) *pVoid;");
	pVoid.type = byteArray.type;
	puts(pVoid.value);

// All JSNatives require private data, which must be freed, before the value is garbage collected.
pVoid.free(); byteArray.free();


} catch (e) { puts(e); };
