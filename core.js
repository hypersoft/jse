#!bin/jse

var input = String.toUTF8("Hello World");

var string = sys.string.indexOf(input, "ll");

print.line(String.fromUTF8(string))

sys.memory.free(input);
