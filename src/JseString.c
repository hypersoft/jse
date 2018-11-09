#include "jse.h"
#include <errno.h>
#include <endian.h>
#include <iconv.h>
#if (BYTE_ORDER == LITTLE_ENDIAN)
#define TOCODE "UTF-32LE"
#elif (BYTE_ORDER == BIG_ENDIAN)
#define TOCODE "UTF-32BE"
#endif

char * JSStringToUtf8(JSString string)
{
	unsigned bufferSize = JSStringGetMaximumUTF8CStringSize(string);
	char * s = g_malloc(bufferSize);
	JSStringGetUTF8CString(string, s, bufferSize);
	return s;
}

