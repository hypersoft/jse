
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


