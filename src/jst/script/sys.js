/*

Hypersoft Systems JST AND Hypersoft Systems JSE
Copyright (c) 2014, Triston J. Taylor

All rights reserved.

*/

sys.command = function() {
	this.argv = Array.apply(null, arguments);
	var bound = sys.execute.bind(this);
	bound.capture = this;
	bound.argv = this.argv;
	bound.toString = function(){
		return this.argv[0];
	};
	return bound;
};

var echo = new sys.command("echo");
