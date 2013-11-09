
// This is not a very good example!
// but it demonstrates a working theory...

// create the function to call..
function target(i) {
	return echo("callback target was called with:", i);
}

// setup the callback & prototype...

var cb = Callback(this, 'int', target, ["int"]);

// create a native procedure to test the callback...
var test = Procedure(native.engine, 'int', "jsNativeCallbackTest", ['pointer', "int"]);

test(cb, 12);

cb.free();

