
var Address = js.extendPrototype(function Address(v, t){
	var o = js.native.instance(Address.container, 0);
	js.native.setPrototype(o, Address.prototype);
	if (t != undefined) o.type = (typeof t == 'string')?js.type[t]:t;
	if (v != undefined) o['*'] = parseInt(v);
	return o;
}, {
	container: null, name: "Address", prototype: {
		'&': null, 'value': null, type: js.type.void, address: 0
	},
	set: function(name, value) {
		if (!isNaN(i = parseInt(name))) {
			return js.native.address.write(this['*'] + (i * this.size), this.type, this.value);
		}
		if (name == 'type') {
			this.type = (typeof value == 'string')?js.type[value]:value;
			this.size = js.native.typeSize(this.type);
		}
		return null;
	},
	get: function(name) {
		if (!isNaN(i = parseInt(name))) {
			return js.native.address.read((this['*'] + (i * this.size)), this.type);
		}
		var index = name.match(/^\&([0-9]+)$/);
		if (index != null) {
			index = index[1];
			return (this['*'] + (index * this.size));
		}
		return null;
	}
})

Address.prototype.constructor = Address;
Address.container = js.native.container(Address);

/* ------------ */

// print out arguments
var argv = Address(js.run.argv, "utf8 *");
var i, puts = new Procedure("jse", "puts", "native", ["int", "utf8 *"]);
for (i = 0; i < js.run.argc; i++) puts(argv[i]);

