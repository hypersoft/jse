
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
api.classAccessor = api.classSet | api.classGet;

(function(){var $api = api; JSNative = { api:$api }; delete api})();

