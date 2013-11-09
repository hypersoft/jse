
// All Number objects have the 'toAddress' convertor...
// All Number objects have the 'toHex' convertor...

var utf8p = native.type.utf8.pointer;
var cUtf8p = utf8p.constant;

// toAddress is an applied shortcut for... Address(number, type, count)
var argv = js.run.argv.toAddress(cUtf8p, js.run.argc);
argv[0] = "blue";
var i, puts = new Procedure(native.engine, native.type.int, "puts", [utf8p]);

// In C `&' means to get address instead of value ...
// the address returned is a primitve value

// Print out &argv as object->string
puts(argv)

// print out &argv in hex
puts(argv['&'].toHex())

// Print out &argv[0] in hex
puts(argv['&0'].toHex())

// Print out &argv[1] in hex
puts(argv['&' + 1].toHex())

for (i in argv) puts(argv[i]);

