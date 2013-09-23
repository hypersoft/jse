#define JSTValue JSValueRef

#define JSTValueGetType(v) JSValueGetType, v)

#define JSTValueTypeUndefined kJSTypeUndefined
#define JSTValueTypeNull kJSTypeNull
#define JSTValueTypeBoolean kJSTypeBoolean
#define JSTValueTypeNumber kJSTypeNumber
#define JSTValueTypeString kJSTypeString
#define JSTValueTypeObject kJSTypeObject

#define JSTValueIsUndefined(v) JSTAction(JSValueIsUndefined, v)
#define JSTValueIsNull(v) JSTAction(JSValueIsNull, v)
#define JSTValueIsBoolean(v) JSTAction(JSValueIsBoolean, v)
#define JSTValueIsNumber(v) JSTAction(JSValueIsNumber, v)
#define JSTValueIsString(v) JSTAction(JSValueIsString, v)
#define JSTValueIsObject(v) JSTAction(JSValueIsObject, v)
#define JSTValueIsClassInstance(v, c) JST(JSValueIsObjectOfClass, v, c)
#define JSTValueIsEqual(v1, v2) JST(JSValueIsEqual, v1, v2)
#define JSTValueIsStrictEqual(v1, v2) JSTAction(JSValueIsStrictEqual, v1, v2)
#define JSTValueIsConstructorInstance(v, c) JST(JSValueIsInstanceOfConstructor, v, c)

#define JSTValueUndefined JSValueMakeUndefined(ctx)
#define JSTValueNull JSValueMakeNull(ctx)

#define JSTValueFromBoolean(b) JSTAction(JSValueMakeBoolean, b)
#define JSTValueFromDouble(d) JSTAction(JSValueMakeNumber, d)
#define JSTValueFromString(s, r) JST(JSTValueFromString_, s, r)
#define JSTValueFromUTF8(p) JSTValueFromString(JSTStringFromUTF8(p), true)
#define JSTValueFromJSON(p) JST(JSTValueFromJSON, p)

#define JSTValueToJSON(v, indent) JSTStringToUTF8(JST(JSValueCreateJSONString, v, indent), true)
#define JSTValueToBoolean(v) JST(JSValueToBoolean, v)
#define JSTValueToDouble(v) JST(JSValueToNumber, v)
#define JSTValueToString(v) JST(JSValueToStringCopy, v)
#define JSTValueToObject(v) JST(JSValueToObject, v)

#define JSTValueProtect(v) JSTAction(JSValueProtect, v)
#define JSTValueUnprotect(v) JSTAction(JSValueUnprotect, v)

#define JSTValueIsVoid(v) (!v || JSTValueIsNull(v) || JSTValueIsUndefined(v))
#define JSTValueIsFunction(v) (v && JSTValueIsObject(v) && JSTObjectIsFunction(v))
#define JSTValueIsConstructor(v) (v && JSTValueIsObject(v) && JSTObjectIsConstructor(v))

#define JSTValueToPointer(v) ((void*)((long)JSTValueToDouble(v)))
#define JSTValueFromPointer(p) ((JSTValue) JSTValueFromDouble(((long) p)))


