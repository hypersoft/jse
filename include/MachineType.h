#ifndef JSE_H
#include "jse.h"
#ifndef MACHINE_TYPE_C
extern double MachineTypeRead(JSContext ctx, void * address, size_t element, unsigned flags, JSValue exception);
extern bool MachineTypeWrite(JSContext ctx, void * address, size_t element, double value, unsigned flags, JSValue exception);
#else
#endif
#endif

