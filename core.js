#!bin/jse

Shell = new Class('Shell',
	{ 
		echo: function() { Shell.echo.apply(this, arguments) } 
	},
	{
		create: { value: function() { Shell.echo("I am baby", Shell.name); }, configurable:true },
		global: { value: function() { Shell.echo("I am", Shell.name) }, configurable:true },
		echo: {	value: function() { echo.apply(this, arguments) }, configurable:true }
	}
);

Shell();
shell = new Shell();
shell.echo("I am your", classOf(shell));

