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

	The optional argument list supplied to this macro, entails the uncommon argument list of the resulting entry point. For example, a simple native procedure requiring a single object`o` requires three parameters: `ctx, o, exception`. Using `JST()` in conjunction with `JSTUtility()` one can avoid having to specify `ctx` and `exception` parameters.

	Here is a sample function prototype:

		extern JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]);

	which expands to somewhere around the order of:

		extern JSTObject JSTInit_ (JSTContext ctx, JSTObject global, int argc, char * argv[], char * envp[], JSTValue * exception);

* XPR

	Variadic Macro Function `JSTXPR(return expression, expressionN, ...)`

	Evaluates `expressionN-...`, and returns `expression`. This macro is useful for creating inline code blocks.


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

3. Class (JSTClass)
-------------------
Tools for working with JavaScript class references.

* Class

	Type Macro `JSTClass`

	JST's notion of a JavaScript class reference.

4. Declare (JSTDeclare)
-----------------------
Tools for declaring native JavaScript class entry points.

5. Type (JSTType)
-----------------
Tools for working with JavaScript types.

Each of the following Value Macros correspond to JST's notion of the specified type. One obtains the type of a `JSTValue` by calling `JSTValueGetType()`. These macros are explicitly for comparison with such results, but are also viable as an argument to an entry point defined by `JSTDeclareConvertor()`.

* Undefined

	Value Macro `JSTUndefined`

	JST's notion of the JavaScript `undefined` type.

* Null

	Value Macro `JSTNull`

	JST's notion of the JavaScript `null` type.

* Boolean

	Value Macro `JSTBoolean`

	JST's notion of the JavaScript boolean type.

* Number

	Value Macro `JSTNumber`

	JST's notion of the JavaScript number type.

* String

	Value Macro `JSTString`

	JST's notion of the JavaScript string type.

* Object

	Value Macro `JSTObject`

	JST's notion of the JavaScript object type.

6. Value (JSTValue)
-------------------
Tools for working with JavaScript values.

* Value

	Type Macro `JSTValue`

	JST's notion of a JavaScript value reference.

* GetType

	Function Macro `JSTValueGetType(v)`

	Returns the `JSTType` value of `v`.

* IsBoolean

	Function Macro `JTValueIsBoolean(v)`

	Returns boolean true if the type of `v` is equivalent to  `JSTTypeBoolean`.


* IsClassInstance

	Function Macro `JSTValueIsClassInstance(v, c)`

	Tests whether JavaScript `v` is an object with `JSTClass` `c` in its class chain.


* IsConstructor

	Function Macro `JSTValueIsConstructor(v)`

	Returns boolean true if `v` is a JavaScript object constructor.

* IsConstructorInstance

	Function Macro `JSTValueIsConstructorInstance(v, c)`

	Returns boolean true if `c` is the constructor of `v`.


* IsEqual

	Function Macro `JSTValueIsEqual(a, b)`

	Returns boolean true based on the equality of `a` and `b`.

* IsFunction

	Function Macro `JSTValueIsFunction(v)`

	Returns boolean true if `v` is a JavaScript function.

* IsNull

	Function Macro `JTValueIsNull(v)`

	Returns boolean true if `v` is equivalent to the JavaScript value `null`.

* IsNumber

	Function Macro `JTValueIsNumber(v)`

	Returns boolean true if the type of `v` is equivalent to  `JSTTypeNumber`.

* IsObject

	Function Macro `JTValueIsObject(v)`

	Returns boolean true if the type of `v` is equivalent to  `JSTTypeObject`.


* IsStrictEqual

	Function Macro `JSTValueIsStrictEqual(a, b)`

	Returns boolean true based on the JavaScript strict equality (`===`) of `a` and `b`.

* IsString

	Function Macro `JTValueIsString(v)`

	Returns boolean true if the type of `v` is equivalent to  `JSTTypeString`.


* IsUndefined

	Function Macro `JTValueIsUndefined(v)`

	Returns boolean true if `v` is equivalent to the JavaScript value `undefined`.

* IsVoid

	Function Macro `JTValueIsVoid(v)`

	Returns boolean true if `v` is NULL, or equivalent to the JavaScript values `undefined` or `null`.

* ToBoolean

	Function Macro `JSTValueToBoolean(v)`

	Converts the value `v` to native `bool` type.

* ToDouble

	Function Macro `JSTValueToDouble(v)`

	Converts the value `v` to native `double` type.

* ToJSONString

	Function Macro `JSTValueToJSONString(v)`

	Converts value `v` to `UTF-8` encoded `JSON` format.

* ToObject

	Function Macro `JSTValueToObject(v)`
	Qualify `v` as a `JSTObject`.

* ToPointer

	Function Macro `JSTValueToPointer(v)`

	Converts the JavaScript number value `v` to the type `void *`.

* ToString

	Function Macro `JSTValueToString(v)`

	Creates a `JSTString` from `v`.

	Returns the new `JSTString`
.
* Null

	Value Macro `JSTValueNull(v)`

	Returns a reference to the JavaScript type `null`.

* Undefined

	Value Macro `JSTValueUndefined(v)`

	Returns a reference to the JavaScript type `undefined`.

* Protect

	Function Macro `JSTValueProtect(v)`

	Protect `v` from garbage collection.

* Unprotect

	Function Macro `JSTValueUnprotect(v)`

	Flag `v` as a potential candidate for garbage collection.

7. Property (JSTProperty)
-------------------------
Attributes for `JSTObjectSetProperty()`

* Hidden

	Value Macro `JSTPropertyHidden`

	The value is not enumerable.

* ReadOnly

	Value Macro `JSTPropertyReadOnly`

	The value is not writable.

* Required

	Value Macro `JSTPropertyRequired`

	The property may not be deleted from the object.

8. Object (JSTObject)
---------------------
Tools for working with JavaScript objects.

* Object

	Type Macro `JSTObject`

	JST's representation of a JavaScript object reference.

9. String (JSTString)
---------------------
Tools for working with JavaScript strings.

* String

	Type Macro `JSTString`

	JST's representation of a JavaScript string reference.

10. Function (JSTFunction)
--------------------------
Tools for working with JavaScript functions.

11. Constructor (JSTConstructor)
--------------------------------
Tools for working with JavaScript contstructors.

12. Script (JSTScript)
----------------------
Procedures for working with JavaScript evaluation.
