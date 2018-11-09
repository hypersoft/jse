#include "jse.h"

JSValue JSValueFromUtf8(JSContext ctx, char * string)
{
	JSString jsr = JSStringCreateWithUTF8CString(string);
	JSValue v = JSValueMakeString(ctx, jsr);
	JSStringRelease(jsr);
	return v;
}

char * JSValueToUtf8(JSContext c, JSValue v)
{
	JSString tmp = JSValueToStringCopy(c, v, NULL);
	char * utf8 = JSStringToUtf8(tmp);
	JSStringRelease(tmp);
	return utf8;
}

JSString JSValueToString(JSContext c, JSValue v, JSValue * e)
{
	return JSValueToStringCopy(c, v, e);
}
