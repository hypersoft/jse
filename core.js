#!bin/jse

(function () {

JSNative.List = function(){};

function construct() {
	echo('new list constructed!');
}

function get(name) {
	echo('property requested:', name);
	return null;
}

function set(name, value) {
	return false;
}

function enumerate() {
	echo('native list enumerated!');
	return ['cat','tail'];
}

function instance() {
	echo('instance of list called!');
}

JSNative.List = new JSNative.Class("JSNative.List", {}, {
	construct:{value:construct},
	get:{value:get}, set:{value:set},
	enumerate:{value:enumerate},
//	instanceof:{value:instance},
})

})();

var x = new JSNative.List();
new x()
x instanceof JSNative.List;
echo(typeof x);
for (name in x) echo(name);
echo(x.constructor.name);

