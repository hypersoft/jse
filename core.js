#!bin/jse

/* Ye Old Test Class */

JSNative = new Class("JSNative");

Shell = new Class(

	'Shell',
	{	/* Shell.prototype Object */
		echo: function() { Shell.echo.apply(this, arguments) } 
	},
	{	/* Shell 'SuperConstructor' methods */
		create: { value: function() { this.echo("I am baby", Shell.name); }, configurable:true },
		global: { value: function() { Shell.echo("I am", Shell.name) }, configurable:true },
		/* an example function */
		echo: {	value: function() { echo.apply(this, arguments) }, configurable:true }
	}

);

Shell();
shell = new Shell();
shell.echo("I am your", classOf(shell));


