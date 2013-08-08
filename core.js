#!bin/jse

JSNative.api.typeVoid = 2,
JSNative.api.typeBoolean = 4,
JSNative.api.typeChar = 8,
JSNative.api.typeShort = 16,
JSNative.api.typeInt = 32,
JSNative.api.typeLong = 64,
JSNative.api.typeLongLong = 128,
JSNative.api.typeFloat = 256,
JSNative.api.typeDouble = 512,
JSNative.api.typePointer = 1024,
JSNative.api.typeFunction = 2048;


// current type system does not do anything! :(
// need methods to extract types from parsed sources, or define types,
// as parsed sources...

/* parsing algo needs to meet the following exception models
//unsigned bool t1; //  error: both ‘unsigned’ and ‘_Bool’ in declaration specifiers
//int bool t1; // error: two or more data types in declaration specifiers
//signed unsigned char t2; // error: both ‘signed’ and ‘unsigned’ in declaration specifiers
//signed signed char t2; // error: duplicate ‘signed’
*/

/* JS NATIVE TYPE */ (function JSNativeTypeClass() {


var classConstruct = function(code, name){



}

var classInvoke = function(code) {return this.Type[code]}

new JSNative.Class
(
	"JSNative.Type",
	{ /* class instance methods (constructor prototype) */
		toString: function() {return this.name},
		valueOf: function() {return this.type},
	},
	{ /* class methods (constructor methods) */
		classConstruct:classConstruct,
		classInvoke:classInvoke,
	}
)


delete JSNative.Type.classConstruct
delete JSNative.Type.classInvoke

Object.defineProperties(JSNative.Type, {
	classConstruct:{value:classConstruct},
	classInvoke:{value:classInvoke},
})

Object.defineProperty(JSNative.Type, "defineCoreType", {value: function(typeCode, name) {

	var type = {};
	type.code = typeCode;
	type.name = name;
	type.const = false;
	type.size = JSNative.api.getTypeSize(typeCode);
	type.constructor = JSNative.Type

	if (typeCode <= JSNative.api.typeBoolean || typeCode > JSNative.api.typeLongLong) {
		type.integer = false;
	} else {
		type.integer = true;
		type.signed = true;
	}

	Object.seal(type)
	Object.defineProperty(JSNative.Type, typeCode, {value:type});
	Object.defineProperty(JSNative.Type, name, {value:type, enumerable:true});

	if (type.integer == true && typeCode > JSNative.api.typeChar) {
		if (typeCode != JSNative.api.typeInt && name.match(/\bint\b/) === null) {
			JSNative.Type.defineCoreType(typeCode, name+' int');
			JSNative.Type.defineCoreType(typeCode, 'int '+name);
		}
	}

}})

// Initialize Core Types
JSNative.Type.defineCoreType(JSNative.api.typeVoid,		"void")
JSNative.Type.defineCoreType(JSNative.api.typeBoolean,	"bool")
JSNative.Type.defineCoreType(JSNative.api.typeChar,		"char")
JSNative.Type.defineCoreType(JSNative.api.typeShort,	"short")
JSNative.Type.defineCoreType(JSNative.api.typeInt,		"int")
JSNative.Type.defineCoreType(JSNative.api.typeLong,		"long")
JSNative.Type.defineCoreType(JSNative.api.typeLongLong,	"long long")
JSNative.Type.defineCoreType(JSNative.api.typeFloat,	"float")
JSNative.Type.defineCoreType(JSNative.api.typeDouble,	"double")

})();

