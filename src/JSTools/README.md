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

* Property
  - None
  - ManualPrototype

4. Declare (JSTDeclare)
-----------------------
Tools for declaring native JavaScript class entry points.

5. Value (JSTValue)
-------------------
Tools for working with JavaScript values.

* Value

	Type Macro `JSTValue`

	JST's notion of a JavaScript value reference.

* FromBoolean

	Function Macro `JSTValueFromBoolean(b)`

	Converts value `b` to the JavaScript class `Boolean`.

* FromDouble

	Function Macro `JSTValueFromDouble(d)`

	Converts `double` value `d` to the JavaScript class `Number`.

* FromJSON

	Function Macro `JSTValueFromJSON(p)`

	Converts UTF-8 `JSON` string pointer `p` to a `JSTValue` of JavaScript class `Object`.

* FromPointer

	Function Macro `JSTValueFromPointer(p)`

	Converts the pointer value `p` to the JavaScript class `Number`.

* FromString

	Function Macro `JSTValueFromString(s, f)`

	Converts `JSTString s` to a JavaScript value class `String` and if `f` evaluates to true, `JSTStringRelease()` will be called with `s` to trigger string deallocation.

* FromUTF8

	Function Macro `JSTValueFromUTF8(p)`

	Converts UTF-8 string pointer value `p` to the JavaScript value class `String`.

* GetType

	Function Macro `JSTValueGetType(v)`

	Returns the `JSTValueType` value of `v`.

* IsBoolean

	Function Macro `JSTValueIsBoolean(v)`

	Returns `bool` true if the type of `v` is equivalent to  `JSTValueTypeBoolean`.


* IsClassInstance

	Function Macro `JSTValueIsClassInstance(v, c)`

	Tests whether JavaScript `v` is an object with `JSTClass c` in its class chain.


* IsConstructor

	Function Macro `JSTValueIsConstructor(v)`

	Returns `bool` true if `v` is a JavaScript object constructor.

* IsConstructorInstance

	Function Macro `JSTValueIsConstructorInstance(v, c)`

	Returns `bool` true if `c` is the constructor of `v`.


* IsEqual

	Function Macro `JSTValueIsEqual(a, b)`

	Returns `bool` true based on the equality (`==`) of `a` and `b`.

* IsFunction

	Function Macro `JSTValueIsFunction(v)`

	Returns `bool` true if `v` is a JavaScript function.

* IsNull

	Function Macro `JSTValueIsNull(v)`

	Returns `bool` true if `v` is equivalent to the JavaScript value `null`.

* IsNumber

	Function Macro `JSTValueIsNumber(v)`

	Returns `bool` true if the type of `v` is equivalent to  `JSTValueTypeNumber`.

* IsObject

	Function Macro `JSTValueIsObject(v)`

	Returns `bool` true if the type of `v` is equivalent to  `JSTValueTypeObject`.


* IsStrictEqual

	Function Macro `JSTValueIsStrictEqual(a, b)`

	Returns `bool` true based on the JavaScript strict equality (`===`) of `a` and `b`.

* IsString

	Function Macro `JSTValueIsString(v)`

	Returns `bool` true if the type of `v` is equivalent to  `JSTValueTypeString`.


* IsUndefined

	Function Macro `JSTValueIsUndefined(v)`

	Returns `bool` true if `v` is equivalent to the JavaScript value `undefined`.

* IsVoid

	Function Macro `JSTValueIsVoid(v)`

	Returns `bool` true if `v` is `NULL`, or equivalent to the JavaScript values `undefined` or `null`.

* ToBoolean

	Function Macro `JSTValueToBoolean(v)`

	Converts value `v` to native type `bool`.

* ToDouble

	Function Macro `JSTValueToDouble(v)`

	Converts value `v` to native type  `double`.

* ToJSON

	Function Macro `JSTValueToJSON(v, i)`

	Converts value `v` to UTF-8 encoded `JSON` format. `i` is the number of spaces to indent.

* ToObject

	Function Macro `JSTValueToObject(v)`

	Promotes value `v` to type `JSTObject`.

* ToPointer

	Function Macro `JSTValueToPointer(v)`

	Converts JavaScript number value `v` to type `void *`.

* ToString

	Function Macro `JSTValueToString(v)`

	Creates a `JSTString` from `v`.

	Returns the string value of `v` as a new `JSTString`.

	You must free the returned `JSTString` by calling `JSTStringRelease()`.

