#define JSTUTF8 char
#define JSTUTF16 char16_t
#define JSTUTF32 char32_t
#define JSTString JSStringRef

#define JSTContext JSContextRef
#define JSTValue JSValueRef
#define JSTObject JSObjectRef
#define JSTObjectUndefined ((JSTObject) JSTValueUndefined)
#define JSTValueUndefined JSValueMakeUndefined(ctx)

#define JST(NAME, ...) NAME (ctx, ##__VA_ARGS__, exception)
#define JSTAction(NAME, ...) NAME (ctx, ##__VA_ARGS__)

#define JSTUtility(...) (register JSTContext ctx, ##__VA_ARGS__, JSTValue *exception)
#define JSTChoose(condition, primary, secondary) ((condition)?(primary):(secondary))

#define JSTInit(o, c, a, e) JST(JSTInit_, o, c, a, e)

#define JSTXPR(r, ...) (__VA_ARGS__, r)
#define JSTCND(e, t, f) ((e) ? (t) : (f))


