js
==

	The root namespace of JSE's JavaScript additions.
1. native
---------

	Native API
* alignment

	Number `js.native.alignment`

	Host alignment obtained at compile time.

* byteOrder

	Number `js.native.byteOrder`

	Host byte ordering info obtained from glib.

* typeSize

	Function `js.native.typeSize(t)`

	A JavaScript sizeof operator for native types.

	returns the size of `JSTNativeType t`

* toUTF8

	Function `js.native.toUTF8(v)`

	A data conversion function that turns JavaScript values into UTF8 string buffers.

	These buffers must be freed via a call to `js.native.freeUTF8()`
* fromUTF8

	Function `js.native.fromUTF8(p)`

	Conversion function that turns UTF8 buffers, into JavaScript `String`s.

* freeUTF8

	Fucntion `js.native.freeUTF8(p)`

	Deallocates storage reserved for UTF8 buffers. Obtained through the `js.native` interface.

* lib

	Shared Library Cache. The API itself does not manipulate this cache in anyway. The class responsible for managing shared libraries is allotted this namespace as an intermediate storage facility.

* library

	Native Shared Library Utilities.
  - load

	Attempts to locate and load into memory a Shared Object library by the given name.
  - free

	Releases a loaded library from memory.
  - findSymbol

	Searches library for symbol matching name.

* callVM

	Native Function Invocation Utilities.
  - create

	Creates a native Call Virtual Machine with a specific stack size.
  - free

	Deallocates the CallVM and its stack.
  - getError

	Retrieves the last error recorded by a CallVM.
  - setMode

	Changes the underlying ABI behind the CallVM function invocation routines.
  - reset

	Clears the CallVM stack to make ready for the next procedure call.
  - push

	Push typed arguments into a CallVM.
  - call

	Invoke a procedure call with the current CallVM stack as the arguments to the procedure.

2. run
------

	Runtime initialization records.
* argc

	The number of application command line arguments recieved at startup.

* argv

	Pointer to the application command line parameters.

* envp

	Pointer to the application environment vectors.

* uid

	The user's session login identity.

* euid

	The user's effective credentials.
* gid

	The user's group identifier.
* pid

	The application process ID.
* path

	The working directory of the application at startup.

* date

	The date recorded when this application was initialized.