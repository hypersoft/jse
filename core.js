#!bin/jse

p = sys.object.create(/* system object interface */{

	name: 'p',
	// this class interface table is not public to any script. it is commonly stored
	// on the created prototype as a hidden system property.

	// all procedures herein are defined as a function of an external object. 
	// these external property methods are bound to a private interface 'value'.

	value: {},

	// the public interface instance 'extern' may optionally have inheritable properties.
	// this prototype will be constructed as a prototype of itself, so that modifications
	// to the prototype via script do not propogate.
	prototype: {crazy:"going ham"},

	// some extern objects need an internalized function handler.
	exec:function(){},

	// some extern objects may need an internalized constructor handler as well.
	new:function(){
		/* this handler does not construct any thing. it is called as a function
			of value, therefore one must construct and return an object manually.
		*/
	},

	/* optionally, construct static principles of the resulting system object instance */

	init:function(data, prototype){
		// prototype has not yet been set as the prototype of extern.
		// print.line(prototype.isPrototypeOf(extern)); // false

		//Object.defineProperty(this, "keys", {get:function(){return Object.keys(this.data)}});
	},

	get: function(data, item){
		if (data[item] != undefined) {
			print.line("requesting ", item);
			return data[item]
		}
		return null
	},

	set:function(data, item, value){
		print.line("setting ", item);
		data[item] = value; return true
	},

	delete:function(data, item, v){
		print.line("deleting ", item, " "+v);
		if (Object.hasOwnProperty(data, item)){
			delete data[item]; return true
		} else return false;
	},

	enumerate:function(data){
		print.line("enumerating");
		return Object.keys(data);
	},
});

p.food = ['pear','apple','pineapple','pie']

Object.defineProperty(p, "syrup", 
	{value:['maple','sugar free','caramel','cherry','strawberry'],enumerable:true}
);

for (name in p) value = p[name], print.line(name, ": ",
	(Array.prototype.isPrototypeOf(value)) ? value.join(', ') : value
);

