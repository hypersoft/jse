#!bin/jse

var output;

try {

/* If something goes wrong, we can free ALL allocated JSNatives from this point in the
   JS Execution chain, forward by using a global allocator reference
*/ var allocator = new JSNative.Allocator();

var libc = new JSNative.Library("libc.so.6");

// int puts(char * string);
var int = JSNative.Type("int");
var puts = libc.findSymbol("puts");
var ptr = JSNative.Type("void *");

/* setup the call invocation stack; CallVMs are tracked by the global allocator,
	so this resource and all of its resources will be freed upon a global allocation release.
*/
var vm = new JSNative.CallVM(ptr);

/* 
	JS Strings are auto-converted to native char array, and deallocated on reset....,
	or upon execution of a global deallocation by an allocator guarding the current
	JS call chain.
*/

/* push argument(s), call procedure, reset; reset ALWAYS clears the call stack */
vm.push("Hello World... from JS String!"); vm.call(int, puts);
vm.reset();

/* traditionally, and optimally, you will want to have your own pointers to your own data */
output = new JSNative.Array("char", "Hello world from JSNative char Array!");
vm.push(output); vm.call(int, puts);
/* we are going to free "output" later, for example purposes */

// call VMs are managed so we don't need to call this, we are going to throw an error,
// and release it using the global allocator reference we obtained earlier.
//vm.free();

throw new Error("This fake error is used to test native allocator release");

} catch(e) {
	echo(e);
	/* free our example allocated data, as well as anything we might have forgotten... */
	allocator.release();
}

// prove we don't have any data allocated:
echo("output deallocated:", output.pointer.deallocated);
echo("CallVM deallocated:", vm.deallocated);

