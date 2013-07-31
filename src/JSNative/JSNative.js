
// conversion functions return this constant
api.failedToConvert = Object.freeze({converted:false});

api.classInitialize = 2,
api.classConstruct = 4,
api.classInvoke = 8,
api.classGet = 16,
api.classSet = 32,
api.classDelete = 64,
api.classConvert = 128,
api.classInstanceOf = 256,
api.classEnumerate = 512,
api.classHasProperty = 1024,
api.classAccessor = api.classSet | api.classGet;


(function(){var $api = api; JSNative = { api:$api }; delete api})();

JSNative.api.classFlags = function classFlags(methods) {
	var name, flags;
	for (name in methods) {
		if (name == 'classInitialize') { flags |= JSNative.api.classInitialize; continue }
		if (name == 'classConstruct') { flags |= JSNative.api.classConstruct; continue }
		if (name == 'classInvoke') { flags |= JSNative.api.classInvoke; continue }
		if (name == 'classGet') { flags |= JSNative.api.classGet; continue }
		if (name == 'classSet') { flags |= JSNative.api.classSet; continue }
		if (name == 'classDelete') { flags |= JSNative.api.classDelete; continue }
		if (name == 'classConvert') { flags |= JSNative.api.classConvert; continue }
		if (name == 'classEnumerate') { flags |= JSNative.api.classEnumerate; continue }
		if (name == 'classHasProperty') { flags |= JSNative.api.classHasProperty; continue }
		if (name == 'classInstanceOf') { flags |= JSNative.api.classInstanceOf; continue }
		if (name == 'classAccessor') { flags |= JSNative.api.classAccessor; continue }
	};
	return flags;
}

