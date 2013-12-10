#!bin/jse

sys.object.bitmap = function(keys) {
	var index; for (index in keys) this[keys[index]] = (1 << index);
}; sys.object.bitmap.prototype = Object.defineProperty({}, "constructor", {value:sys.object.bitmap});

code = new sys.object.bitmap([
	'const', 'integer', 'unsigned',
	'struct', 'union',  'utf',
	'void', 'boolean', 'char',
	'short', 'int', 'long',
	'size', 'pointer', 'int64',
	'float', 'double'
]);

for (name in code) print.line("name: "+name, " value: "+code[name]);
