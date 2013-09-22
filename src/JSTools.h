
#include "JSTools.inc"

extern const void * JSTReservedAddress;

extern JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]);
extern JSTObject JSTNativeInit_ JSTUtility(JSTObject js);
extern char * JSTStringToUTF8_ JSTUtility(JSTString s, bool release);
extern JSTObject JSTConstructorCall_(JSTContext ctx, JSTValue *exception, JSTObject c, ...);
extern JSTValue JSTFunctionCall_(JSTContext ctx, JSTValue *exception, JSTObject method, JSTObject object, ...);
extern JSTObject JSTFunctionCallback_ JSTUtility(char * p, void * f);
extern JSTValue JSTValueFromString_ JSTUtility(JSTString s, bool release);
extern bool JSTObjectHasProperty_ JSTUtility(JSTObject o, char * p);
extern void JSTObjectSetProperty_ JSTUtility(JSTObject o, char *p, JSTValue v, size_t a);
extern JSTValue JSTObjectGetProperty_ JSTUtility(JSTObject o, char * p);
extern bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, char * p);
extern JSTValue JSTScriptEval_ JSTUtility(char *p1, JSTObject o, char * p2, size_t i);
extern bool JSTScriptCheckSyntax_ JSTUtility(char *p1, char *p2, size_t i);
extern JSTValue JSTValueFromJSON_ JSTUtility(char * p);
extern void iStackMachineWrite(iStackMachine * stack, unsigned index, double value);
extern double iStackMachineRead(iStackMachine * stack, unsigned index);

