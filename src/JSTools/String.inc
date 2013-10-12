#define JSTString JSStringRef

#define JSTStringRelease(s) JSStringRelease(s)

#ifndef JSTools_h

extern char * JSTStringToUTF8_ JSTUtility(JSTString s, bool release);

#else

char * JSTStringToUTF8_ JSTUtility(JSTString s, bool release) {
	register size_t bufferLength = 0; register void * buffer = NULL;
	if (s && (bufferLength = JSStringGetMaximumUTF8CStringSize(s)))
		JSStringGetUTF8CString(s, (buffer = malloc(bufferLength)), bufferLength);
	if (release) JSTStringRelease(s);
	return buffer;
}

#endif

#define JSTStringFromUTF8(p) JSStringCreateWithUTF8CString(p)
#define JSTStringToUTF8(s, r) JST(JSTStringToUTF8_, s, r)
#define JSTStringFreeUTF8(p) free(p)
#define JSTStringFromUTF16(b, c) JSStringCreateWithCharacters(b, c)
#define JSTStringUTF16(s) JSStringGetCharactersPtr(s)
#define JSTStringUTF16Length(s) JSStringGetLength(s)
#define JSTStringRetain(s) JSStringRetain(s)
#define JSTStringCompare(s1, s2) JSStringIsEqual(s1, s2)
#define JSTStringCompareToUTF8(s, p) JSStringIsEqualToUTF8CString(s, p)
#define JSTStringToValue(s, r) JSTValueFromString(s, r)
#define JSTStringFromValue(v) JSTValueToString(v)


