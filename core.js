#!bin/jse

p = sys.object.create({
	init:function(prototype, extern){this.keep = {}},
	exec:function(){}, new:function(){},
	get: function(item){return this.keep[item] || null},
	set: function(item, value){this.keep[item] = value; return true},
	delete: function(item){delete this.keep[item]; return true},
	enumerate:function(){return Object.keys(this.keep)},
});

Object.defineProperties(p, {apple:{value:1,enumerable:true}});

p.food = ['sammich','apple','banana']

for (name in p) print.line(name);

new p()


