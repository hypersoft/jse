#define JSToolsSource

#include "JSTools.inc"
#include "JSTools.h"

#include <wait.h>

JSTGlobalRuntime JSTRuntime;

static JSValueRef jst_chdir JSToolsFunction () {
	char * val; JSValueRef
	result = JSTMakeNumber(g_chdir(JSTGetValueBuffer(JSTParam(1), &val)));
	JSTFreeBuffer(val); return result;
}

void _JSTReportException JSToolsProcedure (JSValueRef e) {

	JSTEval("if (\"sourceURL\" in this) writeError(this.sourceURL + \": \");", e);
	JSTEval("if (\"line\" in this) writeError(\"line \" + this.line + \": \");", e);
	JSTEval("if (\"name\" in this) writeError(this.name + \": \");", e);
	JSTEval("writeError(this.message + \"\\n\");", e);

}

void _JSTReportFatalException JSToolsProcedure(int code, JSValueRef e) {
	JSToolsCall(_JSTReportException, e); exit(code);
}

static JSValueRef jst_exit JSToolsFunction () {
	exit(JSTInteger(JSTParam(1)));
	return RtJSValue(undefined);
}

static JSValueRef jst_shellExecute JSToolsFunction () {

	int child_status = 0, i = 0;
	gchar *exec_child_out = NULL, *exec_child_err = NULL; gint exec_child_status = 0;

	JSObjectRef exec = RtJSObject(undefined);

	bool captureOut = JSTBoolean(JSTGetProperty(this, "captureOutput"));
	bool captureError = JSTBoolean(JSTGetProperty(this, "captureError"));
	bool returnStatus = JSTBoolean(JSTGetProperty(this, "returnStatus"));

	char * nargv[argc + 1];

	while (i < argc) {
		nargv[i] = JSTGetValueBuffer(argv[i], NULL);
		i++;
	}

	nargv[i] = NULL;

	if (true) {
		if (g_spawn_sync(NULL, nargv, NULL, G_SPAWN_LEAVE_DESCRIPTORS_OPEN | G_SPAWN_SEARCH_PATH | G_SPAWN_CHILD_INHERITS_STDIN, NULL, NULL, (captureOut) ? &exec_child_out : NULL, (captureError) ? &exec_child_err : NULL, &exec_child_status, NULL)) {
			exec_child_status = WEXITSTATUS(exec_child_status);
			bool success = (exec_child_status == 0);
			exec = JSTCreateClassObject(NULL, NULL);
			JSTSetPrototype(exec, JSTCreateClassObject(NULL, NULL));
			JSObjectRef proto = JSTGetPrototypeObject(exec);
			if ( returnStatus ) {
				JSTSetProperty(exec, "status", JSTMakeNumber(exec_child_status), 0);				
				JSTSetPropertyScript(proto, "valueOf", "return this.status");
			} else {
				JSTSetPropertyScript(proto, "valueOf", (success) ? "return true" : "return false");
			}
			if (captureOut && exec_child_out) {
				JSTSetPropertyScript(proto, "toString", "return this.stdout");
				JSTSetProperty(exec, "stdout", JSTMakeBufferValue(exec_child_out), 0);
				JSTFreeBuffer(exec_child_out);
			}
			if (captureError && exec_child_err) {
				if (! success && ! captureOut)
					JSTSetPropertyScript(proto, "toString", "return this.stderr");
				JSTSetProperty(exec, "stderr", JSTMakeBufferValue(exec_child_err), 0);
				JSTFreeBuffer(exec_child_err);
			} 
		}
		i = 0; while (i < argc) JSTFreeBuffer(nargv[i++]);
	}
	return (JSValueRef) exec;
}

static JSValueRef jst_writeOutput JSToolsFunction () {

	char * output;
	JSValueRef result = JSTMakeNumber(
		g_printf("%s", JSTGetStringBuffer(JSTGetValueString(JSTParam(1), NULL), NULL, &output, true))
	);
	JSTFreeBuffer(output);
	return result;

}

static JSValueRef jst_writeError JSToolsFunction () {

	char * output;
	JSValueRef result = JSTMakeNumber(
		g_fprintf(stderr, "%s", JSTGetStringBuffer(JSTGetValueString(JSTParam(1), NULL), NULL, &output, true))
	);
	JSTFreeBuffer(output);
	return result;

}

