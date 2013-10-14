#!bin/jse

var Address = function(){} // placeholder for definition
Address = js.native.container(Object.defineProperties(function Address(p) {
	var ptr = js.native.instance(Address, p);
	return ptr;
}, {
	deallocate:{value:true}
}));

// exceptions will hurt you unless you develop an allocation stack to keep track of pointers
// and catch those exceptions!

b = new Address(0);

echo("Container type: "+js.classOf(b));

delete b;

// the program will now loop until forever to show you that garbage collect
// DOES NOT FINALIZE CLASS INSTANCE EVER!
// SHOULD GARBAGE COLLECT CALL FINALIZE, YOU WILL BE NOTIFIED VIA STDOUT.


