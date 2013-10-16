#define JSTString JSStringRef

#define JSTStringRelease(s) JSStringRelease(s)
#define JSTStringFromUTF8(p) JSStringCreateWithUTF8CString(p)
#define JSTStringFreeUTF8(p) free(p)
#define JSTStringFreeUTF32(p) free(p)
#define JSTStringFromUTF16(b, c) JSStringCreateWithCharacters(b, c)
#define JSTStringUTF16(s) JSStringGetCharactersPtr(s)
#define JSTStringUTF16Length(s) JSStringGetLength(s)
#define JSTStringRetain(s) JSStringRetain(s)
#define JSTStringCompare(s1, s2) JSStringIsEqual(s1, s2)
#define JSTStringCompareToUTF8(s, p) JSStringIsEqualToUTF8CString(s, p)
#define JSTStringToValue(s, r) JSTValueFromString(s, r)
#define JSTStringFromValue(v) JSTValueToString(v)


#ifndef JSTools_h

extern char * JSTStringToUTF8 (JSTString s, bool release);
extern UTF32 * JSTStringToUTF32(register JSTString jss, size_t len, bool release);

#else

char * JSTStringToUTF8 (JSTString s, bool release) {
	register size_t bufferLength = 0; register void * buffer = NULL;
	if (s && (bufferLength = JSStringGetMaximumUTF8CStringSize(s)))
		JSStringGetUTF8CString(s, (buffer = malloc(bufferLength)), bufferLength);
	if (release) JSTStringRelease(s);
	return buffer;
}

UTF32 * JSTStringToUTF32(register JSTString jss, size_t len, bool release) {
	const gunichar2 * utf16 = JSTStringUTF16(jss);
	void * result = g_utf16_to_ucs4(utf16, len, NULL, NULL, NULL);
	if (release) JSTStringRelease(jss);
	return result;
}

#endif

