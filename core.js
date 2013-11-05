#!bin/jse

//var echo = new Command('echo', '-E');
//var print = new Command('echo', '-En');
//var printf = new Command('printf', '--');


Procedure("jse", "int", "printf", ['utf8 *', 'utf8 *', 'utf8 *', 'utf8 *'])
("%s JSE %s v%s\n", js.vendor, js.codename, js.version);

