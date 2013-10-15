#!bin/jse

var echo = new Command('echo', '-E');
var print = new Command('echo', '-En');
//var printf = new Command('printf', '--');

// exceptions will hurt you unless you develop an allocation stack to keep track of pointers
// and catch those exceptions!

var int = js.type.int;
var utf8p = js.type.utf8 | js.type.pointer;
var utf32p = js.type.utf32 | js.type.pointer;
var ellipsis = js.call.ellipsis;

var printf = new Procedure(js.engine, 'printf', ellipsis, [int, utf8p]);

// setup ellipsis call: one utf8 pointer (call managed encoding & allocation), one utf32 pointer (call managed encoding & allocation)
printf.parameters(utf8p, utf32p); // we have to setup our ... calls because we don't mark types explicitly,
// as a compiler would at compile time.

// you can call this parameter set as many times as you like.
printf("%s %ls!\n", "Hello", js.user.name);
printf("%s %ls?\n", "How", "are you today");

// setting the parameters again will reset the procedure prototype to the base defintion (supplied at construction)

exit(0);

