#!bin/jse

// The way you are 'supposed' to do it:

var printVersion = (function mainLoad() {

var int = native.type.int;
var utf8p = native.type.utf8.constant.pointer;

return Procedure(
	native.engine, int, "printf", [utf8p, utf8p, utf8p, utf8p]).bind(
	this, "%s JSE %s v%s\n", js.vendor, js.codename, js.version
);

})()

printVersion();

// More than one way to skin a 'cat'

//var printf = new Command('printf', '--');
//var vendor = String.fromUTF8(js.vendor); // these are JS managed string copies ;)
//var codename = String.fromUTF8(js.codename);
//var version = String.fromUTF8(js.version);

//printf("%s JSE %s v%s\n", vendor, codename, version);

// Since 'Commands' expect string parameters, the ellipsis interpretation is strictly
// implied.

