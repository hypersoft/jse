#!bin/jse

b = sys.memory.block(null, sys.type.pointer, 16);

print.line(b.addressOf(15));

b.free()