static JSValueRef jst_writeFile JSToolsFunction () {
	char * contents, * file;	JSValueRef result = RtJS(undefined);
	result = JSTMakeBoolean(g_file_set_contents(
		JSTGetValueBuffer(JSTParam(1), &file), 
		JSTGetValueBuffer(JSTParam(2), &contents),
		-1, NULL
	)); JSTFreeBuffer(contents) && JSTFreeBuffer(file);
	return result;
}

static JSValueRef jst_loadScript JSToolsFunction () {

	char * fileName; JSValueRef file = JSTParam(1); JSObjectRef object = JSTParamObject(2);
	JSValueRef result = JSTRunScript(JSTGetValueBuffer(file, &fileName), object);
	JSTFreeBuffer(fileName);
	return result;

}

void _JSTLoadRuntime(register JSContextRef ctx, JSObjectRef global, int argc, char *argv[], char *envp[], register JSValueRef * exception) {

	RtJS(Global) = global;

	JSTEvalScript(JSToolsSupport, global, "JSToolsSupport");
	if (JSTCaughtException) JSTReportFatalException(1);

	RtJS(argc) = argc;
	RtJS(argv) = argv;
	RtJS(envp) = envp;

	JSTSetPropertyScript(
		global, "classOf", "if (o === null) return \"Null\"; if (o === undefined) return \"Undefined\"; return Object.prototype.toString.call(o).slice(8,-1);", "o"
	);

	JSTSetPropertyScript(
		global, "InvokeError", "try { throw Error(m) } catch(err) {var info = err.stack.split(\"\\n\")[3].split('@').pop().split(':');var e = new Error(m); e.sourceURL=info[0]; e.line=info[1]; e.name=t; return e;}", "t", "m"
	);

	JSTSetPropertyFunction(global, "writeOutput", &jst_writeOutput);
	JSTSetPropertyFunction(global, "writeError", &jst_writeError);

	JSTSetPropertyFunction(global, "loadScript", &jst_loadScript);
	JSTSetPropertyFunction(global, "shellExecute", &jst_shellExecute);
	JSTSetPropertyFunction(global, "exit", &jst_exit);
	JSTSetPropertyFunction(global, "chdir", &jst_chdir);

#define mkObjLnk(V) JSTRuntime.V = JSTGetPropertyObject(global, #V)
	mkObjLnk(isNaN); mkObjLnk(parseInt); mkObjLnk(parseFloat); mkObjLnk(escape); 
	mkObjLnk(isFinite); mkObjLnk(decodeURI); mkObjLnk(encodeURI); 
	mkObjLnk(decodeURIComponent); mkObjLnk(encodeURIComponent); 
	mkObjLnk(Array); mkObjLnk(Boolean); mkObjLnk(Date); mkObjLnk(Error);
	mkObjLnk(Function); mkObjLnk(JSON); mkObjLnk(Math); mkObjLnk(Number);
	mkObjLnk(Object); mkObjLnk(RangeError); mkObjLnk(ReferenceError);
	mkObjLnk(RegExp); mkObjLnk(String); mkObjLnk(SyntaxError); mkObjLnk(TypeError);
	mkObjLnk(URIError); mkObjLnk(classOf); mkObjLnk(writeOutput); mkObjLnk(writeError);

#define mkValLnk(V) JSTRuntime.V = JSTGetProperty(global, #V)
	mkValLnk(Infinity); mkValLnk(NaN); mkValLnk(undefined);

}

bool JSTFreeBuffer(char * buffer) {
	if (buffer) g_free(buffer); return true;
}
bool JSTFreeString(JSStringRef string) {
	if (string) JSStringRelease(string); return true;
}
bool _JSTCoreSetProperty JSToolsProcedure (JSObjectRef jsObject, JSStringRef jsStringName, JSValueRef jsValue, long jsAttributes) {

	/* This procedure does not return void like a dumbass would ... */

	JSObjectSetProperty(ctx, jsObject, jsStringName, jsValue, jsAttributes, exception);

	return true;

}

JSStringRef JSTCreateString(char * chrPtrBuffer, JSStringRef * jsStringRefResult, bool bFreeBuffer) {
	JSStringRef destination; if (! jsStringRefResult ) jsStringRefResult = &destination;
	*jsStringRefResult = JSStringCreateWithUTF8CString(chrPtrBuffer); if (bFreeBuffer) JSTFreeBuffer(chrPtrBuffer);
	return *jsStringRefResult;
}

