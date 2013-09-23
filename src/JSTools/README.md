JST (JavaScriptTools)
=====================
Copyright (C) 2013 Triston J. Taylor (Hypersoft Systems); All Rights Reserved.

1. Base
-------
Miscellaneous Assistance Macros.

Items in this category don't carry the section heading name 'Base'. Because of that fact, one has to be very careful about (re)-naming macros defined in this section. Familiarity with content is key.

* JST

	Variadic Macro `JST(identifier, ...)`

	Call upon native `JSTUtility` function `identifier` with a varying number of intermittent arguments and return value types.

	The generated native function call must match the native function prototype of `identifier` to compile properly.

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

* Utility

	Variadic Declaration Macro `JSTUtility(...)`

	This macro defines the arguments to native entry points requiring both an exception and context parameter. By default these known parameters are ALWAYS labled `exception` and `ctx` respectively.

	The argument list supplied to this macro, entails the uncommon argument list of the resulting entry point. For example, a simple native procedure requiring a single object`o` requires three parameters: `ctx, o, exception`. Using `JST()` in conjunction with `JSTUtility()` one can avoid having to specify `ctx` and `exception` parameters.
