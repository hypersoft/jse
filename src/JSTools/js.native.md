js.native
=========

	Native API
1. library
----------

	Native Shared Library Utilities.

* load

	Attempts to locate and load into memory a Shared Object library by the given name.

* free

	Releases a loaded library from memory.

* findSymbol

	Searches library for symbol matching name.

2. callVM
---------

	Native Function Invocation Utilities.

* create

	Creates a native Call Virtual Machine with a specific stack size.

* free

	Deallocates the CallVM and its stack.

* getError

	Retrieves the last error recorded by a CallVM.

* setMode

	Changes the underlying ABI behind the CallVM function invocation routines.

* reset

	Clears the CallVM stack to make ready for the next procedure call.

* push

	Push typed arguments into a CallVM.

* call

	Invoke a procedure call with the current CallVM stack as the arguments to the procedure.
