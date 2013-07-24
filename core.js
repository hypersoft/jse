#!bin/jse


Reference = function Reference(o, p) {
	if (! Reference.prototype.isPrototypeOf(this)) {
		echo('called as a function');
		return;
	}
	if (! this.new) throw new TypeError('call Reference as a constructor') 
	this.new = undefined;
	this.object = o, this.property = p;
	Object.defineProperty(this, "constructor", Reference);
}

Object.defineProperties( Reference.prototype = {
	object:undefined,property:undefined,new:Reference.bind(),
	get:function(){return this.object[this.property];},
	set:function(value){this.object[this.property] = value;},
	enumerable:true,writeable:true,configurable:true
}, {
	descriptor:{get:function(){return Object.getOwnPropertyDescriptor(this.object, this.property)},enumerable:true,configurable:true},
	prototype:{get:function(){return Object.getPrototypeOf(this.object[this.property])},enumerable:true,configurable:true},
	clone:{get:function(){return Object.create(this.object[this.property])},enumerable:true,configurable:true},
	type:{get:function(){return typeof this.object[this.property]},enumerable:true,configurable:true},
}  )

Reference.getPrototypeProperty = function(name) {
	if (name == 'toString' || name == 'valueOf') return this.get.bind(this);
	return null;
}
Reference.setPrototypeProperty = function(name, value) {
	echo(name)
	//if (name == 'toString' || name == 'valueOf') return this.get.bind(this);
	return false;
}

Reference = JSNative.api.createClass(Reference, Reference.name,
	JSNative.api.classConstructor, JSNative.api.classAccessor
)

var person = {name:'me'}

x = new Reference(person, 'name')

x.bread = 'fed';

echo(x, '=', x.bread)


