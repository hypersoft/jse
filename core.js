#!bin/jse

p = sys.object.create(/* system object interface */{

	name: 'p', value: {}, prototype: {crazy:"going ham"},
	exec:function(){}, new:function(){},
	init:function(data, prototype){
		// prototype has not yet been set as the prototype of this.
		// print.line(prototype.isPrototypeOf(this)); // false
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
	delete:function(data, item){
		print.line("deleting ", item, " "+v);
		if (Object.hasOwnProperty(data, item)){
			delete data[item]; return true
		} else return false;
	},
	product: function(value){ // check some stuff, return some truth.
		return false;
	},
});

p.food = ['pear','apple','pineapple','pie']

Object.defineProperty(p, "syrup", 
	{value:['maple','sugar free','caramel','cherry','strawberry'],enumerable:true}
);

for (name in p) value = p[name], print.line(name, ": ",
	(Array.prototype.isPrototypeOf(value)) ? value.join(', ') : value
);

