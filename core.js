#!bin/jse

JSNative.CallVM = function() {
	var size = 0;
	for (var i = 0; i < arguments.count; i++) {
		var n = Number(arguments[i]);
		if (n < JSNative.Address.alignment) n = JSNative.Address.alignment;
		size += n;
	}
	var vm = JSNative.jsnNewCallVM(size);
	var mode = {
		value: 0
	};
	Object.defineProperties(vm, {
		constructor: { value: JSNative.CallVM },
		free: { value: JSNative.CallVM.free.bind(vm) },
		error: { get: JSNative.CallVM.error.bind(vm) },
		mode: { set: JSNative.CallVM.mode.bind(vm, mode), get: function() { return mode.value; } },
		reset: { value: JSNative.CallVM.reset.bind(vm, mode) }
	});
	return vm;
}

JSNative.CallVM.free = function() { if (this === JSNative.CallVM) return;
	JSNative.jsnCallVMFree(this);
}

JSNative.CallVM.error = function() { if (this === JSNative.CallVM) return;
	return JSNative.jsnCallVMGetError(this);
}

JSNative.CallVM.mode = function(writeBack, mode) { if (this === JSNative.CallVM) return;
	writeBack.value = mode; JSNative.jsnCallVMSetMode(this, JSNative.CallVM.mode[mode]);
}

Object.defineProperties(JSNative.CallVM.mode, {
	"default": { value: 0, enumerable: true },
	"system default": { value: 200, enumerable: true ),
	"linux system": { value: 201, enumerable: true ),
	"ellipsis": { value: 100, enumerable: true },
	"varargs": { value: 101, enumerable: true },
	"cdecl": { value: 1, enumerable: true },

	// not adding any more because I cannot test them (nor do I plan to...) - pc.wiz.tt

});

JSNative.CallVM.reset = function(writeBack, mode) { if (this === JSNative.CallVM) return;
	writeBack.value = mode; JSNative.jsnCallVMReset(this, JSNative.CallVM.mode[mode]);
}

var output;

try {

/* If something goes wrong, we can free ALL allocated JSNatives from this point, forward */
var allocator = new JSNative.Allocator();

// Allocate resource in this try block
output = new JSNative.Array("char", "Hello world from JSE -> DynCall -> libc -> int puts(char * s); !");

// load the lib
var libc = new JSNative.Library("libc.so.6");

echo(classOf(libc));
// find the procedure
var puts = libc.findSymbol("puts");

// create a call stack
var vm = JSNative.jsnNewCallVM(JSNative.Type("void *").size);

// Push arguments
JSNative.jsnArgPointer(vm, output);

// Make the call
JSNative.jsnCallInt(vm, puts);

throw new Error("This fake error is used to test native allocator release");

} catch(e) {
	echo(e);
	// Free ALL JSNative.* allocated resources
	allocator.release();
}

// prove we don't have any data allocated:
echo("output deallocated:", output.pointer.deallocated);

