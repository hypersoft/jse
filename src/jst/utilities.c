

/* strict argument to integer conversion */
bool JSTArgumentToInt_ JSTUtility(int argc, JSTValue argv[], int bits, int index, void * dest) {

	/*
	   we don't worry about sign, just that the data will fit into the destination !
	   if this function returns false, an error has been set, clean up and return 
	*/

	JSTValue value;
	if (bits != 8 && bits != 16 && bits != 32 && bits != 64) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert argument %i to %i-bit integer: %i-bit integer is not a supported type",
		index, bits, bits
	); else	if (index >= argc) return JSTScriptNativeError(JST_TYPE_ERROR,
		"cannot convert argument %i to %i-bit integer: argument is undefined", index, bits
	); else if (dest == NULL) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"cannot convert argument %i to %i-bit integer: destination is NULL", index, bits
	);

	// first we convert to JavaScript integer which should be an int64
	value = JSTValueParseInt(argv[index]);
	// then we check for NaN
	if (JSTValueIsNaN(value)) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert argument %i to integer: value is not a number", index
	);

	// now we convert to native double
	double integer = JSTValueToDouble(value);

	if (bits == 8) { // char first, most likely repetitive conversion
		if (integer < G_MININT8) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is less than %i",
			index, 8, integer, G_MININT8
		); else if (integer > G_MAXINT8)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is greater than %i",
			index, 8, integer, G_MAXINT8
		);
		*(gint8*) dest = (gint8) integer;
	} else if (bits == 32) { // then long, most likely conversion
		if (integer < G_MININT32) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is less than %i",
			index, 32, integer, G_MININT32
		); else if (integer > G_MAXINT32)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is greater than %i",
			index, 32, integer, G_MAXINT32
		);
		*(gint32*) dest = (gint32) integer;
	} else if (bits == 16) {  // then short, least likely conversion
		if (integer < G_MININT16) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is less than %i",
			index, 16, integer, G_MININT16
		); else if (integer > G_MAXINT16)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert argument %i to %i-bit integer: %.0g is greater than %i",
			index, 16, integer, G_MAXINT16
		);
		*(gint16*) dest = (gint16) integer;
	} else if (bits == 64) { // then this, which is the "fastest conversion"
		// NOTE: long long has more integer bits than double
		// double can only hold 52 bits of integer (2^53)!
		*(gint64*) dest = (gint64) integer;
	}
	return true;
}

/* strict value to integer conversion */
bool JSTValueToInt_ JSTUtility(int bits, JSTValue input, void * dest) {

	/*
	   we don't worry about sign, just that the data will fit into the destination !
	   if this function returns false, an error has been set, clean up and return 
	*/

	JSTValue value;
	if (bits != 8 && bits != 16 && bits != 32 && bits != 64) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert value to %i-bit integer: %i-bit integer is not a supported type",
		bits, bits
	); else if (dest == NULL) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"cannot convert value to %i-bit integer: destination is NULL", bits
	);

	// first we convert to JavaScript integer which should be an int64
	value = JSTValueParseInt(input);
	// then we check for NaN
	if (JSTValueIsNaN(value)) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert value to %i-bit integer: input is not a number", bits
	);

	// now we convert to native double
	double integer = JSTValueToDouble(value);

	if (bits == 8) { // char first, most likely repetitive conversion
		if (integer < G_MININT8) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is less than %i",
			8, integer, G_MININT8
		); else if (integer > G_MAXINT8)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is greater than %i",
			8, integer, G_MAXINT8
		);
		*(gint8*) dest = (gint8) integer;
	} else if (bits == 32) { // then long, most likely conversion
		if (integer < G_MININT32) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is less than %i",
			32, integer, G_MININT32
		); else if (integer > G_MAXINT32)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is greater than %i",
			32, integer, G_MAXINT32
		);
		*(gint32*) dest = (gint32) integer;
	} else if (bits == 16) { // then short, least likely conversion
		if (integer < G_MININT16) return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is less than %i",
			16, integer, G_MININT16
		); else if (integer > G_MAXINT16)  return JSTScriptNativeError(JST_RANGE_ERROR,
			"cannot convert value to %i-bit integer: %.0g is greater than %i",
			16, integer, G_MAXINT16
		);
		*(gint16*) dest = (gint32) integer;
	} else if (bits == 64) { // then long long, which is the fastest conversion
		// NOTE: long long has more integer bits than double
		// double can only hold 52 bits of integer (2^53)!
		*(gint64*) dest = (gint64) integer;
	}
	return true;
}

