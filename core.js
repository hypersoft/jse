#!bin/jse

JSNative.Tokenizer = function(getToken) {
	if (getToken) this.callback = getToken,
	this.syntax = new Array();
}

JSNative.Tokenizer.Expression = function(operationClass, elementName, selector) {
	this.expression = operationClass;
	if (elementName) this.name = elementName; // optional, if not present, "elementName == scanned data"
	if (selector) {
		// numbers are a string slice
		if (typeof selector == 'number') this.readAhead = selector
		// strings are a single case indexOf search
		else if (typeof selector == 'string') this.seekAhead = selector
		// regexps are a single char by expression match.
		else if (classOf(selector) == 'RegExp') this.scanAhead = selector;
	}
}

JSNative.Tokenizer.prototype = {
	constructor: JSNative.Tokenizer,
	callback:function(){
		this.element = this.getChar(); this.scanned = this.element
	}, scanned:'', element:'', index:0, token:'', source:'',
	syntax:[], matchReadAhead: 4, failed:"Failed to tokenize stream",
	getChar:function getChar() {return this.source[this.index++]},
	unGetChar:function unGetChar() {this.index--},
	advance:function advance(len) {this.index += len},
	rewind:function rewind(len) {this.index += len},
	readString:function peek(len) {return this.source.slice(this.index, (len == 0)?undefined:this.index + len)},
	wordPeek:function wordPeek(count) {
		var sentence = this.source.slice(this.index);
		var wpk = new RegExp('^(\\s*\\w+\\s*){'+count+'}')
		if ((words = wpk.exec(sentence)) != null) return words[0];
	},
	reset:function(source){
		this.scanned = '', this.element = '', this.index = 0, this.token = '',
		this.matchReadAhead = 4, this.source = (source)?source:'';
		delete this.subExpression;
	},
	load:function(source){this.reset(source)},
	recognize:function(regularExpression, name, readAhead){
		this.syntax.push(new JSNative.Tokenizer.Expression(regularExpression, name, readAhead))
	},
	knownToken:function(){
		while (this.source[this.index] && this.source[this.index].match(/\s/) != null) this.index++;
		if (this.source[this.index] === undefined) { this.getToken(); return }
		var source = this.readString(this.matchReadAhead);
		var syntax = this.syntax;
		for (rule in syntax) {
			var name = syntax[rule].name;
			var expression = syntax[rule].expression, match, data;
			var readAhead = syntax[rule].readAhead;
			if (readAhead != undefined) {
				data = this.readString(readAhead);
			} else {
				var seekAhead = syntax[rule].seekAhead;
				if (seekAhead != undefined) {
					var sought = this.source.indexOf(seekAhead, this.index);
					if (sought == -1) continue; // you missed...
					if (sought == this.index) continue; // this ain't no 'token'
					data = this.source.slice(this.index, sought);
				} else {
					var scanAhead = syntax[rule].scanAhead;
					if (scanAhead != undefined) {
						var x, txt;
						for (x = this.index; x <= this.source.length; x++) {
							txt = this.source[x];
							if (txt === undefined) txt = '';
							if (txt.match(scanAhead)) {
								if (x == this.index) break;
								data = this.source.slice(this.index, x);
								break;
							}
						}
					}
				}
			}
			delete this.subExpression;
			if (classOf(expression) == 'RegExp') { // when regexp we match data
				if (data) match.exec(data); else match.exec(source);
				if (match != null) {
					this.advance((this.scanned = match.shift()).length);
					if (match.length) this.subExpression = match;
					this.element = (name)?name:this.scanned;
					return 1;
				}
			} else if (classOf(expression) == 'Object') { // when object we match keys
				if (!data) data = source; var key
				for (key in expression) if (key == data) {
					this.scanned = key; this.advance(this.scanned.length);
					this.element = (name)?name:this.scanned;
					return 1;				
				}
			} else if (classOf(expression) == 'Array') { // when array we match index data
				if (!data) data = source; var index
				for (index in expression) if (expression[index] == data) {
					this.scanned = expression[index]; this.advance(this.scanned.length);
					this.element = (name)?name:this.scanned;
					return 1;				
				}
			} // might want to error out here..
		}
		return 0;
	},
	getToken:function() {
		if (this.index >= this.source.length) {
			delete this.subExpression; this.scanned = null, this.element = null; return;
			// leave this.token and this.expression alone, they were set by this.accept
		}
		// if we have rules to match attempt a successful match, else if we have a callback, call it
		if (this.syntax.length) { if (! this.knownToken() ) 
			this.callback()
		} else this.callback()
	},
	accept:function accept(elementName) {
		if (this.element == elementName) {
			this.token = (this.scanned)?this.scanned:this.element;
			this.expression = this.subExpression;
			this.getToken(); return 1;
		}
		return 0;
	},
	expect:function expect(elementName) {
		if (this.accept(elementName)) return this.token;
		throw new SyntaxError(this.failed+" at column "+this.index+": expected `"+elementName+"', found `"+this.element+"'");
	},
}

