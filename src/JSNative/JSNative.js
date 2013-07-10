
new JSNative.Type.Alias(0, "void");
new JSNative.Type.Alias(10, "bool");

new JSNative.Type.Alias(20, "char");
new JSNative.Type.Alias(20, "signed char");
new JSNative.Type.Alias(20, "char signed");
new JSNative.Type.Alias(21, "unsigned char");
new JSNative.Type.Alias(21, "char unsigned");

new JSNative.Type.Alias(30, "short");
new JSNative.Type.Alias(30, "signed short");
new JSNative.Type.Alias(30, "short signed");
new JSNative.Type.Alias(31, "unsigned short");
new JSNative.Type.Alias(31, "short unsigned");

new JSNative.Type.Alias(40, "int");
new JSNative.Type.Alias(40, "signed int");
new JSNative.Type.Alias(40, "int signed");
new JSNative.Type.Alias(41, "unsigned int");
new JSNative.Type.Alias(41, "int unsigned");

new JSNative.Type.Alias(50, "long");
new JSNative.Type.Alias(50, "signed long");
new JSNative.Type.Alias(50, "long signed");
new JSNative.Type.Alias(51, "unsigned long");
new JSNative.Type.Alias(51, "long unsigned");

new JSNative.Type.Alias(60, "long long");
new JSNative.Type.Alias(60, "signed long long");
new JSNative.Type.Alias(60, "long long signed");
new JSNative.Type.Alias(61, "unsigned long long");
new JSNative.Type.Alias(61, "long long unsigned");

new JSNative.Type.Alias(70, "float");
new JSNative.Type.Alias(80, "double");

new JSNative.Type.Alias(90, "pointer");
new JSNative.Type.Alias(90, "void *");

var print = function(msg) { writeOutput(msg + "\n"); }
var stdin = new Object(0); stdin.EOF = false; stdin.line = 0;

fileExists = function(file) {
	return ! (shell("bash -c '[[ -e \"$1\" ]]' \"" + file + "\""));
}

stdin.readLine = function() {
	var result = shell("bash -c 'read; status=$?; printf %s \"$REPLY\"; exit $status;'");
	this.EOF = ( parseInt(result) != 0 );
	if (! this.EOF ) return result.stdout + "\n";
	stdin.line++;
	return result.stdout;
}

stdin.repl = function() {
	while (! stdin.EOF ) {
		try { eval(stdin.readLine()); } catch (e) { print(e); }
	}
}


