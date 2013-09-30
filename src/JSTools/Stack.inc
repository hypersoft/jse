// iStackMachine: Inline Variant Stack Machine; Copyright 2013 Triston J. Taylor; All Rights Reserved.

#ifndef __STD_UTF_16__
#include <uchar.h>
#endif

#define iStackMachineBool 1
#define iStackMachineChar 2
#define iStackMachineShort 3
#define iStackMachineInt 4
#define iStackMachineLong 5
#define iStackMachineLongLong 6
#define iStackMachineFloat 7
#define iStackMachineDouble 8
#define iStackMachineUTF8 9
#define iStackMachineUTF16 10
#define iStackMachineUTF32 11
#define iStackMachineVoid 12

#define iStackMachine1 sizeof(bool)
#define iStackMachine2 sizeof(char)
#define iStackMachine3 sizeof(short)
#define iStackMachine4 sizeof(int)
#define iStackMachine5 sizeof(long)
#define iStackMachine6 sizeof(long long)
#define iStackMachine7 sizeof(float)
#define iStackMachine8 sizeof(double)
#define iStackMachine9 sizeof(char)
#define iStackMachine10 sizeof(char16_t)
#define iStackMachine11 sizeof(char32_t)
#define iStackMachine12 0L

#define iStackMachineSize(t) iStackMachine##t

#define iStackMachineInit(t, n) \
iStackMachine *n = malloc(sizeof(iStackMachine)); \
memset(n, 0, sizeof(iStackMachine)), \
n->width = iStackMachineSize(t), \
n->type = t, n->growable = 1, \
n->expand = sizeof(void*)

#define iStackMachinePrivate(s) s->private
#define iStackMachineSetPrivate(s, p) (s->private = p)
#define iStackMachineUnits(s) s->units
#define iStackMachineCount(s) s->index
#define iStackMachineLock(s) ((s->growable = 0) != 1)
#define iStackMachineSlots(s) (s->units - s->index)
#define iStackMachineSlot(s, n) (s->address + (n * s->width))
#define iStackMachineBytes(s) (s->units * s->width)
#define iStackMachineMass(s) (sizeof(iStackMachine)+iStackMachineBytes(s))

#define iStackMachineGrow(s, u) (	\
	JSTCND(s->growable,	\
		JSTXPR(true, s->address = realloc(s->address, ((s->units += u) * s->width))),	\
		false	\
	)	\
)

#define iStackMachineShrink(s, u) (	\
	JSTCND(s->units && s->growable,	\
		JSTXPR(true, s->address = realloc(s->address, ((s->units -= u) * s->width))), \
		false	\
	)	\
)

#define iStackMachineCompress(s) iStackMachineShrink(s, iStackMachineSlots(s))

#define iStackMachineFree(s) JSTCND(!s, true, \
	JSTXPR(true, free(s->address), free(s), s = NULL) \
)

#define iStackMachinePush(s, dv) s->swap = dv, JSTCND(iStackMachineSlots(s), \
	JSTXPR(true, iStackMachineWrite(s, s->index++, s->swap)),	\
	JSTCND(iStackMachineGrow(s, s->expand),	\
		JSTXPR(true, iStackMachineWrite(s, s->index++, s->swap)),	\
		false	\
	)	\
)

#define iStackMachinePop(s) ( \
	(s->index) ? \
		iStackMachineRead(s, (s->index -= 1)) \
	:	\
		iStackMachineRead(s, s->index)	\
)

#define iStackMachinePushInt(s, i) iStackMachinePush(s, (long) i)
#define iStackMachinePopInt(s) ((long) iStackMachinePop(s))

#define iStackMachineEnum(s, i, v, a)	\
if (s->index) for (i = 0, v = 0; i < s->index; i++)	\
	if (! (v = iStackMachineRead(s, i), a)) break	\

#define iStackMachineEnumReverse(s, i, v, a)	\
if (s->index) for (i = (s->index - 1), v = 0; i; i--)	\
	 if (! (v = iStackMachineRead(s, i), a)) break	\

#define iStackMachineRefactor(s, t) (	\
	s->index = ((s->width > iStackMachineSize(t))?s->index * s->width * iStackMachineSize(t):(s->index * s->width) / iStackMachineSize(t)),	\
	s->units = (iStackMachineBytes(s)) / (iStackMachineSize(t)),	\
	s->width = iStackMachineSize(t),	\
	s->type = t,	\
	s	\
)

#define iStackMachineSlice(s, d, i, c) ((s && d)?(	\
	d->address = iStackMachineSlot(s, i),	\
	d->type = s->type,	\
	d->width = s->width,	\
	d->index = 0,	\
	d->growable = 0,	\
	d->units = (!c || c > iStackMachineSlots(s) ? iStackMachineSlots(s) : c),	\
	true	\
):false)

typedef struct iStackMachine {
	void * address;		// stack data address
	unsigned type;		// iStackMachineX
	unsigned width;		// iStackMachineSize(iStackMachineX)
	unsigned units;		// stack units
	unsigned index;		// stack top (count)
	unsigned expand;	// unit count of automatic growth
	void * private;		// user data
	double swap;		// hands off: push cache (quoted string / calc. opt.)
	bool growable;		// status: whether or not the machine can grow
} iStackMachine;


#ifdef JSTools_h

extern void iStackMachineWrite(iStackMachine * stack, unsigned index, double value);
extern double iStackMachineRead(iStackMachine * stack, unsigned index);

#else

void iStackMachineWrite(iStackMachine * stack, unsigned index, register double value) {
	register unsigned type = stack->type; register void * address = iStackMachineSlot(stack, index);
	if (type == iStackMachineBool) *(bool*)address = value;
	else if (type == iStackMachineChar) *(char*)address = value;
	else if (type == iStackMachineShort) *(short*)address = value;
	else if (type == iStackMachineInt) *(int*)address = value;
	else if (type == iStackMachineLong) *(long*)address = value;
	else if (type == iStackMachineLongLong) *(long long*)address = value;
	else if (type == iStackMachineFloat) *(float*)address = value;
	else if (type == iStackMachineDouble) *(double*)address = value;
	else if (type == iStackMachineUTF8) *(char*)address = value;
	else if (type == iStackMachineUTF16) *(char16_t*)address = value;
	else if (type == iStackMachineUTF32) *(char32_t*)address = value;
}

double iStackMachineRead(iStackMachine * stack, unsigned index) {
	register unsigned type = stack->type; register void * address = iStackMachineSlot(stack, index);
	if (type == iStackMachineBool) return *(bool*)address;
	else if (type == iStackMachineChar) return *(char*)address;
	else if (type == iStackMachineShort) return *(short*)address;
	else if (type == iStackMachineInt) return *(int*)address;
	else if (type == iStackMachineLong) return *(long*)address;
	else if (type == iStackMachineLongLong) return *(long long*)address;
	else if (type == iStackMachineFloat) return *(float*)address;
	else if (type == iStackMachineDouble) return *(double*)address;
	else if (type == iStackMachineUTF8) return *(char*)address;
	else if (type == iStackMachineUTF16) return *(char16_t*)address;
	else if (type == iStackMachineUTF32) return *(char32_t*)address;
	return 0;
}

#endif
