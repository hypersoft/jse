#define JSTFunctionCall(f, o, ...) JSTFunctionCall_(ctx, exception, f, o, ##__VA_ARGS__, JSTReservedAddress)
#define JSTFunctionCallback(p, f) JST(JSTFunctionCallback_, p, f)


