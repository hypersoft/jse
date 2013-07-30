function classOf(o) {
	if (o === null) return "Null"; if (o === undefined) return "Undefined";
	return Object.prototype.toString.call(o).slice(8,-1);
}

function extend(o, p) {
	for (prop in p) o[prop] = p[prop];
	return o;
}

function extendPrototype(o, p) {
	for(prop in p) {
		var property = p[prop];
		if (typeof property == 'function') Object.defineProperty(o, prop, {value:property,enumerable:false,configurable:true,writeable:true});
		else o[prop] = property;
	}
	return o;
}

function callStack() {
	var trace = [];
	function toString() {
		return this.file+': '+this.function+': line '+this.line
	}
	function parse(c) {
		var o = {};
		var parsed = c.match(/(([^@]+)@)?([^:]+):(.+)$/);
		if (parsed !== null) {
		o.line = parsed.pop();
		o.file = parsed.pop();
		(o.function = parsed.pop())? undefined:(o.function = 'anonymous function');
		} else {
			o.file = c;
		}
		o.toString = toString;
		return o;
	}
	try { throw Error('n/a') } catch(err) {
		var info = err.stack.split("\n");
		info.shift(); // remove 'this call'
		for (index in info) trace.unshift(parse(info[index])); // add in reverse order (logical order)
	}
	return trace;
}

function InvokeError(title, message) {
	var stack = callStack();
	while (stack.pop().function != 'InvokeError');
	while((invoke = stack.pop()).file == '[native code]');
	var e = new Error(message);
	if (invoke.function != 'global code') e.function = invoke.function;
	e.sourceURL=invoke.file; e.line=invoke.line; e.name=title;
	e.toString = InvokeError.toString;
	return e;
}

InvokeError.toString = function() {
	return this.sourceURL+': '+this.function+': line '+this.line+': '+this.name+': '+this.message;
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
	if (script == undefined) { if (name == undefined) throw new InvokeError("bash.scriptlet", "invalid arguments"); script = name; name = "jse.bash"; }
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

