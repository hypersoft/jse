
var Address = js.extendPrototype(function Address(v, t){
	var o = js.native.instance(Address.container, 0);
	js.native.setPrototype(o, Address.prototype);
	if (t != undefined) o.type = (typeof t == 'string')?js.type[t]:t;
	if (v != undefined) o.address = parseInt(v);
	return o;
}, {
	container: null, name: "Address", prototype: {
		'&': null, 'value': null, type: js.type.void, address: 0
	},
	convert: function(type) {
		return this.address;
	},
	set: function(name, value) {
		if (!isNaN(p = parseInt(name))) {
			return js.native.address.write(this.address + (p * this.size), this.type, this.value);
		}
		if (name == 'value') return js.native.address.write(this.address, this.type, value);
		if (name == '&') return this.address = value;
		if (name == 'type') {
			this.type = (typeof value == 'string')?js.type[value]:value;
			this.size = js.native.typeSize(this.type);
		}
		return null;
	},
	get: function(name) {
		if (name == 'value') return js.native.address.read(this.address, this.type);
		if (name == '&') return this.address;
		if ((index = name.match(/\&([0-9]+)/)) != null) {
			index = index[1];
			return (this.address + (index * this.size));
		}
		if (!isNaN(i = parseInt(name))) {
			return js.native.address.read((this.address + (i * this.size)), this.type);
		}
		return null;
	}
})

Address.prototype.constructor = Address;
Address.container = js.native.container(Address);

/* ------------ */

// print out arguments
var echo = new Command('echo');
var i, argv = Address(js.run.argv, "utf8 *");
for (i = 0; i < js.run.argc; i++) echo(js.native.fromUTF8(argv[i]));

