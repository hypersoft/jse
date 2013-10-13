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
extern UTF32 * JSTStringToUTF32(register JSTString jss, bool release);

#else

char * JSTStringToUTF8 (JSTString s, bool release) {
	register size_t bufferLength = 0; register void * buffer = NULL;
	if (s && (bufferLength = JSStringGetMaximumUTF8CStringSize(s)))
		JSStringGetUTF8CString(s, (buffer = malloc(bufferLength)), bufferLength);
	if (release) JSTStringRelease(s);
	return buffer;
}

UTF32 * JSTStringToUTF32(register JSTString jss, bool release) {
	long utf16bytes = JSTStringUTF16Length(jss);
	long utf32bytes = ((utf16bytes) * 2);
	const UTF16 * utf16 = JSTStringUTF16(jss);
	const UTF16 * utf16in = utf16;
	UTF32 * utf32 = malloc(utf32bytes + sizeof(UTF32)); // pad terminate
	UTF32 * utf32out = utf32;
	ConvertUTF16toUTF32(
		&utf16in, utf16in + utf16bytes,
		&utf32out, utf32out + utf32bytes,
		0 // strict conversion
	);
	if (release) JSTStringRelease(jss);
	*(utf32out) = L'\0';
	return utf32;
}

#endif

