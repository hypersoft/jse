#define JSTUTF8 char
#define JSTUTF16 char16_t
#define JSTUTF32 char32_t

#define JSTPropertyHidden kJSPropertyAttributeDontEnum
#define JSTPropertyReadOnly kJSPropertyAttributeReadOnly
#define JSTPropertyRequired kJSPropertyAttributeDontDelete
#define JSTPropertyConst (JSTPropertyReadOnly | JSTPropertyRequired)
#define JSTPropertyPrivate (JSTPropertyHidden | JSTPropertyRequired)
#define JSTPropertyProtected (JSTPropertyConst | JSTPropertyPrivate)

#define JSTTypeUndefined kJSTypeUndefined
#define JSTTypeNull kJSTypeNull
#define JSTTypeBoolean kJSTypeBoolean
#define JSTTypeNumber kJSTypeNumber
#define JSTTypeString kJSTypeString
#define JSTTypeObject kJSTypeObject

#define JST(NAME, ...) NAME (ctx, ##__VA_ARGS__, exception)
#define JSTAction(NAME, ...) NAME (ctx, ##__VA_ARGS__)

#define JSTUtility(...) (JSTContext ctx, ##__VA_ARGS__, JSTValue *exception)
#define JSTChoose(condition, primary, secondary) ((condition)?(primary):(secondary))

#define JSTInit(o, c, a, e) JST(JSTInit_, o, c, a, e)

#define JSTXPR(r, ...) (__VA_ARGS__, r)
#define JSTCND(e, t, f) ((e) ? (t) : (f))


