#!bin/jse

//var echo = new Command('echo', '-E');
//var print = new Command('echo', '-En');
//var printf = new Command('printf', '--');

printVersion = Procedure(
	"jse", "int", "printf", ['utf8 *', 'utf8 *', 'utf8 *', 'utf8 *']).bind(
	this, "%s JSE %s v%s\n", js.vendor, js.codename, js.version
);

printVersion();

// More than one way to skin a 'cat'
var printf = new Command('printf', '--');
var vendor = js.native.fromUTF8(js.vendor); // these are JS managed string copies ;)
var codename = js.native.fromUTF8(js.codename);
var version = js.native.fromUTF8(js.version);

printf("%s JSE %s v%s\n", vendor, codename, version);

// Since 'Commands' expect string parameters, the ellipsis interpretation is strictly
// implied.

