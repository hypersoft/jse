
// This is not a very good example!
// but it demonstrates a working theory...

var echo = new Command('echo', '-E');

// create the function to call..
function target(i) {
	return echo("callback target was called with:", i);
}


// setup the callback & prototype...
var cb = Callback(this, target, ["int", "int"]);

// create a native procedure to test the callback...
var test = Procedure("jse", "jsNativeCallbackTest", "native", ["int", "void *", "int"]);

test(cb, 12);

cb.free();