char * JSTGetStringBuffer(JSStringRef jsStringSource, long * lngPtrSize, char ** chrPtrResult, bool bFreeSource) {
	char * destination; long size = 0;
	if (! chrPtrResult ) chrPtrResult = &destination;
	if (! lngPtrSize )  lngPtrSize = &size;
	if (jsStringSource) {
		if (*lngPtrSize == 0) *lngPtrSize = JSStringGetMaximumUTF8CStringSize(jsStringSource);
		if (*lngPtrSize) {
			JSStringGetUTF8CString(jsStringSource, (*chrPtrResult = g_new(char, *lngPtrSize)), *lngPtrSize);
		}
		if (bFreeSource) JSTFreeString(jsStringSource);
	}
	return *chrPtrResult;
}

JSValueRef _JSTMakeString JSToolsProcedure (JSStringRef jsString, JSValueRef * jsValRefResult, bool bFreeString) {
	JSValueRef destination; if (! jsValRefResult ) jsValRefResult = &destination;
	*jsValRefResult = JSValueMakeString(ctx, jsString); if (bFreeString) JSTFreeString(jsString);
	return *jsValRefResult;
}

JSStringRef _JSTGetValueString JSToolsProcedure (JSValueRef jsValueRef, JSStringRef * jsStringRefPtrResult) {
	JSStringRef destination; if (! jsStringRefPtrResult ) jsStringRefPtrResult = &destination;
	*jsStringRefPtrResult = JSValueToStringCopy(ctx, jsValueRef, exception);
	return *jsStringRefPtrResult;
}

char * _JSTLoadStringBuffer (char * chrPtrFile, char ** chrPtrPtrResult, bool bFreeFile) {
	char * destination; if (! chrPtrPtrResult ) chrPtrPtrResult = &destination;
	*chrPtrPtrResult = NULL;
	g_file_get_contents(chrPtrFile, chrPtrPtrResult, NULL, NULL); if (bFreeFile) JSTFreeBuffer(chrPtrFile);
	return *chrPtrPtrResult;
}

bool _JSTSetProperty JSToolsProcedure(JSObjectRef jsObject, char * chrPtrName, JSValueRef jsValue, long jsAttributes) {
	JSStringRef jsPropertyName;
	bool result = JSTCoreSetProperty(jsObject, JSTCreateStaticString(chrPtrName, &jsPropertyName), jsValue, jsAttributes);
	JSTFreeString(jsPropertyName);
	return result;
}

JSValueRef _JSTGetProperty JSToolsProcedure(JSObjectRef jsObject, char * chrPtrName) {
	JSStringRef jsPropertyName;
	JSValueRef result = JSTCoreGetProperty(jsObject, JSTCreateStaticString(chrPtrName, &jsPropertyName));
	JSTFreeString(jsPropertyName);
	return result;
}

bool _JSTDeleteProperty JSToolsProcedure(JSObjectRef jsObject, char * chrPtrName) {
	JSStringRef jsPropertyName;
	bool result = JSToolsCall(JSObjectDeleteProperty, jsObject, JSTCreateStaticString(chrPtrName, &jsPropertyName));
	JSTFreeString(jsPropertyName);
	return result;
}

bool _JSTHasProperty (JSContextRef ctx, JSObjectRef jsObject, char * chrPtrName) {
	JSStringRef jsPropertyName;
	bool result = JSObjectHasProperty(ctx, jsObject, JSTCreateStaticString(chrPtrName, &jsPropertyName));
	JSTFreeString(jsPropertyName);
	return result;
}

JSObjectRef _JSTMakeFunction JSToolsProcedure(char * chrPtrName, void * fnPtr) {
	JSStringRef jsPropertyName;
	JSObjectRef result = JSTCoreMakeFunction(JSTCreateStaticString(chrPtrName, &jsPropertyName), fnPtr);
	JSTFreeString(jsPropertyName);
	return result;
}

JSObjectRef _JSTSetPropertyFunction JSToolsProcedure(JSObjectRef jsObject, char * chrPtrName, void * fnPtr) {
	JSStringRef jsPropertyName;
	JSObjectRef result = JSTCoreMakeFunction(JSTCreateStaticString(chrPtrName, &jsPropertyName), fnPtr);
	JSTCoreSetProperty(jsObject, jsPropertyName, result, JSTPropertyConst);
	JSTFreeString(jsPropertyName);
	return result;
}

