#!bin/jse

var buffer = sys.memory.allocate(1, 2);

io.stream.buffer.void(stdin);

io.stream.read(stdin, buffer, 1, 1);
io.stream.write(stderr, buffer, 1, 1);

sys.memory.release(buffer);

