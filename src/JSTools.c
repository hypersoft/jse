
#include "JSToolsShared.h"
#include <wait.h>

JSTGlobalRuntime JSTRuntime;

JSTPropertyMaster;

bool JSTFreeBuffer(char * buffer) {
	if (buffer) g_free(buffer); return true;
}
bool JSTFreeString(JSStringRef string) {
	if (string) JSStringRelease(string); return true;
}
bool _JSTCoreSetProperty JSToolsProcedure (JSObjectRef jsObject, JSStringRef jsStringName, JSValueRef jsValue, long jsAttributes) {
	JSObjectSetProperty(ctx, jsObject, jsStringName, jsValue, jsAttributes, exception);
	return true;
}
JSStringRef _JSTCreateString(char * chrPtrBuffer, JSStringRef * jsStringPtrResult, bool bFreeBuffer) {
	return JSTCreateString(chrPtrBuffer, jsStringPtrResult, bFreeBuffer);
}
char * _JSTGetStringBuffer(JSStringRef jsStringSource, char ** chrPtrPtrResult, long * lngPtrSize, bool bFreeSource) {
	return JSTGetStringBuffer(jsStringSource, chrPtrPtrResult, lngPtrSize, bFreeSource);
}
JSValueRef _JSTMakeString JSToolsProcedure (JSStringRef jsString, JSValueRef * jsValPtrResult, bool bFreeString) {
	return JSTMakeString(jsString, jsValPtrResult, bFreeString);
}
JSStringRef _JSTGetValueString JSToolsProcedure (JSValueRef jsVal, JSStringRef * jsStringPtrResult) {
	return JSTGetValueString(jsVal, jsStringPtrResult);
}
char * _JSTLoadStringBuffer (char * chrPtrFile, char ** chrPtrPtrResult, bool bFreeFile) {
	return JSTLoadStringBuffer(chrPtrFile, chrPtrPtrResult, bFreeFile);
}

static JSValueRef jst_chdir JSToolsFunction () {
	char * val; JSValueRef
	result = JSTMakeNumber(g_chdir(JSTGetValueBuffer(JSTParam(1), &val)));
	JSTFreeBuffer(val); return result;
}

static JSValueRef jst_shell JSToolsFunction () {
	char ** nargv; char * usrcommand; int nargc, child_status;
	JSObjectRef exec = RtJSObject(undefined);
	gchar *exec_child_out, *exec_child_err; gint exec_child_status;
	if (g_shell_parse_argv(JSTGetValueBuffer(JSTParam(1), &usrcommand), &nargc, &nargv, NULL)) {
		if (g_spawn_sync(NULL, nargv, NULL, G_SPAWN_LEAVE_DESCRIPTORS_OPEN | G_SPAWN_SEARCH_PATH | G_SPAWN_CHILD_INHERITS_STDIN, NULL, NULL, &exec_child_out, &exec_child_err, &exec_child_status, NULL)) {
			exec_child_status = WEXITSTATUS(exec_child_status);
			exec = JSValueToObject(ctx, JSTMakeNumber(exec_child_status), NULL);
			JSTSetProperty(exec, "command", JSTParam(1), 0);
			JSTSetProperty(exec, "stdout", JSTMakeBufferValue(exec_child_out), 0);
			JSTFreeBuffer(exec_child_out);
			JSTSetProperty(exec, "stderr", JSTMakeBufferValue(exec_child_err), 0);
			JSTFreeBuffer(exec_child_err); 
		}
		g_strfreev(nargv);
	}
	JSTFreeBuffer(usrcommand);
	return (JSValueRef) exec;
}

static JSValueRef jst_put JSToolsFunction () {

	JSTPropertyMaster; JSTValueRef = JSTMakeNumber(
		g_printf("%s\n",
			JSTGetStringBuffer(
				JSTGetValueString(JSTParam(1), &JSTStringRef), &JSTBufferRef, &JSTBufferLen, true
			)
		)
	);
	JSTFreeBuffer(JSTBufferRef);
	return JSTValueRef;

}

