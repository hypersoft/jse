#!bin/jse

var input = String.toUTF8("Hello World");

print.line(sys.string.read.field(input, ' '))

sys.memory.free(input);

