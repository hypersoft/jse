#!bin/jse

var print = function(msg) {
	writeOutput(msg + "\n");
}

print(JSNative.Type("short").name);