JSValueRef _JSTCallFunction (register JSContextRef ctx, JSValueRef * exception, JSObjectRef THIS, JSObjectRef FUNC, ...) {
 if ( ! THIS ) THIS = (JSTRuntime.Global);
 if ( ! (_Bool) ( FUNC && ! ((_Bool) (JSValueIsNull(ctx, (JSValueRef)FUNC))) && ! ((_Bool) (JSValueIsUndefined(ctx, (JSValueRef)FUNC))) ) || ! (_Bool) (JSObjectIsFunction(ctx, FUNC))) {
  return JSValueMakeUndefined(ctx);
 }
	register int argc = 0, argi = 0; va_list ap; JSValueRef arg = NULL;
	va_start(ap, FUNC);
		while ((arg = (JSValueRef) va_arg(ap, JSValueRef))) argc++;
	va_end(ap);
	if (argc) {
	JSValueRef argv[argc+1];
	argv[argc] = NULL;
	va_start(ap, FUNC);
		while (argi <= argc) argv[argi++] = va_arg(ap, JSValueRef);
	va_end(ap);
	return JSObjectCallAsFunction(ctx, FUNC, THIS, argc, argv, exception);
	} else {
	return JSObjectCallAsFunction(ctx, FUNC, THIS, 0, NULL, exception);
	}
}

JSObjectRef _JSTCallConstructor(register JSContextRef ctx, JSValueRef * exception, JSObjectRef OBJ, ...) {
 if ( ! (_Bool) ( OBJ && ! ((_Bool) (JSValueIsNull(ctx, (JSValueRef)OBJ))) && ! ((_Bool) (JSValueIsUndefined(ctx, (JSValueRef)OBJ))) ) || ! JSObjectIsConstructor(ctx, OBJ)) {
  return (JSObjectRef) (JSValueMakeUndefined(ctx));
 }
	register int argc = 0, argi = 0; va_list ap; JSValueRef arg = NULL;
	va_start(ap, OBJ);
		while ((arg = (JSValueRef) va_arg(ap, JSValueRef))) argc++;
	va_end(ap);
	if (argc) {
	JSValueRef argv[argc+1];
	argv[argc] = NULL;
	va_start(ap, OBJ);
		while (argi <= argc) argv[argi++] = va_arg(ap, JSValueRef);
	va_end(ap);
	return JSObjectCallAsConstructor(ctx, OBJ, argc, argv, exception);
	} else {
	return JSObjectCallAsConstructor(ctx, OBJ, 0, NULL, exception);
	}
}

JSObjectRef _JSTCompileFunction(JSContextRef ctx, JSValueRef * exception, char * chrPtrName, char * chrPtrBody, ...) {

	JSObjectRef result; va_list ap;
	JSStringRef jsName, jsBody; unsigned jsCount = 0, jsIndex = 0;

	JSTCreateStaticString(chrPtrName, &jsName);
	JSTCreateStaticString(chrPtrBody, &jsBody);

	va_start(ap, chrPtrBody);	while ((va_arg(ap, long) != 0)) jsCount++; va_end(ap);

	if (jsCount) {
		JSStringRef jsParam[jsCount+1]; jsParam[jsCount] = NULL;
		va_start(ap, chrPtrBody);
			while (jsIndex < jsCount) JSTCreateStaticString((char*)va_arg(ap, void *), &jsParam[jsIndex++]);
		va_end(ap);
		result = JSToolsCall(JSObjectMakeFunction, jsName, jsCount, jsParam, jsBody, NULL, 1);
		jsIndex = 0; while (jsIndex < jsCount) JSTFreeString(jsParam[jsIndex++]);
	} else {
		result = JSToolsCall(JSObjectMakeFunction, jsName, 0, NULL, jsBody, NULL, 1);		
	}

	JSTFreeString(jsName); JSTFreeString(jsBody);
	return result;

}

