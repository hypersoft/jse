#include "JSTools.inc"
#include "JSTError.inc"
#include "JSTInit.inc"
#include "sys.inc"
//#include "JSTools/Native.inc"

JSTValue jst_test_function JSTDeclareFunction() {
	puts("called test function");
	return JSTValueUndefined;
}

static JSTDeclareConstructor(jst_test_constructor) {
	puts("called test constructor");
	return NULL;
}

JSTObject JSTInit_ JSTUtility(JSTObject global, int argc, char * argv[], char * envp[]) {

	jst_envp_default = envp;

	JSTObject sys = JSTClassInstance(NULL, NULL);
	JSTObjectSetProperty(global, "sys", sys, JSTObjectPropertyAPI);
	JSTObjectSetProperty(sys, "global", global, JSTObjectPropertyAPI);

	JSTObjectSetMethod(global, "sys_error_number", jst_error_number, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_error_message", jst_error_message, JSTObjectPropertyAPI);

	JSTScriptEval(JSTErrorScript, global, "JSTError.js", 1);
	if (JSTScriptHasError) JSTScriptReportException(), exit(1);

	JSTObjectSetMethod(global, "sys_env_function", jst_envp_function, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_env_constructor", jst_envp_constructor, JSTObjectPropertyAPI);

	JSTObjectSetMethod(global, "sys_prototype", jst_object_prototype, JSTObjectPropertyAPI);

	JSTObjectSetMethod(global, "sys_exit", jst_exit, JSTObjectPropertyAPI);
	JSTObjectSetMethod(global, "sys_execute", jst_execute, JSTObjectPropertyAPI);

	JSTScriptEval(JSTInitScript, global, "JSTInit.js", 1);
	if (JSTScriptHasError) JSTScriptReportException(), exit(1);

/*	JSTObject object, engine, io, stream, file, read, write, memory, string;*/

/*	JSTObjectSetMethod(sys, "eval", jst_eval, 0);*/
/*	JSTObjectSetMethod(sys, "run", jst_run, 0);*/
/*	JSTObjectSetMethod(sys, "userOf", jst_user_of, 0);*/

/*	JSTObjectSetProperty(sys, "uid", JSTValueFromDouble(getuid()), 0);*/
/*	JSTObjectSetProperty(sys, "euid", JSTValueFromDouble(geteuid()), 0);*/
/*	JSTObjectSetProperty(sys, "gid", JSTValueFromDouble(getgid()), 0);*/
/*	JSTObjectSetProperty(sys, "pid", JSTValueFromDouble(getpid()), 0);*/

/*	object = JSTClassInstance(NULL, NULL);*/
/*	JSTObjectSetProperty(sys, "object", object, 0);*/
/*	JSTObjectSetMethod(object, "prototype", jst_object_prototype, 0);*/

/*	memory = JSTClassInstance(NULL, NULL);*/
/*	JSTObjectSetProperty(sys, "memory", memory, 0);*/
/*	JSTObjectSetMethod(memory, "free", jst_memory_free, 0);*/
/*	JSTObjectSetMethod(memory, "allocate", jst_memory_allocate, 0);*/
/*	JSTObjectSetMethod(memory, "resize", jst_memory_resize, 0);*/
/*	JSTObjectSetMethod(memory, "clear", jst_memory_clear, 0);*/
/*	JSTObjectSetMethod(memory, "read", jst_memory_read, 0);*/
/*	JSTObjectSetMethod(memory, "write", jst_memory_write, 0);*/
/*	JSTObjectSetMethod(memory, "open", jst_memory_open, 0);*/
/*	JSTObjectSetMethod(memory, "stream", jst_memory_stream, 0);*/
/*	JSTObjectSetMethod(memory, "block", jst_memory_block, 0);*/

/*	read = (JSTObject) JSTObjectGetProperty(memory, "read");*/
/*	JSTObjectSetMethod(read, "block", jst_memory_read_block, 0);*/

/*	write = (JSTObject) JSTObjectGetProperty(memory, "write");*/
/*	JSTObjectSetMethod(write, "block", jst_memory_write_block, 0);*/

/*	JSTObjectSetProperty(sys, "main", JSTValueFromUTF8(argv[1]), JSTObjectPropertyReadOnly);*/
/*	JSTObjectSetProperty(sys, "date", JSTScriptNativeEval("Date.now()", global), JSTObjectPropertyReadOnly);*/

/*	JSTObjectSetProperty(sys, "context", JSTValueFromPointer(ctx), 0);*/
/*	JSTObjectSetProperty(sys, "exception", JSTValueFromPointer(exception), 0);*/

/*	{	char buffer[PATH_MAX]; getcwd(buffer, PATH_MAX);*/
/*		JSTObjectSetProperty(sys, "path", JSTValueFromUTF8(buffer), JSTObjectPropertyReadOnly);*/
/*	}*/

/*	JSTObjectSetProperty(sys, "argc", JSTValueFromDouble(argc), 0);*/
/*	JSTObjectSetProperty(sys, "argv", JSTValueFromPointer(argv), 0);*/
/*	JSTObjectSetProperty(sys, "envp", JSTValueFromPointer(envp), 0);*/
/*	*/
/*	JSTObjectSetMethod(sys, "_io_path", jst_io_path, 0);*/
/*	JSTObjectSetMethod(sys, "_io_stream_print", jst_io_stream_print, 0);*/
/*	JSTObjectSetMethod(sys, "_io_stream_pointer", jst_io_stream_pointer, 0);*/

/*	string = JSTClassInstance(NULL, NULL);*/
/*	JSTObjectSetProperty(sys, "string", string, 0);*/
/*	JSTObjectSetMethod(string, "indexOf", jst_string_indexOf, 0);*/
/*	JSTObjectSetMethod(string, "read", jst_string_read, 0);*/
/*	read = (JSTObject) JSTObjectGetProperty(string, "read");*/
/*	JSTObjectSetMethod(read, "format", jst_string_read_format, 0);*/
	// implemented in script post init
/*	JSTObjectSetMethod(read, "field", jst_string_read_field, 0);*/
/*	JSTObjectSetMethod(read, "line", jst_string_read_line, 0);*/

/*	string = (JSTObject) JSTObjectGetProperty(global, "String");*/
/*	JSTObjectSetMethod(string, "toUTF8", jst_string_toUTF8, 0);*/
/*	JSTObjectSetMethod(string, "fromUTF8", jst_string_fromUTF8, 0);*/
/*	JSTObjectSetMethod(string, "toUTF16", jst_string_toUTF16, 0);*/
/*	JSTObjectSetMethod(string, "fromUTF16", jst_string_fromUTF16, 0);*/
/*	JSTObjectSetMethod(string, "toUTF32", jst_string_toUTF32, 0);*/
/*	JSTObjectSetMethod(string, "fromUTF32", jst_string_fromUTF32, 0);*/

/*	engine = JSTClassInstance(NULL, NULL);*/
/*	JSTObjectSetProperty(sys, "engine", engine, 0);*/

/*	JSTObjectSetProperty(engine, "copyright", JSTValueFromUTF8(JSTReservedAddress), JSTObjectPropertyReadOnly);*/
/*	JSTObjectSetProperty(engine, "vendor", JSTValueFromUTF8(VENDOR), JSTObjectPropertyReadOnly);*/
/*	JSTObjectSetProperty(engine, "version", JSTValueFromUTF8(VERSION), JSTObjectPropertyReadOnly);*/
/*	JSTObjectSetProperty(engine, "codename", JSTValueFromUTF8(CODENAME), JSTObjectPropertyReadOnly);*/
/*	JSTObjectSetProperty(engine, "path", JSTValueFromUTF8(argv[0]), JSTObjectPropertyReadOnly);*/

/*	io = JSTClassInstance(NULL, NULL);*/
/*	JSTObjectSetProperty(sys, "io", io, 0);*/
/*	*/
/*	JSTObjectSetMethod(io, "flush", jst_io_flush, 0);*/

/*	file = JSTClassInstance(NULL, NULL);*/
/*	JSTObjectSetProperty(io, "file", file, 0);*/
/*	JSTObjectSetMethod(file, "open", jst_io_file_open, 0);*/
/*	JSTObjectSetMethod(file, "close", jst_io_file_close, 0);*/
/*	JSTObjectSetMethod(file, "read", jst_io_file_read, 0);*/
/*	JSTObjectSetMethod(file, "write", jst_io_file_write, 0);*/
/*	JSTObjectSetMethod(file, "stream", jst_io_file_stream, 0);*/
/*	JSTObjectSetMethod(file, "pipe", jst_io_file_pipe, 0);*/
/*	JSTObjectSetMethod(file, "clone", jst_io_file_clone, 0);*/
/*	JSTObjectSetMethod(file, "redirect", jst_io_file_redirect, 0);*/
/*	JSTObjectSetMethod(file, "mode", jst_io_file_mode, 0);*/

/*	JSTScriptEval(JSTInitScript, global, "jse.init.js", 1);*/
/*	if (JSTScriptHasError) JSTScriptReportException(), exit(1);*/

/*	stream = (JSTObject) JSTObjectGetProperty(io, "stream");*/
/*	JSTObjectSetProperty(stream, "read", read, 0);*/

/*	JSTObjectSetMethod(stream, "open", jst_io_stream_open, 0);*/
/*	JSTObjectSetMethod(stream, "reopen", jst_io_stream_reopen, 0);*/
/*	JSTObjectSetMethod(stream, "close", jst_io_stream_close, 0);*/
/*	JSTObjectSetMethod(stream, "flush", jst_io_stream_flush, 0);*/
/*	JSTObjectSetMethod(stream, "rewind", jst_io_stream_rewind, 0);*/
/*	JSTObjectSetMethod(stream, "seek", jst_io_stream_seek, 0);*/
/*	JSTObjectSetMethod(stream, "position", jst_io_stream_position, 0);*/
/*	JSTObjectSetMethod(stream, "eof", jst_io_stream_eof, 0);*/
/*	JSTObjectSetMethod(stream, "descriptor", jst_io_stream_descriptor, 0);*/
/*	JSTObjectSetMethod(stream, "wide", jst_io_stream_wide, 0);*/
/*	JSTObjectSetMethod(stream, "read", jst_io_stream_read, 0);*/
/*	JSTObjectSetMethod(stream, "write", jst_io_stream_write, 0);*/
/*	JSTObjectSetMethod(stream, "buffer", jst_io_stream_buffer, 0);*/

/*	read = (JSTObject) JSTObjectGetProperty(stream, "read");*/
/*	JSTObjectSetMethod(read, "line", jst_io_stream_read_line, 0);*/
/*	JSTObjectSetMethod(read, "field", jst_io_stream_read_field, 0);*/
/*	JSTObjectSetMethod(read, "format", jst_io_stream_read_format, 0);*/
/*	JSTObjectSetProperty(global, "read", read, 0);*/

/*	JSTObject process = JSTClassInstance(NULL, NULL);*/
/*	JSTObjectSetProperty(stream, "process", process, 0);*/
/*	JSTObjectSetMethod(process, "open", jst_io_stream_process_open, 0);*/
/*	JSTObjectSetMethod(process, "close", jst_io_stream_process_close, 0);*/

/*	JSTObjectSetMethod(stream, "error", jst_io_stream_error, 0);*/
/*	JSTObject error = (JSTObject) JSTObjectGetProperty(stream, "error");*/
/*	JSTObjectSetMethod(error, "clear", jst_io_stream_error_clear, 0);*/

/*	char * script = JSTConstructUTF8(*/
/*		"sys.type.width.bool = %i, sys.type.width.char = %i, "*/
/*		"sys.type.width.short = %i, sys.type.width.long = %i, "*/
/*		"sys.type.width.size = %i, sys.type.width.pointer = %i, "*/
/*		"sys.type.width.int64 = %i, sys.type.width.float = %i, "*/
/*		"sys.type.width.double = %i, sys.type.width.value = %i, "*/
/*		"sys.type.width.string = %i; Object.freeze(sys.type.width);"*/
/*		// other convenient data */
/*		"sys.byteOrder = %i; "*/
/*		// finalization*/
/*		"sys.postScript(); sys.memory.alignment = %i;",*/
/*		sizeof(bool), sizeof(char),*/
/*		sizeof(short), sizeof(long),*/
/*		sizeof(size_t), sizeof(intptr_t),*/
/*		sizeof(long long), sizeof(float),*/
/*		sizeof(double), sizeof(intptr_t),*/
/*		sizeof(intptr_t),*/
/*		// other convenient data 
/*		G_BYTE_ORDER, G_MEM_ALIGN*/
/*	);*/

//	JSTScriptNativeEval(script, global); 

/*	free(script);*/
/*	if (JSTScriptHasError) JSTScriptReportException(), exit(1);*/

	return global;

}

