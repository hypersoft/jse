#define JSTPropertyHidden kJSPropertyAttributeDontEnum
#define JSTPropertyReadOnly kJSPropertyAttributeReadOnly
#define JSTPropertyRequired kJSPropertyAttributeDontDelete
#define JSTPropertyConst (JSTPropertyReadOnly | JSTPropertyRequired)
#define JSTPropertyPrivate (JSTPropertyHidden | JSTPropertyRequired)
#define JSTPropertyProtected (JSTPropertyConst | JSTPropertyPrivate)


