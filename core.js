#!bin/jse

(function () {

function construct() {

}

function get() {
	return null;
}

function set() {
	return false;
}

exit(0);

JSNative.List = new JSNative.Class("JSNative.List", {}, {
	construct:{value:construct}, get:{value:get}, set:{value:set},
}, JSNative.Class)

})();