JSNative.api.typeVoid = 2,
JSNative.api.typeBoolean = 4,
JSNative.api.typeChar = 8,
JSNative.api.typeShort = 16,
JSNative.api.typeInt = 32,
JSNative.api.typeLong = 64,
JSNative.api.typeLongLong = 128,
JSNative.api.typeFloat = 256,
JSNative.api.typeDouble = 512,
JSNative.api.typeFunction = 1024,
JSNative.api.typeStruct = 2048,
JSNative.api.typeUnion = 4096;


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

Object.defineProperty(JSNative.Type, "define", {value: function(typeCode, name, constant, indirection, dimensions) {

	var type = {};
	type.constructor = JSNative.Type

	type.code = typeCode;
	type.name = name;
	type.const = (constant)?constant:false;

	if (indirection) type.indirection = indirection; // how many *'s to deref?
	if (dimensions) type.dimensions = dimensions; // array dimensions?

	if (typeCode == JSNative.api.typeFunction) {
		type.value = arguments[5];
		type.arguments = arguments[6];
	} else if (typeCode == JSNative.api.typeStruct) {
		// not supported...
		// If we supported structs and unions, arguments[5] would hold the identifier list..
	} else if (typeCode == JSNative.api.typeUnion) {
		// not supported...
		// If we supported structs and unions, arguments[5] would hold the identifier list..
	} else {
		type.size = JSNative.api.getTypeSize(typeCode);
		if (typeCode > JSNative.api.typeBoolean && typeCode < JSNative.api.typeFloat) {
			type.integer = true;
			type.signed = (arguments[5] !== undefined)?arguments[5]:true;
		} else type.integer = false;
	}

	Object.seal(type)
	Object.defineProperty(JSNative.Type, typeCode, {value:type});
	Object.defineProperty(JSNative.Type, name, {value:type, enumerable:true});

}})

// Initialize Core Types
JSNative.Type.define(JSNative.api.typeVoid,		"void")
JSNative.Type.define(JSNative.api.typeBoolean,	"bool")
// chars is not signed
JSNative.Type.define(JSNative.api.typeChar,		"char", undefined, undefined, undefined, false)
JSNative.Type.define(JSNative.api.typeShort,	"short")
JSNative.Type.define(JSNative.api.typeShort,	"short int")
JSNative.Type.define(JSNative.api.typeShort,	"int short")
JSNative.Type.define(JSNative.api.typeInt,		"int")
JSNative.Type.define(JSNative.api.typeInt,		"signed")
// unsigned is not signed
JSNative.Type.define(JSNative.api.typeInt,		"unsigned", undefined, undefined, undefined, false)
JSNative.Type.define(JSNative.api.typeLong,		"long")
JSNative.Type.define(JSNative.api.typeLong,		"long int")
JSNative.Type.define(JSNative.api.typeLong,		"int long")
JSNative.Type.define(JSNative.api.typeLongLong,	"long long")
JSNative.Type.define(JSNative.api.typeLongLong,	"long long int")
JSNative.Type.define(JSNative.api.typeLongLong,	"int long long")
JSNative.Type.define(JSNative.api.typeLongLong,	"long double")
JSNative.Type.define(JSNative.api.typeFloat,	"float")
JSNative.Type.define(JSNative.api.typeDouble,	"double")

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
	assign:'=',
	
	storage:'storage class', qualifier:'type qualifier', type:'type specifier',
	identifier:'variant identifier', number:'constant number', ellipsis:'ellipsis',

	termination:"end of statement"

}

var storageClasses = {
	auto:true, register:true, 'static':true, extern:true, typedef:true
}

var typeQualifiers = { 'const':true, volatile:true }

var typeSpecifiers = {
	'void':true, char:true, short:true, int:true, long:true,
	float:true, double:true, signed:true, unsigned:true,
	struct:true, union:true, 'enum':true
}

var parse = function parse(source) {

	// It would be nice to pack the source, but that creates error reporting issues...

	var element, charPosition = 0, token, accepted;

	var types = JSNative.Type;

	var typeQualifier = { // entirely technically uncorrect, far easier parsing though..

		'const':true,'volatile':true,'auto':true,'static':true,'extern':true,
		'register':true,'typedef':true,'signed':true,'unsigned':true

	};

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
			if (accepted in typeQualifier) {
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
			if (token == 'unsigned') { this.signed = false; continue; }
			else this[token] = true;
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


//for (name in JSNative.Type) echo(name)
//result = JSNative.Type.parse('const const char const unsigned * name')
//echo(JSON.stringify(result, undefined, '....'))
//echo(JSON.stringify(JSNative.Type.unsigned, undefined, '....'))
tokenizer = new JSNative.Tokenizer();
tokenizer.recognize({'one':true,'two':true}, "word number", /;|\W|^$/);
//tokenizer.recognize(/^;/i, "end of statement");
tokenizer.load("one"); tokenizer.getToken();
tokenizer.expect('word number');
echo('value = "'+tokenizer.token+'"')
//echo(tokenizer.expect(';'));
tokenizer.expect(null)
//echo(classOf(/\b/))
//JSNative.Type.parse('char (data);')
//JSNative.Type.parse('char foo[];')
//JSNative.Type.parse('char *(foo);')
//JSNative.Type.parse('char *(*foo)();')
//JSNative.Type.parse('char (*(*x())[][13])();')

