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
	prototype: {},

	// some extern objects need an internalized function handler.
	exec:function(){},

	// some extern objects may need an internalized constructor handler as well.
	new:function(){
		/* this handler does not construct any thing. it is called as a function
			of value, therefore one must construct and return an object manually.
		*/
	},

	/* optionally, construct static principles of the resulting system object instance */

	init:function(extern, prototype){

		// prototype has not yet been set as the prototype of extern.
		// in the native code, this means the property functions of extern are not,
		// reached. return value of this function is void.

		this.keep = {}

	},

	get: function(item){
		return this.keep[item] || null
	},

	set: function(item, value){
		this.keep[item] = value; return true
	},

	delete: function(item){
		if (Object.hasOwnProperty(keep, item)){
			delete this.keep[item]; return true
		} else return false;
	},

	enumerate:function(){
		return Object.keys(this.keep)
	},

	/* it is a special case when the system or script defines a property on extern */
	/* the property set handler is external to the class interface, therefore */
	/* the resulting data is defined on extern as is permissible, instead of */
	/* the operation prescribed by set. equally, this interface will be queried */
	/* for all property operations, before they are handled by extern or prototype */

});

//print.line(p);

p.food = ['pear','apple','pie']

for (name in p) print.line(name);

print.line(Object.hasOwnProperty(p, 'food'));

//print.line(p.food[2]);

