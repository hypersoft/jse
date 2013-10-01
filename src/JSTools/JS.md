js
==

The root namespace of JSE's JavaScript additions.
1. run
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
2. native
---------

Available [here](./js.native.md "js.native")