Object.defineProperty(JSNative.Type, "parse", {value: (function() {

/* C "dcl" grammar

	declarator:
		pointer? direct-declarator

	direct-declarator:
		identifier
		( declarator )
		direct-declarator [ constant-expression? ]
		direct-declarator ( identifier-list? )
		direct-declarator ( parameter-type-list )

	pointer:
		* type-qualifier-list?
		* type-qualifier-list? pointer

	type-qualifier-list:
		type-qualifier
		type-qualifier-list type-qualifier

Outsourced from
"The C Programming Language 2nd Ed." (Prentice Hall) pp. 101, 174

Modeled after: http://en.wikipedia.org/wiki/Recursive_descent_parser#C_implementation (July 31st, 2013)

Author: Triston J. Taylor pc.wiz.tt@gmail.com

*/

var syntax = {

	lparen:'(',		rparen:')',
	lbracket:'[',	rbracket:']',
	lbrace:'{',		rbrace:'}',
	asterisk:'*',	comma:',',

	qualifier:'qualifier', type:'type name', identifier:'identifier', number:'number', ellipsis:'ellipsis',

	termination:"end of statement"

}

var parse = function parse(source) {

	// It would be nice to pack the source, but that creates error reporting issues...

	var element, charPosition = 0, token, accepted;

	var types = JSNative.Type, qualifiers = {'const':true,'signed':true,'unsigned':true,'extern':true};

	var anonymousDeclarator = false;

	function getChar() {return source[charPosition++]}
	function unGetChar() {--charPosition}

	function advance(len) {charPosition += len}
	function peek(len) {return source.slice(charPosition, charPosition + len)}

	function wordPeek(count) { /* type check hack for getSyntax */
		var sentence = source.slice(charPosition);
		var wpk = new RegExp('^(\\s*\\w+\\s*){'+count+'}')
		if ((words = wpk.exec(sentence)) != null) return words[0];
	}

	function getSyntax() {
		var ok, c, words; accepted = ''
		while ((element = getChar()) !== undefined && element.match(/\s/) != null);
		if (element === undefined || element == ';') { element = syntax.termination; return; }
		if (element.match(/[\(\)\[\]{},\*]/) != null) return // we parse simple elements here, not constructs
		if (element.match(/[a-z_]/i) != null) {
			/* "Smart Ass Subversive Descent Parsing" -- pc.wiz.tt */
			for (accepted = element; (element = getChar()) != undefined && element.match(/[a-z0-9_]/i) != null; accepted += element);
			unGetChar();
			if (accepted in qualifiers) {
				// we used to collect words here...
				//for(c = 1; (words = accepted + wordPeek(c)).replace(/\s+/gm, ' ').trim().split(' ').pop() in qualifiers; ok = words, c++);
				element = syntax.qualifier;
			} else if (accepted in types && types[accepted].constructor == types) {
				// the constructor of each type, must also point to the registry.
				for(c = 1; (words = accepted + wordPeek(c)).replace(/\s+/gm, ' ').trim() in types; ok = words, c++);
				element = syntax.type;
			} else element = syntax.name;
			if (ok) { advance(ok.length - accepted.length); accepted = ok.trim() }; return
		} else if (element.match(/[0-9]/) != null) {
			for (accepted = element; (element = getChar()) != undefined && element.match(/[0-9]/) != null; accepted += element);
			unGetChar(); element = syntax.number; return
		} else if (element == '.' && peek(2) == '..') {
			advance(2); accepted = '...'; element = syntax.ellipsis; return
		} else throw new SyntaxError("Failed to parse native declarator: useless declarator character `"+element+"' in parse stream at column "+charPosition);
	}

	function accept(s) {
		if (element == s) {
			token = (accepted)?accepted:element; // No, really... its okay to use it now...
			getSyntax(); return 1;
		}
		return 0;
	}
 
	function expect(s) {
		if (accept(s)) return token;
		throw new SyntaxError("Failed to parse native declarator at column "+charPosition+": expected `"+s+"', found `"+element+"'");
	}

	function identifier() {
		this.name = token;
	}

	function typeQualifierList() {
		while (accept(syntax.qualifier)) {
			if (token == 'const') { this.const = true; continue; }
			if (! this.specifier) this.specifier = [];
			this.specifier.push(token);
		}
	}

	function pointer() {
		if (accept(syntax.asterisk)) {
			for(this.pointer = 1; accept(syntax.asterisk); this.pointer++);
		}
		typeQualifierList.call(this);
		if (element == syntax.asterisk) {
			pointer.call(this);
		}
	}

	function declarator() {
		pointer.call(this);
		directDeclarator.call(this);
	}

	function directDeclarator() {
		if (accept(syntax.name)) {
			this.from = 'variant'
			identifier.call(this);
		} else if (accept(syntax.lparen)) {
			var d = new declarator();
			if (this.name === undefined && (d.pointer === undefined )) extend(this, d);
			else this.declarator = d;
			expect(syntax.rparen);
		} else if (accept(syntax.termination)) {
			throw new SyntaxError("Failed to parse native declarator: useless type name in empty declaration");
		} else {
			if (element == syntax.comma && anonymousDeclarator == false)
			throw new SyntaxError("Failed to parse native declarator: expected `*', identifier, or `(' before `"+element+"' token");
		}

		if (accept(syntax.lbracket)) {
			this.from = 'array'
			this.dimensions = new Array();
			if (accept(syntax.rbracket)) {
				this.dimensions.push('');
			} else if (accept(syntax.number)) {
				this.dimensions.push(token)
				expect(syntax.rbracket)
			}
			while (accept(syntax.lbracket)) {
				if (accept(syntax.number)) {
					this.dimensions.push(token);
					expect(syntax.rbracket)
				}
			}			
		} else if (accept(syntax.lparen)) {
			this.from = 'function'
			this.arguments = new Array();
			argumentIdentiferList.call(this.arguments);
			expect(syntax.rparen);
		} else {
			if (this.from === undefined) {
				var x = this.declarator; delete this.declarator;
				extend(this, x);
			}
		}

	}

	function argumentIdentiferList() {
		var i = 0; anonymousDeclarator = true;
		while (accept(syntax.type)) {
			this[i] = {};
			this[i].expect = token;
			declarator.call(this[i++]);
			accept(syntax.comma)
		}
		anonymousDeclarator = false;
	}

	var dcls = new Array();
	dcls.source = source;
	function declaration() {
		this.expect = expectedType;
		extend(this, theseSpecs);
		declarator.call(this);
		dcls.push(this);
	}

	try { // we use internal functions, so we need to refactor any reference to an error
		getSyntax();
		var theseSpecs = {};
		typeQualifierList.call(theseSpecs);
		var expectedType = expect(syntax.type);
		do {
			if (accept(syntax.type)) { // That's an error
				throw new SyntaxError("Failed to parse native declarator: expected `*', identifier, or `(' before `"+token+"'");			
			}
			new declaration();
		} while (accept(syntax.comma));
		expect(syntax.termination)
	} catch(e) {
		throw new InvokeError("SyntaxError", e.message);
	}

	// convert to pseudo-array-object so we can JSONIFY source as well.
	var dclso = {}; extend(dclso, dcls); dclso.length = dcls.length;
	return dclso;

}

return parse;

})()})


for (name in JSNative.Type) echo(name)
result = JSNative.Type.parse('const const char const unsigned * name')
echo(JSON.stringify(result, undefined, '....'))

//JSNative.Type.parse('char (data);')
//JSNative.Type.parse('char foo[];')
//JSNative.Type.parse('char *(foo);')
//JSNative.Type.parse('char *(*foo)();')
//JSNative.Type.parse('char (*(*x())[][13])();')

