#!bin/jse

var ptr = sys.type.pointer, size = sys.type.size;

var buffer = sys.memory.block(0, ptr, 1);
var length = sys.memory.allocate(size, 1);

var file = sys.memory.stream(buffer, length);

io.stream.print(file, "fuck");
io.stream.close(file);

print.line(sys.fromUTF8(buffer[0]));


