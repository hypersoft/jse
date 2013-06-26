#!bin/jse

try {

JSNative.defineAlias("unsigned char", "byte");
puts("byte type code: " + JSNative.typeCode("byte"));
puts("byte type code to prototype: " + JSNative.typeCodeToPrototype(JSNative.typeCode("byte")));

// Each native must be constructed.
byteArray = new JSNative("byte", 32);
puts("byte array type: " + byteArray + " " + JSNative.typeCodeToPrototype(byteArray.type));
puts("byte array count: " + byteArray.count);
puts("byte array pointer: " + byteArray.pointer);

byteArray[0] = 64; // zero is the default value
puts("byte array default value: " + byteArray.value);

// And freed.
byteArray.free();

} catch (e) { puts(e); };
