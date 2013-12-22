#!bin/jse

b = sys.memory.block(null, sys.type.pointer, 16);

print.line(Object.keys(b));

b.free()
