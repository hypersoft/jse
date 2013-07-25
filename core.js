#!bin/jse

(function () {

function construct() {
	echo('new list constructed!');
	Object.defineProperty(this, "constructor", {value:JSNative.List});
}

function get(name) {
	echo('property requested:', name);
	return null;
}

function set() {
	return false;
}

function enumerate() {
	echo('native list enumerated!');
	return ['cat','tail'];
}

JSNative.List = new JSNative.Class("JSNative.List", {}, {
	construct:{value:construct},
	get:{value:get}, set:{value:set},
	enumerate:{value:enumerate},
})

})();

x = new JSNative.List();
for (name in x) echo(name);
echo(x.constructor.name);