static JSValueRef jst_write JSToolsFunction () {

	JSTPropertyMaster; JSTValueRef = JSTMakeNumber(
		g_printf("%s",
			JSTGetStringBuffer(
				JSTGetValueString(JSTParam(1), &JSTStringRef), &JSTBufferRef, &JSTBufferLen, true
			)
		)
	);

	JSTFreeBuffer(JSTBufferRef);
	return JSTValueRef;

}

static JSValueRef jst_readFile JSToolsFunction () {

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

static JSValueRef jst_appendFile JSToolsFunction () {

}

static JSValueRef jst_loadScript JSToolsFunction () {

	char * fileName; JSValueRef file = JSTParam(1); JSObjectRef object = JSTParamObject(2);
	JSTValueRef = JSTRunScript(JSTGetValueBuffer(file, &fileName), object);
	JSTFreeBuffer(fileName);
	return JSTValueRef;

}

static JSValueRef jst_fake2 JSTFunctionRelay (
	JSTCompileFunction("fake", "put(\"you called fake: \" + arg);", "arg")
)

void _JSTLoadRuntime(register JSContextRef ctx, register JSValueRef * exception) {

	register JSObjectRef global = (JSTRuntime.Global = JSContextGetGlobalObject(ctx));

	JSTSetPropertyScript(
		global, "classOf", "if (o === null) return \"Null\"; if (o === undefined) return \"Undefined\"; return Object.prototype.toString.call(o).slice(8,-1);", "o"
	);

	JSTSetPropertyFunction(global, "loadScript", &jst_loadScript);
	JSTSetPropertyFunction(global, "readFile", &jst_readFile);
	JSTSetPropertyFunction(global, "writeFile", &jst_writeFile);
	JSTSetPropertyFunction(global, "appendFile", &jst_appendFile);
	JSTSetPropertyFunction(global, "put", &jst_put);
	JSTSetPropertyFunction(global, "write", &jst_write);
	JSTSetPropertyFunction(global, "shell", &jst_shell);
	JSTSetPropertyFunction(global, "chdir", &jst_chdir);

#define mkObjLnk(V) JSTRuntime.V = JSTGetPropertyObject(global, #V)
	mkObjLnk(isNaN); mkObjLnk(parseInt); mkObjLnk(parseFloat); mkObjLnk(escape); 
	mkObjLnk(isFinite); mkObjLnk(decodeURI); mkObjLnk(encodeURI); 
	mkObjLnk(decodeURIComponent); mkObjLnk(encodeURIComponent); 
	mkObjLnk(Array); mkObjLnk(Boolean); mkObjLnk(Date); mkObjLnk(Error);
	mkObjLnk(Function); mkObjLnk(JSON); mkObjLnk(Math); mkObjLnk(Number);
	mkObjLnk(Object); mkObjLnk(RangeError); mkObjLnk(ReferenceError);
	mkObjLnk(RegExp); mkObjLnk(String); mkObjLnk(SyntaxError); mkObjLnk(TypeError);
	mkObjLnk(URIError); mkObjLnk(classOf);

#define mkValLnk(V) JSTRuntime.V = JSTGetProperty(global, #V)
	mkValLnk(Infinity); mkValLnk(NaN); mkValLnk(undefined);

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

	JSTPropertyMaster; va_list ap;
	JSStringRef jsName, jsBody; unsigned jsCount = 0, jsIndex = 0;

	JSTCreateStaticString(chrPtrName, &jsName);
	JSTCreateStaticString(chrPtrBody, &jsBody);

	va_start(ap, chrPtrBody);	while ((va_arg(ap, long) != 0)) jsCount++; va_end(ap);

	if (jsCount) {
		JSStringRef jsParam[jsCount+1]; jsParam[jsCount] = NULL;
		va_start(ap, chrPtrBody);
			while (jsIndex < jsCount) JSTCreateStaticString((char*)va_arg(ap, void *), &jsParam[jsIndex++]);
		va_end(ap);
		JSTObjectRef = JSToolsCall(JSObjectMakeFunction, jsName, jsCount, jsParam, jsBody, NULL, 1);
		jsIndex = 0; while (jsIndex < jsCount) JSTFreeString(jsParam[jsIndex++]);
	} else {
		JSTObjectRef = JSToolsCall(JSObjectMakeFunction, jsName, 0, NULL, jsBody, NULL, 1);		
	}

	JSTFreeString(jsName); JSTFreeString(jsBody);
	return JSTObjectRef;

}

JSObjectRef _JSTCompilePropertyFunction(JSContextRef ctx, JSValueRef * exception, JSStringRef jsName, char * chrPtrBody, ...) {

	JSTPropertyMaster; va_list ap;
	JSStringRef jsBody; unsigned jsCount = 0, jsIndex = 0;

	JSTCreateStaticString(chrPtrBody, &jsBody);

	va_start(ap, chrPtrBody);	while ((va_arg(ap, long) != 0)) jsCount++; va_end(ap);

	if (jsCount) {
		JSStringRef jsParam[jsCount+1]; jsParam[jsCount] = NULL;
		va_start(ap, chrPtrBody);
			while (jsIndex < jsCount) JSTCreateStaticString((char*)va_arg(ap, void *), &jsParam[jsIndex++]);
		va_end(ap);
		JSTObjectRef = JSToolsCall(JSObjectMakeFunction, jsName, jsCount, jsParam, jsBody, NULL, 1);
		jsIndex = 0; while (jsIndex < jsCount) JSTFreeString(jsParam[jsIndex++]);
	} else {
		JSTObjectRef = JSToolsCall(JSObjectMakeFunction, jsName, 0, NULL, jsBody, NULL, 1);		
	}

	JSTFreeString(jsBody);
	return JSTObjectRef;

}

JSObjectRef _JSTCoreCompileFunction(JSContextRef ctx, JSValueRef * exception, JSStringRef jsName, JSStringRef jsBody, ...) {

	JSTPropertyMaster; va_list ap;
	unsigned jsCount = 0, jsIndex = 0;

	va_start(ap, jsBody);	while ((va_arg(ap, long) != 0)) jsCount++; va_end(ap);

	if (jsCount) {
		JSStringRef jsParam[jsCount+1]; jsParam[jsCount] = NULL;
		va_start(ap, jsBody);
			while (jsIndex < jsCount) (jsParam[jsIndex++] = (JSStringRef)va_arg(ap, void *));
		va_end(ap);
		JSTObjectRef = JSToolsCall(JSObjectMakeFunction, jsName, jsCount, jsParam, jsBody, NULL, 1);
	} else {
		JSTObjectRef = JSToolsCall(JSObjectMakeFunction, jsName, 0, NULL, jsBody, NULL, 1);		
	}

	return JSTObjectRef;

}


JSValueRef _JSTRunScript JSToolsProcedure(char * file, JSObjectRef this) {

	char * data; char * script;
	script = JSTLoadStringBuffer(file, &data, false);
	if (*data == '#' && *(data+1) =='!') while (*script && *script != 10) script++;
	JSValueRef result = JSTEval(script, this); JSTFreeBuffer(data);

	JSValueRef * foo = { NULL };

	MicroStackAlias(sfer, TypeList(void, *one = NULL, *two, *three)) x;
	x.one++;
	if (JSTCaughtException) {
		JSTPropertyMaster;
		JSTSetProperty((JSObjectRef)JSTCaughtException, "file", JSTMakeBufferValue(file), 0);
	}
	
	return result;
}

#define StackBuffer(NAME, SIZE)	NAME[SIZE] = g_alloca(SIZE)

#define uBranch(CASE, PASS, FAIL) ( (CASE) ? (PASS) : (FAIL) )

#define StackType(TYPE, NAME, ...) TYPE __VA_ARGS__

