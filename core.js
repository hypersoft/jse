#!bin/jse

var e = 0, scanned, buffer = new sys.memory.pointer(sys.type.long).toBlock(3, true);

print.error("Enter a date mm/dd/yy: ");

scanned = io.stream.read.format(stdin, "%i/%i/%i", buffer(0), buffer(1), buffer(2));

if (scanned == 3)
	print.line("Month: ", buffer[0], " Day: ", buffer[1], " Year: ", buffer[2])
else {
	print.error.line(sys.main, ": unable to scan date!");
	e = 1;
}

buffer.free();

sys.exit(e);
