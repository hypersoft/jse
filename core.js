#!bin/jse

p = sys.object.create(/* system object interface */{

	name: 'p', value: {},
	prototype: {crazy:"going ham"}, // self prototype
	new:function(){ 
		/*
			if you haven't defined an instance prototype, the value of this will be
			the instance of value, instead of the instance of prototype
		*/
	},
	instance: {color:'blue'}, // new.prototype
	product: function(value){ /* 'instanceof' new */
		/* the value of 'this' is the new constructor */
		return this.prototype.isPrototypeOf(value)
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

// instanceof test
//r = new p();
//(r instanceof p) && print.line(true)

p.food = ['pear','apple','pineapple','pie']

Object.defineProperty(p, "syrup", 
	{value:['maple','sugar free','caramel','cherry','strawberry'],enumerable:true}
);

for (name in p) value = p[name], print.line(name, ": ",
	(Array.prototype.isPrototypeOf(value)) ? value.join(', ') : value
);