JSObjectRef _JSTSetPropertyScript(JSContextRef ctx, JSValueRef * exception, JSObjectRef jsObject, char * chrPtrName, char * chrPtrBody, ...) {
	JSObjectRef result; va_list ap;
	JSStringRef jsName = JSTCreateStaticString(chrPtrName, NULL);
	JSStringRef jsBody = JSTCreateStaticString(chrPtrBody, NULL);
	unsigned jsCount = 0, jsIndex = 0;
	va_start(ap, chrPtrBody); while ((va_arg(ap, long) != 0)) jsCount++; va_end(ap);
	if (jsCount) {
		JSStringRef jsParam[jsCount+1]; jsParam[jsCount] = NULL;
		va_start(ap, chrPtrBody);
			while (jsIndex < jsCount) {
				jsParam[jsIndex++] = JSTCreateStaticString((char*)va_arg(ap, void *), NULL);
			}
		va_end(ap);
		result = JSToolsCall(JSObjectMakeFunction, jsName, jsCount, jsParam, jsBody, NULL, 1);
		jsIndex = 0; while (jsIndex < jsCount) JSTFreeString(jsParam[jsIndex++]);
	} else {
		result = JSToolsCall(JSObjectMakeFunction, jsName, 0, NULL, jsBody, NULL, 1);		
	}

	JSTCoreSetProperty(jsObject, jsName, result, JSTPropertyRequired);
	JSTFreeString(jsName); JSTFreeString(jsBody);
	return result;
}

JSObjectRef _JSTCompilePropertyFunction(JSContextRef ctx, JSValueRef * exception, JSStringRef jsName, char * chrPtrBody, ...) {

	JSObjectRef result; va_list ap;
	JSStringRef jsBody; unsigned jsCount = 0, jsIndex = 0;

	JSTCreateStaticString(chrPtrBody, &jsBody);

	va_start(ap, chrPtrBody);	while ((va_arg(ap, long) != 0)) jsCount++; va_end(ap);

	if (jsCount) {
		JSStringRef jsParam[jsCount+1]; jsParam[jsCount] = NULL;
		va_start(ap, chrPtrBody);
			while (jsIndex < jsCount) JSTCreateStaticString((char*)va_arg(ap, void *), &jsParam[jsIndex++]);
		va_end(ap);
		result = JSToolsCall(JSObjectMakeFunction, jsName, jsCount, jsParam, jsBody, NULL, 1);
		jsIndex = 0; while (jsIndex < jsCount) JSTFreeString(jsParam[jsIndex++]);
	} else {
		result = JSToolsCall(JSObjectMakeFunction, jsName, 0, NULL, jsBody, NULL, 1);		
	}

	JSTFreeString(jsBody);
	return result;

}

JSObjectRef _JSTCoreCompileFunction(JSContextRef ctx, JSValueRef * exception, JSStringRef jsName, JSStringRef jsBody, ...) {

	JSObjectRef result; va_list ap;
	unsigned jsCount = 0, jsIndex = 0;

	va_start(ap, jsBody);	while ((va_arg(ap, long) != 0)) jsCount++; va_end(ap);

	if (jsCount) {
		JSStringRef jsParam[jsCount+1]; jsParam[jsCount] = NULL;
		va_start(ap, jsBody);
			while (jsIndex < jsCount) (jsParam[jsIndex++] = (JSStringRef)va_arg(ap, void *));
		va_end(ap);
		result = JSToolsCall(JSObjectMakeFunction, jsName, jsCount, jsParam, jsBody, NULL, 1);
	} else {
		result = JSToolsCall(JSObjectMakeFunction, jsName, 0, NULL, jsBody, NULL, 1);		
	}

	return result;

}

JSValueRef _JSTEvalScript JSToolsProcedure(char * script, JSObjectRef jsObject, char * fileName, long line) {
	JSStringRef jscript = JSTCreateStaticString(script, NULL);
	JSStringRef jfile = JSTCreateStaticString(fileName, NULL);
	JSValueRef result = JSToolsCall(JSEvaluateScript, jscript, jsObject, jfile, line);
	JSTFreeString(jscript); JSTFreeString(jfile);
	return result;
}

JSValueRef _JSTEval JSToolsProcedure (char * chrPtrScript, JSObjectRef jsObject) {
	JSStringRef jsStringRefScript;
	JSValueRef result = JSToolsCall(JSEvaluateScript, JSTCreateStaticString(chrPtrScript, &jsStringRefScript), jsObject, NULL, 1); JSTFreeString(jsStringRefScript);
	return result;
}

JSValueRef _JSTRunScript JSToolsProcedure(char * file, JSObjectRef this) {
	char *data, *script; script = JSTLoadStringBuffer(file, &data, false);
	if (*data == '#' && *(data+1) =='!') while (*script && *script != 10) script++;
	JSValueRef result = JSTEvalScript(script, this, file); JSTFreeBuffer(data);
	return result;
}

