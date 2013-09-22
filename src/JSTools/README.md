JST
===
Copyright (C) 2013 Triston J. Taylor (Hypersoft Systems); All Rights Reserved.

1. Base (JST)
-------------
Miscellaneous Assistance Macros.

	Items in this category don't carry the section heading name 'Base'.

* `JST(procedure name, ...)`

	Call upon a context exception function frame with a varying number of intermittent arguments and return value types.

* `JSTObject JSTInit(JSTObject global, int argc, char *argv[], char *envp[])`

	Instruct JST to initialize all static data, and runtime operability.

* `JSTAction(...)`

	Call upon a JST context function frame.

* `JSTUtility(...)`

	Opposite of `JST()`, this macro declares a context exception function frame. By design, these are JST interoperability  entry points.

2. Context
----------

* Group
  - Create()
  - Retain()
  - Release()

* Create()

* CreateInGroup()

* Retain()

* Release()

* GetGlobalObject()

* GetGroup()

3. Class
--------

* Accessor

* Function

* Definition

* EmptyDefinition

* Create()

* Retain()

* Release()

* Instance()

* Property
  - None
  - ManualPrototype

4. Declare
----------

* Initializer()

* Finalizer()

* HasProperty()

* GetProperty()

* SetProperty()

* DeleteProperty()

* GetPropertyNames()

* Function()

* Constructor()

* HasInstance()

* Convertor()

5. Type
-------

* Undefined

* Null

* Boolean

* Number

* String

* Object

6. Value
--------

* GetType()

* IsUndefined()

* IsNull()

* IsBoolean()

* IsNumber()

* IsString()

* IsObject()

* IsClassInstance()

* IsEqual()

* IsStrictEqual()

* IsConstructorInstance()

* Undefined

* Null

* FromBoolean()

* FromDouble()

* FromString()

* FromUTF8()

* FromJSONString()

* ToJSONString()

* ToBoolean()

* ToDouble()

* ToString()

* ToObject()

* Protect()

* Unprotect()

* IsVoid()

* IsFunction()

* IsConstructor()

* ToPointer()

* FromPointer()

7. Property (JSTProperty)
-------------------------
Attributes for JSTObjectSetProperty

* JSTPropertyHidden

	The property is not enumerable.

* JSTPropertyReadOnly

	The property can be read, but may not be written to.
* JSTPropertyRequired

	The property may not be deleted from the object.

* JSTPropertyConst

	The property should be treated as a constant value.

* JSTPropertyPrivate

	The property is not typically consumed by external function.

* JSTPropertyProtected

	The property must exist as long as the object exists.

8. Object
---------

* GetPrototype()

* SetPrototype()

* HasProperty()

* GetProperty()

* SetProperty()

* DeleteProperty()

* GetPropertyAtIndex()

* SetPropertyAtIndex()

* GetPrivate()

* SetPrivate()

* IsFunction()

* IsConstructor()

* ToValue()

* Undefined

* Null

9. String
---------

* FromUTF8

* ToUTF8()

* FreeUTF8

* FromUTF16()

* UTF16()

* UTF16Length()

* Retain()

* Release()

* Compare()

* CompareToUTF8()

* ToValue()

* FromValue()

10. Function
------------

* Call()

* Callback()

11. Constructor
---------------

* Call()

* Callback()

12. Script
----------

* Eval()

* CheckSyntax()

* NativeEval()

* Error()

* EvalError()

* RangeError()

* ReferenceError()

* SyntaxError()

* TypeError()

* URIError()

* SetError()

* HasError

* Function()

13. Native
----------

* Init()

* Alignment

* ByteOrder

* Type
  - Bool
  - Char
  - Short
  - Int
  - Long
  - LongLong
  - Float
  - Double
  - UTF8
  - UTF16
  - UTF32
  - Void
  - Size()
  - Padding()
