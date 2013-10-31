#!bin/jse

var echo = new Command('echo', '-E');
var print = new Command('echo', '-En');
//var printf = new Command('printf', '--');

echo(new SharedLibrary('libgtk-7.so.0').pointer)


