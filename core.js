#!bin/jse

p = sys.object.create(/* system object interface */{

	name: 'p', value: {},
	prototype: {crazy:"going ham"}, // self prototype
	instance: {color:'blue'}, // new prototype of 'this' for new object constructor
	new:function(){return {color:'green'}}, // may have prototype if instance not defined
	product: function(value){ /* 'instanceof' new ;) */
		return false;
	},
	exec:function(){},
	init:function(data, prototype){},
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
});

r = new p();

print.line(r.color);

//p.food = ['pear','apple','pineapple','pie']

//Object.defineProperty(p, "syrup", 
//	{value:['maple','sugar free','caramel','cherry','strawberry'],enumerable:true}
//);

//for (name in p) value = p[name], print.line(name, ": ",
//	(Array.prototype.isPrototypeOf(value)) ? value.join(', ') : value
//);

