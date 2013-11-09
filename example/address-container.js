
// All Number objects have the 'toAddress' convertor...
// All Number objects have the 'toHex' convertor...

var utf8p = native.type.utf8.pointer;

var argv = js.run.argv.toAddress(utf8p, js.run.argc);

//Command('echo')()

var i, puts = new Procedure(native.engine, 'int', "puts", [utf8p]);

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

