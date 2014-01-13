void * jst_memory_allocator_class = NULL;

JSTDeclareInitializer(jst_memory_allocator_initialize) {
	puts("initialize");
}

JSTDeclareFinalizer(jst_memory_allocator_finalize) {
	puts("finalized");
}

static JSTDeclareConstructor(jst_memory_allocator_new) {
	if (jst_memory_allocator_class == NULL) jst_memory_allocator_init();
	JSTObject stack = JSTValueProtect(JSObjectMakeArray(ctx, 0, NULL, exception));
	JSTObject this = JSTClassInstance(jst_memory_allocator, stack);
}

void jst_memory_allocator_init() {
	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	jsClass.attributes = JSTClassPropertyManualPrototype,
	jsClass.className = "allocator",
	jsClass.initialize = &jst_memory_allocator_initialize;
	jsClass.finalize = &jst_memory_allocator_finalize;
//	jsClass.callAsFunction = &jst_memory_allocator_addressOf,
//	jsClass.setProperty = &jst_memory_allocator_set,
//	jsClass.getProperty = &jst_memory_allocator_get,
//	jsClass.hasProperty = &jst_memory_allocator_query,
//	jsClass.getPropertyNames = &jst_memory_allocator_enumerate;
	jst_memory_allocator_class = JSClassRetain(JSClassCreate(&jsClass));
}

