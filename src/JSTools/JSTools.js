function InvokeError(t, m){
    try { throw Error(m) } catch(err) {
		var info = err.stack.split("\n")[3].split("@").pop().split(":");
		var e = new Error(m);
		e.file=info[0];
		e.line=info[1];
		e.name=t; 
		return e;
	}
}

var print = function(msg) { writeOutput(msg + "\n"); }


var ShellCommand = function() {
	var genCommandProto = function(fn) {
		return {
			constructor: fn,
			push: function () { return Array.prototype.push.apply(fn.argv, arguments); },
			pop: function () { return Array.prototype.pop.apply(fn.argv, arguments); },
			shift: function () { return Array.prototype.shift.apply(fn.argv, arguments); },
			unshift: function () { return Array.prototype.unshift.apply(fn.argv, arguments); },
			slice: function () { return Array.prototype.slice.apply(fn.argv, arguments); },
		}
	}
	var command = function() {
		var args = [];
		args.push.apply(args, command.argv);
		args.push.apply(args, arguments);
		return shellExecute.apply(command, args);
	}
	command.argv = [];
	command.captureOutput = false;
	command.captureError = false;
	command.returnStatus = false;
	command.prototype = genCommandProto(command);
	command.prototype.push.apply(command.argv, arguments);
	return command;
}

bash = ShellCommand("bash");

bash.scriptlet = function(name, script) {
	if (script == undefined) { script = name; name = "jse.bash"; }
	var cmd = ShellCommand("bash", "-c", script, name);
	Object.defineProperty(cmd, "interpreter", { 
		get: function() {  return this.argv[0]; },
		set: function(value) {  this.argv[0] = value; },
	});
	Object.defineProperty(cmd, "option", { 
		get: function() {  return this.argv[1]; },
		set: function(value) {  this.argv[1] = value; },
	});
	Object.defineProperty(cmd, "script", { 
		get: function() {  return this.argv[2]; },
		set: function(value) {  this.argv[2] = value; },
	});
	Object.defineProperty(cmd, "id", { 
		get: function() {  return this.argv[3]; },
		set: function(value) {  this.argv[3] = value; },
	});
	return cmd;
}

printf = new bash.scriptlet('printf "$@"');
echo = new bash.scriptlet('echo "$@"');
canReadFile = new bash.scriptlet('[[ -r "$1" ]]');
canWriteFile = new bash.scriptlet('[[ -w "$1" ]]');

function readFile(file) {
	if (canReadFile(file)) {
		var cat = ShellCommand("cat");
		cat.captureOutput = true;
		return String(cat(file));
	}
	return undefined;
}

function writeFile(file, data, append) {
	var write = bash.scriptlet("jse.writeFile", 'printf %s "$2" '+(append == true)?(">>"):(">")+' "$1";');
	return Boolean(write(file, data));
}

function appendFile(file, data) {
	return writeFile(file, data, true);
}

function touchFile() {
	var touch = ShellCommand("touch");
	return Boolean(touch.apply(touch, arguments));
}

function deleteFile() {
	var rm = ShellCommand("rm");
	return Boolean(rm.apply(rm, arguments));
}

function moveFile() {
	var mv = ShellCommand("mv");
	return Boolean(mv.apply(mv, arguments));
}

function changeFileMode() {
	var chmod = ShellCommand("chmod");
	return Boolean(chmod.apply(chmod, arguments));
}

function copyFile() {
	var cp = ShellCommand("cp");
	return Boolean(cp.apply(cp, arguments));
}

function installFile() {
	var install = ShellCommand("install");
	return Boolean(install.apply(install, arguments));
}

function fileLink() {
	var ln = ShellCommand("ln");
	return Boolean(ln.apply(ln, arguments));
}

function makeFifo() {
	var mkfifo = ShellCommand("mkfifo");
	return Boolean(mkfifo.apply(mkfifo, arguments));
}

function fileGlob() {
	var sh = bash.scriptlet("globlin", 'shopt -s extglob; for glob in $@; do printf "%s\n" $glob; done;');
	return Boolean(sh.apply(sh, arguments));
}

