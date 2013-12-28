#!bin/jse

var input = sys.string.toUTF8("Hello World");

var e = 0, scanned, buffer = new sys.memory.pointer(sys.type.long, true);

scanned = sys.string.read.format(input, "%ms", buffer);

if (scanned == 1)
	print.line(sys.string.fromUTF8(buffer.value))
else {
	print.error.line(sys.main, ": unable to scan word!");
	e = 1;
}

sys.memory.free(input, buffer.value); buffer.free();

sys.exit(e);
