#!bin/jse

var buffer = new sys.memory.pointer(sys.type.pointer, true);
var length = new sys.memory.pointer(sys.type.size, true);

// open memory stream
var file = sys.memory.stream(buffer, length);

// print to memory stream
io.stream.print(file, "wow this text was printed in a memory stream");

// close memory stream
io.stream.close(file);

// print the data from the stream buffer
print.line(sys.fromUTF8(buffer.value));

// free the buffers
sys.memory.free(buffer.value), buffer.free(), length.free();

