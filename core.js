#!bin/jse

a = sys.memory.read.block(sys.argv, sys.type.pointer, sys.argc);

for (i in a) print.line(i);

