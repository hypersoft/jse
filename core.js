#!bin/jse

var ptr = sys.type.pointer

var buffer = sys.memory.block(ptr, 0, 1);
var length = sys.memory.allocate(sys.type.size, 1);

var file = sys.memory.stream(buffer, length);

io.stream.print(file, "fuck");
io.stream.close(file);

print.line(sys.fromUTF8(buffer[0]));


