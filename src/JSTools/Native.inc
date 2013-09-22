
#define JSTNativeInit(o) JST(JSTNativeInit_, o)

#define JSTNativeAlignment G_MEM_ALIGN
#define JSTNativeByteOrder G_BYTE_ORDER

#define JSTNativeTypeBool 1
#define JSTNativeTypeChar 2
#define JSTNativeTypeShort 3
#define JSTNativeTypeInt 4
#define JSTNativeTypeLong 5
#define JSTNativeTypeLongLong 6
#define JSTNativeTypeFloat 7
#define JSTNativeTypeDouble 8
#define JSTNativeTypeUTF8 9
#define JSTNativeTypeUTF16 10
#define JSTNativeTypeUTF32 11

#define JSTNativeTypeVoid 12
#define JSTNativeTypeStructure 13

#define JSTNativeTypeSize iStackMachineSize
#define JSTNativeTypePadding iStackMachinePadding

