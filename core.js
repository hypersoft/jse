#!bin/jse

var buffer = sys.memory.block(sys.type.pointer, 1);
var length = sys.memory.allocate(sys.type.size, 1);

var file = sys.memory.stream(buffer, length);

io.stream.print(file, "fuck");
io.stream.close(file);

print.line(sys.fromUTF8(buffer[0]));


