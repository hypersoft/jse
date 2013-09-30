
#define JSTDeclareInitializer(n) void n (JSTContext ctx, JSTObject object)
#define JSTDeclareFinalizer(n) void n (JSTObject object)
#define JSTDeclareHasProperty(n) bool n (JSTContext ctx, JSTObject object, JSTString propertyName)
#define JSTDeclareGetProperty(n) JSTValue n (JSTContext ctx, JSTObject object, JSTString propertyName, JSTValue* exception)
#define JSTDeclareSetProperty(n) bool n (JSTContext ctx, JSTObject object, JSTString propertyName, JSTValue value, JSTValue* exception)
#define JSTDeclareDeleteProperty(n) bool n (JSTContext ctx, JSTObject object, JSTString propertyName, JSTValue* exception)
#define JSTDeclareGetPropertyNames(n) void n (JSTContext ctx, JSTObject object, JSPropertyNameAccumulatorRef propertyNames)
#define JSTDeclareFunction(...) (JSTContext ctx, JSObjectRef function, JSObjectRef this, size_t argc, const JSTValue argv[], JSTValue* exception)
#define JSTDeclareConstructor(n) JSTObject n (JSTContext ctx, JSTObject constructor, size_t argumentCount, const JSTValue arguments[], JSTValue* exception)
#define JSTDeclareHasInstance(n) bool n (JSTContext ctx, JSTObject constructor, JSTValue possibleInstance, JSTValue* exception)
#define JSTDeclareConvertor(n) JSTValue n (JSTContext ctx, JSTObject object, JSType type, JSTValue* exception)

