#ifdef JSTools_h

extern JSTObject JSTConstructorCall_(register JSTContext ctx, JSTValue *exception, JSTObject c, ...);

#else

JSTObject JSTConstructorCall_(register JSTContext ctx, JSTValue *exception, JSTObject c, ...) {
	va_list ap; register size_t argc = 0, count = 0; va_start(ap, c); 
	while(va_arg(ap, void*) != JSTReservedAddress) argc++; va_start(ap, c);
	if (argc > 32) return JSTObjectUndefined; JSTValue argv[argc+1];
	while (count < argc) argv[count++] = va_arg(ap, JSTValue);
	argv[count] = NULL; return JST(JSObjectCallAsConstructor, c, argc, argv);
}

#endif

#define JSTConstructorCall(o, ...) JSTConstructorCall_(ctx, exception, o, ##__VA_ARGS__, JSTReservedAddress)
#define JSTConstructorCallback(c, f) JSTAction(JSObjectMakeConstructor, c, f)


