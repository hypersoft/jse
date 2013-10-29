
// All Number objects have the 'toAddress' convertor...
// All Number objects have the 'toHex' convertor...

var argv = js.run.argv.toAddress("utf8 *", js.run.argc);
var i, puts = new Procedure("jse", "puts", "native", ["int", "utf8 *"]);

// In C `&' means to get address instead of value ...
// the address returned is a primitve value

// Print out &argv as object->string
puts(argv['&'].toAddress())

// Print out &argv[0] in hex
puts(argv['&0'].toHex())

// Print out &argv[1] in hex
puts(argv['&' + 1].toHex())

for (i in argv) puts(argv[i]);

