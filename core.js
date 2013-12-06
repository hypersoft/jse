#!bin/jse

var map = new sys.object.bitmap(16, 
	['void','const','integer','unsigned','bool','char','short','int','long','int64','float','double','pointer','size','string','value']
)(0);

map([1,undefined,1]);

//var map = new sys.object.bitmap(2, ['a','b'])(7)
print(map);
