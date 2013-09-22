
#define JSTClassAccessor JSStaticValue
#define JSTClassFunction JSStaticFunction
#define JSTClassDefinition JSClassDefinition
#define JSTClassEmptyDefinition kJSClassDefinitionEmpty

#define JSTClassCreate(d) JSClassCreate(d)
#define JSTClassRetain(c) JSClassRetain(c)
#define JSTClassRelease(c) JSClassRelease(c)

#define JSTClassInstance(c, p) JSTAction(JSObjectMake, c, p)

#define JSTClassPropertyNone kJSClassAttributeNone
#define JSTClassPropertyManuaPrototype kJSClassAttributeNoAutomaticPrototype