/* strict argument to pointer conversion */
bool JSTArgumentToPointer_ JSTUtility(int argc, JSTValue argv[], int index, void * dest) {

	/* if this function returns false, an error has been set, clean up and return */

	JSTValue value;
	if (index >= argc) return JSTScriptNativeError(JST_TYPE_ERROR,
		"cannot convert argument %i to pointer: argument is undefined", index
	); else if (dest == NULL) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"cannot convert argument %i to pointer: destination is NULL", index
	);

	// first we convert to JavaScript integer which should be an int64
	value = JSTValueParseInt(argv[index]);
	// then we check for NaN
	if (JSTValueIsNaN(value)) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert argument %i to pointer: value is not a number", index
	);

	// now we convert to native double
	double integer = JSTValueToDouble(value);

	if (integer < 0) return JSTScriptNativeError(JST_RANGE_ERROR,
		"cannot convert argument %i to pointer: %.0g is less than 0",
		index, integer
	); else if (integer > G_MAXUINT)  return JSTScriptNativeError(JST_RANGE_ERROR,
		"cannot convert argument %i to pointer: %.0g is greater than %u",
		index, integer, G_MAXUINT
	);
	*(guintptr*) dest = (guintptr) integer;

	return true;
}

/* strict value to pointer */
bool JSTValueToPointer_ JSTUtility(JSTValue input, void * dest) {

	/* if this function returns false, an error has been set, clean up and return */

	JSTValue value;
	if (dest == NULL) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"cannot convert value to pointer: destination is NULL"
	);

	// first we convert to JavaScript integer which should be an int64
	value = JSTValueParseInt(input);
	// then we check for NaN
	if (JSTValueIsNaN(value)) return JSTScriptNativeError(
		JST_TYPE_ERROR,
		"cannot convert value to pointer: input is not a number"
	);

	// now we convert to native double
	double integer = JSTValueToDouble(value);

	if (integer < 0) return JSTScriptNativeError(JST_RANGE_ERROR,
		"cannot convert value to pointer: %.0g is less than 0", integer
	); else if (integer > G_MAXUINT)  return JSTScriptNativeError(JST_RANGE_ERROR,
		"cannot convert value to pointer: %.0g is greater than %u",
		integer, G_MAXUINT
	);
	*(guintptr*) dest = (guintptr) integer;

	return true;
}

JSTObject JSTConstructorCall_(register JSTContext ctx, JSTValue * exception, JSTObject c, ...) {
	va_list ap; register gsize argc = 0, count = 0; va_start(ap, c); 
	while(va_arg(ap, void*) != JSTReservedAddress) argc++; va_start(ap, c);
	if (argc > 32) return JSTObjectUndefined; JSTValue argv[argc+1];
	while (count < argc) argv[count++] = va_arg(ap, JSTValue);
	argv[count] = NULL; return JSObjectCallAsConstructor(ctx, c, argc, argv, exception);
}

JSTObject JSTFunctionCallback_ JSTUtility(const utf8 * p, void * f) {
	JSTObject result = NULL;
	if (p && f) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSObjectMakeFunctionWithCallback, s, f);
		JSTStringRelease(s);
	}
	return result;
}

JSTValue JSTFunctionCall_(register JSTContext ctx, JSTValue * exception, JSTObject method, JSTObject object, ...) {
	va_list ap; register gsize argc = 0, count = 0; va_start(ap, object); 
	while(va_arg(ap, void*) != JSTReservedAddress) argc++; va_start(ap, object);
	if (argc > 32) return JSTValueUndefined; JSTValue argv[argc+1];
	while (count < argc) argv[count++] = va_arg(ap, JSTValue);
	argv[count] = NULL; return JSObjectCallAsFunction(ctx, method, object, argc, argv, exception);
}

JSTObject JSTObjectSetMethod_ JSTUtility(JSTObject o, const utf8 * n, void * m, int a) {
	JSTObject method = NULL;
	if (JSTValueIsObject(o)) {
		JSTString name = (n)?JSTStringFromUTF8(n):NULL; 
		if (m) JSObjectSetProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), name, (method = JSObjectMakeFunctionWithCallback(ctx, name, m)), a, exception); 
		JSTStringRelease(name); 
	}
	return method;
}

JSTObject JSTObjectSetConstructor_ JSTUtility(JSTObject o, const utf8 * n, JSTClass c, void * m, gsize a) {
	JSTObject constructor = NULL;
	if (JSTValueIsObject(o)) {
		JSTString name = (n)?JSTStringFromUTF8(n):NULL;
		JSObjectSetProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), name, (constructor = JSObjectMakeConstructor(ctx, c, m)), a, exception);
		JSTStringRelease(name);
	}
	return constructor;
}

bool JSTObjectDeleteProperty_ JSTUtility(JSTObject o, const utf8 * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSObjectDeleteProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), s, exception);
		JSTStringRelease(s);
	}
	return result;
}

void * JSTObjectGetProperty_ JSTUtility(JSTObject o, const utf8 * p) {
	void * result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = (void*) JSObjectGetProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), s, exception);
		JSTStringRelease(s);
	}
	return result;
}

