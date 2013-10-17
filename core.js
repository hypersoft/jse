#!bin/jse

var echo = new Command('echo', '-E');
var print = new Command('echo', '-En');
//var printf = new Command('printf', '--');

var int = js.type.int;
var utf8p = js.type.utf8 | js.type.pointer;
var utf32p = js.type.utf32 | js.type.pointer;
var mode = js.call.ellipsis;

var printf = new Procedure(js.engine, 'printf', mode, [int, utf8p]);

// the following method is only available for js.call.ellipsis
// setup ellipsis call: one utf8 pointer (call managed encoding & allocation)
printf.parameters(utf32p); // we have to setup our ... calls because we don't mark types explicitly,
// as a compiler would at compile time.

// you can call this parameter set as many times as you like.
printf("%ls\n", "Hello " + js.user.name);

// BE VERY CAREFUL ABOUT SWITCHING PARAMETER TYPES.
// If you get Seg Faults, check that your parameters prototype actually matches your
// supplied parameters.

// let's do a new prototype based on the definition from line 15

printf.parameters(utf8p);
printf("%s\n", "Goodbye.");

