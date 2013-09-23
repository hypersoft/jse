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

	The return value of this function is a context global `JSTObject` named "js".

* Action

	Variadic Macro `JSTAction(identifier, ...)`

	Call `identifier` with ctx as the first parameter, followed by any number of unknown arguments.

	Return value is identified by the prototype designated by `identifier`.

* Utility

	Variadic Declaration Macro `JSTUtility(...)`

	This macro defines the arguments to native entry points requiring both an exception and context parameter. By default these known parameters are ALWAYS labled `exception` and `ctx` respectively.

	The optional argument list supplied to this macro, entails the uncommon argument list of the resulting entry point. For example, a simple native procedure requiring a single object`o` requires three parameters: `ctx, o, exception`. Using `JST()` in conjunction with `JSTUtility()` one can avoid having to specify `ctx` and `exception` parameters.

	Here is a sample function prototype:

		extern JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]);

	which expands to somewhere around the order of:

		extern JSTObject JSTInit_ (JSTContext ctx, JSTObject global, int argc, char * argv[], char * envp[], JSTValue * exception);

* XPR

	Variadic Macro Function `JSTXPR(return expression, expressionN, ...)`

	Evaluates `expressionN` through `...` (in that order), and returns `expression`. This macro is useful for creating inline code blocks.


* CND

	Macro Function `JSTCND(try, true, false)`

	Evaluate expression `try`, if that succeeds, evaluate and return `true`. if `try` failed, evaluate and return `false`.

	`JSTChoose()` is a deprecated copy of this functionality.

2. Context (JSTContext)
-----------------------
Tools for working with JavaScript contexts.

* Context

	Type Macro `JSTContext`

	JST's representation of a JavaScript context reference.

* ContextGroup

	Type Macro `JSTContextGroup`

	JST's representation of a JavaScript context group reference.

* GroupCreate

	Function Macro `JSTContextGroupCreate()`

	Creates a `JSTContextGroup`.

* GroupRetain

	Function Macro `JSTContextGroupRetain(cg)`

	Retains `JSTContextGroup cg`.

* GroupRelease

	Function Macro `JSTContextGroupRelease(cg)`

	Releases `JSTContextGroup cg`.

* Create

	Function Macro `JSTContextCreate()`

	Creates a `JSTContext`.

* CreateInGroup

	Function Macro `JSTContextCreateInGroup(g)`

	Creates a `JSTContext` within  `JSTContextGroup g`.

* Retain

	Function Macro `JSTContextRetain(ctx)`

	Retains `JSTContext ctx`.

* Release

	Function Macro `JSTContextRelease(ctx)`

	Releases `JSTContext ctx`.

* GetGlobalObject

	Function Macro `JSTContextGetGlobalObject(ctx)`

	Retrieve the JavaScript global object for `JSTContext ctx`.

* GetGroup

	Function Macro `JSTContextGetGroup(ctx)`

	Retrieve the `JSTContextGroup` for `JSTContext ctx`.

3. Class (JSTClass)
-------------------
Tools for working with JavaScript class references.

* Class

	Type Macro `JSTClass`

	JST's notion of a JavaScript class reference.

* Accessor

	Type Macro `JSTClassAccessor`

	A static structure that desribes a get/set native callback pair.

* Function

	Type Macro `JSTClassFunction`

	A static structure that desribes a class object method.

* Definition

	Type Macro `JSTClassDefiniton`

	A static structure that desribes the host properties of a class.

* EmptyDefinition

	Type Macro `JSTClassEmptyDefinition`

	An empty 	`JSTClassDefinition`  structure initialized to zero. Useful for creating new `JSTClassDefinition`s.

* Create

	Function Macro `JSTClassCreate(d)`

	Creates a `JSTClass` from `JSTClassDefinition d`.

* Retain

	Function Macro `JSTClassRetain(c)`

	Retains `JSTClass c`.

* Release

	Function Macro `JSTClassRelease(c)`

	Releases `JSTClass c`.

* Instance

	Function Macro `JSTClassInstance(c, p)`

	Creates a `JSTObject` of `JSTClass c` with `void* p` as the associated private object data.

* PropertyNone

	Type Macro `JSTClassPropertyNone`

	A value used in class definitions to specify that a class requires no special handling by the engine internals.

* PropertyManualPrototype

	Type Macro `JSTClassPropertyManualPrototype`

	A value used in class definitions to specify that a class's instance members should inherit nothing on construction.

4. Declare (JSTDeclare)
-----------------------
Tools for declaring native JavaScript entry points.

* Initializer

* Finalizer

* HasProperty

* GetProperty

* SetProperty

* DeleteProperty

* GetPropertyNames

* Function

* Constructor

* HasInstance

* Convertor

7. String (JSTString)
---------------------
Tools for working with JavaScript strings.

* String

	Type Macro `JSTString`

	JST's representation of a JavaScript string reference.

* FromUTF8

* ToUTF8

* FreeUTF8

* FromUTF16

* UTF16

* UTF16Length

* Retain

* Release

* Compare

* CompareToUTF8

* ToValue

* FromValue

8. Function (JSTFunction)
-------------------------
Tools for working with JavaScript functions.

* Call

	Variadic Macro `JSTFunctionCall(fn, obj, ...)`

	Calls `JSTObject fn` as a method of `JSTObject obj` with `JSTValue ...` arguments.

	Returns the result of the call to `fn` as a `JSTValue`.

* Callback

	Function Macro `JSTFunctionCallback(p, f)`

	Creates a native function callback object to entry point `f` declared with `JSTDeclareFunction()` using UTF-8 pointer `p` as the resulting function name.

9. Constructor (JSTConstructor)
-------------------------------
Tools for working with JavaScript contstructors.

* Call

	Variadic Macro `JSTConstructorCall(cn, ...)`

	Calls `JSTObject cn` with `JSTValue ...` arguments.

	Returns the result of the call to `cn` as a `JSTObject`.

* Callback

	Function Macro `JSTConstructorCallback(c, f)`

	Creates a native constructor callback  object to entry point `f` declared with `JSTDeclareConstructor()` using `JSTClass c` as the default object class.

10. Script (JSTScript)
----------------------
Procedures for working with JavaScript evaluation.

* Eval

* CheckSyntax

* NativeEval

* Error

* EvalError

* RangeError

* ReferenceError

* SyntaxError

* TypeError

* URIError

* SetError

* HasError

* Function

11. Native
----------

* Alignment

* ByteOrder
iStackMachine
=============

1. Init
-------

2. GetPrivate
-------------

3. SetPrivate
-------------

4. Units
--------

5. Count
--------

6. Lock
-------

7. Slots
--------

8. Slot
-------

9. Bytes
--------

10. Mass
--------

11. Grow
--------

12. Shrink
----------

13. Compress
------------

14. Free
--------

15. Push
--------

16. Pop
-------

17. PushInt
-----------

18. PopInt
----------

19. Enum
--------

20. EnumReverse
---------------

21. Refactor
------------

22. Slice
---------

23. Read
--------

24. Write
---------
