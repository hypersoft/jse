
// This is not a very good example!
// but it demonstrates a working theory...

var echo = new Command('echo', '-E');

// create the function to call..
function target(i) {
	return echo("callback target was called with:", i);
}


// setup the callback & prototype...

var cb = Callback(this, 'int', target, ["int"]);

// create a native procedure to test the callback...
var test = Procedure("jse", 'int', "jsNativeCallbackTest", ["void *", "int"]);

test(cb, 12);

cb.free();