* Null

	Function Macro `JSTValueNull()`

	Returns a reference to the JavaScript value `null`.

* Undefined

	Function Macro `JSTValueUndefined()`

	Returns a reference to the JavaScript value `undefined`.

* Protect

	Function Macro `JSTValueProtect(v)`

	Protect `v` from garbage collection.

* Unprotect

	Function Macro `JSTValueUnprotect(v)`

	Flag `v` as a potential candidate for garbage collection.

* TypeUndefined

	Value Macro `JSTValueTypeUndefined`

	JST's notion of the JavaScript `undefined` value class.

* TypeNull

	Value Macro `JSTValueTypeNull`

	JST's notion of the JavaScript `null` value class.

* TypeBoolean

	Value Macro `JSTValueTypeBoolean`

	JST's notion of the JavaScript `Boolean` value class.

* TypeNumber

	Value Macro `JSTValueTypeNumber`

	JST's notion of the JavaScript `Number` value class.

* TypeString

	Value Macro `JSTValueTypeString`

	JST's notion of the JavaScript `String` value class.

* TypeObject

	Value Macro `JSTValueTypeObject`

	JST's notion of the JavaScript `Object` value class.

6. Object (JSTObject)
---------------------
Tools for working with JavaScript objects.

* Object

	Type Macro `JSTObject`

	JST's representation of a JavaScript object reference.

* GetPrototype

	Function Macro `JSTObjectGetPrototype(o)`

	Retrieves the object prototype of `o` as a `JSTObject`

* SetPrototype

	Function Macro `JSTObjectSetPrototype(o, p)`

	Sets `JSTObject p` as the prototype property of `JSTObject o`.

* HasProperty

	Function Macro `JSTObjectHasProperty(o, p)`

	Returns `bool` true if `JSTObject o` has a property by the name of UTF-8 pointer `p`.

* GetProperty

	Function Macro `JSTObjectGetProperty(o, p)`

	Returns the property named by UTF-8 pointer `p` in `JSTObject o` or the JavaScript value `undefined` if the property is not defined on `o`.

* SetProperty

	Function Macro `JSTObjectSetProperty(o, p, v, a)`

	Sets property `JSTValue v` as a member of `JSTObject o` by the name of UTF-8 pointer `p`, with `JSTObjectProperty a` access attributes.
 
* DeleteProperty

	Function Macro `JSTObjectDeleteProperty(o, p)`

	Attempts to remove the property named by UTF-8 pointer `p` in `JSTObject o`.

* GetPropertyAtIndex

	Function Macro `JSTObjectGetPropertyAtIndex(o, i)`

	Returns the property index specified by `unsigned i` in `JSTObject o` or the JavaScript value `undefined` if the property is not defined on `o`.

* SetPropertyAtIndex

	Function Macro `JSTObjectSetProperty(o, i, v, a)`

	Sets property `JSTValue v` as a member of `JSTObject o` by the index of `unsigned i`, with `JSTObjectProperty a` access attributes.

* GetPrivate

	Function Macro `JSTObjectGetPrivate(o)`

	Retrieves the private `void*` address associated with `JSTObject o`.

* SetPrivate

	Function Macro `JSTObjectSetPrivate(o, p)`

	Sets `void* p` as the private data associated with `JSTObject o`.

* IsFunction

	Function Macro `JSTObjectIsFunction(o)`

	Returns `bool` true if `o` is a JavaScript function.

* IsConstructor

	Function Macro `JSTObjectIsConstructor(c)`

	Returns `bool` true if `c` is a JavaScript constructor.

* ToValue

	Function Macro `JSTObjectToValue(o)`

	Performs a type cast of `o` from `JSTObject` to `JSTValue`.

	See also `JSTValueToObject()`.

* Undefined

	Value Macro `JSTObjectUndefined`

	The JavaScript `undefined` value type cast to JavaScript value class `Object`.

* Null

	Value Macro `JSTObjectNull`

	The JavaScript `null` value type cast to JavaScript value class `Object`.

* PropertyHidden

	Value Macro `JSTObjectPropertyHidden`

	The property is not enumerable.

* PropertyReadOnly

	Value Macro `JSTObjectPropertyReadOnly`

	The property is not writable.

* PropertyRequired

	Value Macro `JSTObjectPropertyRequired`

	The property may not be deleted from the object.

7. String (JSTString)
---------------------
Tools for working with JavaScript strings.

* String

	Type Macro `JSTString`

	JST's representation of a JavaScript string reference.

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
