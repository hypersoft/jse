JST (JavaScriptTools)
=====================
Copyright (C) 2013 Triston J. Taylor (Hypersoft Systems); All Rights Reserved.

1. Base
-------
Miscellaneous Assistance Macros.

Items in this category don't carry the section heading name 'Base'. Because of that fact, one has to be very careful about (re)-naming macros defined in this section. Familiarity with content is key.

* JST

	Variadic Macro `JST(pname, ...)`

	Call upon native `JSTUtility` function `pname` with a varying number of intermittent arguments and return value types.

	The generated native function call must match the native function prototype of `pname` to compile properly.

	This macro is typically called by other macros to reduce coding while improving quality, correctness and efficiency.

* Init

	Function Macro `JSTObject JSTInit(JSTObject global, int argc, char *argv[], char *envp[])`

	Instruct JST to initialize all static data, and runtime operability.

	The provided JavaScript object `global` is populated with the standard JST scripting environment provisions.

	The target function of this macro is defined as `src/JSTools.c:JSTObject JSTInit_ JSTUtility` and is declared external by `src/JSTools.h`.

	The return value of this function is a context global JSTObject named "js".

* Action

	Variadic Macro `JSTAction(identifier, ...)`

	Call `identifier` with ctx as the first parameter, followed by any number of unknown arguments.

	Return value is identified by the prototype designated by `identifier`.
