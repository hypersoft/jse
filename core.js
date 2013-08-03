#!bin/jse


//for (name in JSNative.Type) if (isNaN(name) && JSNative.Type[name].constructor == JSNative.Type) echo(name)
result = JSNative.Type.parse('char foo(char[4], char[][4][4][2]), data;')
echo(JSON.stringify(result, undefined, '....'))

//JSNative.Type.parse('char (data);')
//JSNative.Type.parse('char foo[];')
//JSNative.Type.parse('char *(foo);')
//JSNative.Type.parse('char *(*foo)();')
//JSNative.Type.parse('char (*(*x())[][13])();')

