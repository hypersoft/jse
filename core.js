#!bin/jse

var input = sys.string.toUTF8("Hello World");

var string = sys.string.read(input, 4);

print.line(string)

sys.memory.free(input);
