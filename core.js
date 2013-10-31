#!bin/jse

var echo = new Command('echo', '-E');
var print = new Command('echo', '-En');
//var printf = new Command('printf', '--');

echo(new SharedLibrary('libgtk-3.so.0').pointer)

