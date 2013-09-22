#define JSTConstructorCall(o, ...) JSTConstructorCall_(ctx, exception, o, ##__VA_ARGS__, JSTReservedAddress)
#define JSTConstructorCallback(c, f) JSTAction(JSObjectMakeConstructor, c, f)


