
// print out arguments
var argv = new Address(js.run.argv, "utf8 *");
var i, puts = new Procedure("jse", "puts", "native", ["int", "utf8 *"]);

function hex(i) { return '0x' + i.toString(16); }

// In C `&' means to get address instead of value ...
// the address returned is a primitve value

// Print out &argv in hex
puts(hex(argv['&']))

// Print out &argv[0] in hex
puts(hex(argv['&0']))

// Print out &argv[1] in hex
puts(hex(argv['&1']))

for (i = 0; i < js.run.argc; i++) puts(argv[i]);

