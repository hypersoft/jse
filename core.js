#!bin/jse

var stream = io.stream.process.open("cat", "w");

io.stream.print.line(stream, "Hello World");

io.stream.process.close(stream);
