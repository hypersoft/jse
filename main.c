#include "JSTools.h"


int main(int argc, char *argv[], char *envp[]) {

	JSTContext ctx = JSTContextCreateInGroup(JSTContextGroupCreate(), NULL);
	JSTObject global = JSTContextGetGlobalObject(ctx);
	JSTValue e = NULL; JSTValue *exception = &e;

	JSTInit(global, argc, argv, envp);



	iStackMachineInit(iStackMachineDouble, v); // crack a snive smile
	iStackMachineGrow(v, 16); // Generous pre buffering
	printf("pushing a sentinel null pointer.. (%s)\n\n", (iStackMachinePushInt(v, NULL))?"true":"false");
	iStackMachinePushInt(v, "a"); // push some static data

	iStackMachineInit(iStackMachineDouble, v2); // crack a snive smile

	iStackMachineSlice(v, v2, v->index, 1);

	iStackMachinePushInt(v, "b"); // -/>
	// demo a live report
	printf("stack count: %i\nstack units: %ix%i (%i bytes variant storage)\nstack total: %i (bytes header+storge)\n\n", iStackMachineCount(v), iStackMachineUnits(v), v->width, iStackMachineBytes(v), iStackMachineMass(v));

	printf("stack count: %i\nstack units: %ix%i (%i bytes variant storage)\nstack total: %i (bytes header+storge)\n\n", iStackMachineCount(v2), iStackMachineUnits(v2), v2->width, iStackMachineBytes(v2), iStackMachineMass(v2));
	printf("%s\n", iStackMachinePopInt(v2));

exit(0);

	iStackMachineRefactor(v, iStackMachineChar);
	// demo a live report
	printf("stack count: %i\nstack units: %ix%i (%i bytes variant storage)\nstack total: %i (bytes header+storge)\n\n", iStackMachineCount(v), iStackMachineUnits(v), v->width, iStackMachineBytes(v), iStackMachineMass(v));

	iStackMachineRefactor(v, iStackMachineDouble);
	// demo a live report
	printf("stack count: %i\nstack units: %ix%i (%i bytes variant storage)\nstack total: %i (bytes header+storge)\n\n", iStackMachineCount(v), iStackMachineUnits(v), v->width, iStackMachineBytes(v), iStackMachineMass(v));

exit(0);

	printf("pushing a sentinel null pointer.. (%s)\n\n", (iStackMachinePushInt(v, NULL))?"true":"false");
	// demo compression
	iStackMachineCompress(v);
	// demo a live report
	printf("(post compression stats)\nstack count: %i\nstack units: %ix%i (%i bytes variant storage)\nstack total: %i (bytes header+storge)\n\n", iStackMachineCount(v), iStackMachineUnits(v), v->width, iStackMachineBytes(v), iStackMachineMass(v));
	// enumerate the stack from bottom to top
	long p; unsigned c; iStackMachineEnum(v, c, p, printf("%s\n", p));
	printf("\nenum operation complete\n\n");
	// pop everything off of the stack
	while (iStackMachineCount(v)) printf("%s\n", iStackMachinePopInt(v)); // pop static heads
	printf("\npop operation complete\n"); // done deal

	
return 0;

}


