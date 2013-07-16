#!bin/jse

var output;

/* If something goes wrong, we can free ALL allocated resources from this point, forward */
var allocator = new JSNative.Allocator();

try {

// Allocate resource in this try block
output = new JSNative.Array("char", "Hello world from JSE -> Dyncall -> libc !");

// load the lib
var libc = JSNative.jsnLoadLibrary("libc.so.6");
// find the procedure
var puts = JSNative.jsnFindSymbol(libc, "puts");
// create a call stack
var vm = JSNative.jsnNewCallVM(4096);

// Push arguments
JSNative.jsnArgPointer(vm, output);

// Make the call
JSNative.jsnCallInt(vm, puts);

throw new Error("This fake error is used to test the native allocator release");

} catch(e) {
	echo(e);
}

// Free ALL allocated resources
allocator.release();

// prove we don't have nothing allocated:
echo("output deallocated:", output.pointer.deallocated);