// Returns the value set as a void *
void * JSTObjectSetProperty_ JSTUtility(JSTObject o, const utf8 * p, JSTValue v, gsize a) {
	if (p && v) {
		JSTString s = JSTStringFromUTF8(p);
		JSObjectSetProperty(ctx, (o)?o:JSContextGetGlobalObject(ctx), s, v, a, exception);
		JSTStringRelease(s);
	}
	return (void*)v;
}

bool JSTObjectHasProperty_ JSTUtility(JSTObject o, const utf8 * p) {
	bool result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSTAction(JSObjectHasProperty, (o)?o:JSContextGetGlobalObject(ctx), s);
		JSTStringRelease(s);
	}
	return result;
}

bool JSTScriptCheckSyntax_ JSTUtility(const utf8 * p1, const utf8 * p2, gsize i) {
	if (!p1 || !p2) return true;
	JSTString s[2] = {JSTStringFromUTF8(p1), JSTStringFromUTF8(p2)};
	bool result = JSCheckScriptSyntax(ctx, s[0], s[1], i, exception);
	JSTStringRelease(s[0]); JSTStringRelease(s[1]);
}

JSTValue JSTScriptEval_ JSTUtility(const utf8 * p1, JSTObject o, const utf8 * p2, gsize i) {
	if (!p1 || !p2) return NULL;
	JSTString s[2] = {JSTStringFromUTF8(p1), JSTStringFromUTF8(p2)};
	JSTValue result = JSEvaluateScript(ctx, s[0], (o)?o:JSContextGetGlobalObject(ctx), s[1], i, exception);
	JSTStringRelease(s[0]); JSTStringRelease(s[1]);
	return result;
}

void * JSTScriptNativeError_(register JSTContext ctx, register JSTValue * exception, const utf8 * file, gsize line, gsize errorType, const utf8 * format, ...) {
	va_list ap; va_start(ap, format); utf8 * message = NULL;
	g_vasprintf(&message, format, ap);
	if (errorType == 2) JSTScriptEval("throw(this)", JSTScriptError(message), file, line);
	else if (errorType == 3) JSTScriptEval("throw(this)", JSTScriptSyntaxError(message), file, line);
	else if (errorType == 4) JSTScriptEval("throw(this)", JSTScriptTypeError(message), file, line);
	else if (errorType == 5) JSTScriptEval("throw(this)", JSTScriptRangeError(message), file, line);
	else if (errorType == 6) JSTScriptEval("throw(this)", JSTScriptReferenceError(message), file, line);
	else if (errorType == 7) JSTScriptEval("throw(this)", JSTScriptEvalError(message), file, line);
	else if (errorType == 8) JSTScriptEval("throw(this)", JSTScriptURIError(message), file, line);
	else JSTScriptEval("throw(this)", JSTScriptError(message), file, line);
	g_free(message);
	return NULL;
}

int JSTScriptReportException_(register JSTContext ctx, register JSTValue * exception) {

	JSTObject e = ((JSTObject) * exception);
	*exception = NULL;

	e = (JSTObject) JSTScriptNativeEval("Exception(this)", e);
	gsize code = JSTValueToDouble(JSTObjectGetProperty(e, "code"));
	utf8 * name = JSTValueToUTF8(JSTObjectGetProperty(e, "name"));
	
	if (JSTObjectHasProperty(e, "head")) {
		utf8 * head = JSTValueToUTF8(JSTObjectGetProperty(e, "head"));
		g_fprintf(stderr, "Script Exception: %s\n%s", head, name);
		g_free(head);
	} else	{
		utf8 * url = JSTValueToUTF8(JSTObjectGetProperty(e, "sourceURL"));
		gsize line = JSTValueToDouble(JSTObjectGetProperty(e, "line"));
		g_fprintf(
			stderr, "Runtime Exception: %s: line %u\n%s",
			url, line, name
		);
		g_free(url);
	}
	
	g_free(name);
	
	utf8 * message = JSTValueToUTF8(JSTObjectGetProperty(e, "message"));
	if (message && *message) g_fprintf(stderr, ": %s", message);
	
	g_fprintf(stderr, "\n");
	
	JSTValue stackString = JSTScriptNativeEval("Exception.stackString(this)", e);
	if (! JSTValueIsVoid(stackString)) {
		utf8 * stack = JSTValueToUTF8(stackString);
		g_fprintf(stderr, "\n%s\n", stack);
		g_free(stack);
	}
	
	g_free(message);

	return code;

}

JSTValue JSTValueFromString_ JSTUtility(JSTString s, bool release) {
	if (!s) return NULL;
	JSTValue result = JSValueMakeString(ctx, s);
	if (release) JSTStringRelease(s);
	return result;
}

JSTValue JSTValueFromJSON_ JSTUtility(const utf8 * p) {
	JSTValue result = NULL;
	if (p) {
		JSTString s = JSTStringFromUTF8(p);
		result = JSValueMakeFromJSONString(ctx, s);
		JSTStringRelease(s);
	}
	return result;
}
