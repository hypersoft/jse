#!bin/jse

sys.type = {
	code: {
		1:1, 2:2, 4:4, 8:8, signed:(1 << 5), integer:(1 << 6), boolean:(1 << 7),
		value:(1 << 8), utf:(1 << 9)
	}
}


sys.exit();
var Pointer = Class({ name: 'Pointer', attributes: 1,
	
	create: function(ptr, type, length){
		echo(this.constructor);
	},
	
	call: function() {
		echo('calling back');
	},
	
	delete: function(name, value){
		echo('delete prop '+name);
		return true;
	},
	
	set: function(name, value){
		echo('set prop '+name);
		return true;
	},
	
	get: function(name){
		echo('get prop '+name);
	},
	
	test: function(name){
		echo('test prop '+name);
		return false;
	},
	
	list: function(name){
		echo('get prop '+name);
		return [];
	}
	
});

x = new Pointer(1, 2, 3);
delete x[0];
echo(x instanceof Pointer);

//echo (voidtype({}))
//new Type("utf8", {utf:8});
//echo(Type.flags.listNamesOf(utf8))
//b = utf8.toPointer(); //
//echo(Type.serialize(b))
//

//new Type("array", {array:12})
//echo(array.array)

//for (name in Type.flags) echo("#define jst_type_"+name+" "+Type.flags[name]);

sys.exit(0);

//var a = sys.class_define({
//	name: 'Function',
//	properties: 'property',
//	attributes: 1,
//	callAsFunction: null, 
////	callAsConstructor: null,
//	initialize: function(object, callAsFunction){
//		this.callAsFunction = callAsFunction;
//	},
//	hasProperty: function(name){
//		echo("Class has "+name);
//		return name in this.property;
//	},
//	deleteProperty: function(name){
//		echo("Class delete "+name);
//		if (name in this.property) {
//			delete this.property[name]; return true;
//		}
//		return false;
//	},
//	getPropertyNames: function(){
//		echo("Class list names");
//		return Object.keys(this.property);
//	},
//	setProperty: function(name, value){
//		echo("Class set "+name);
//		this.property[name] = value;
//		return true;
//	},
//	getProperty: function(name){
//		echo("Class get "+name);
//		return this.property[name] || null;
//	},
//
//});
//
//var x = a.create({}, function(){
//	echo("called as function!")
//});
//
//x()
