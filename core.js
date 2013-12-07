#!bin/jse

p = sys.object.create({
	value:{keep:{}},
	get: function(item){
		return this.keep[item] || null;
	},
	set: function(item, value){
		print.line("setting ", item);
		this.keep[item] = value; return true
	},
	enumerate: function(data) {return Object.keys(this.keep)},
});

Object.defineProperties(p, {a:{value:1,enumerable:true}});

//p.food = "sammich"

print.line(p.a)
for (name in p) print.line(name);
