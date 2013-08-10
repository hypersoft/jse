#!bin/jse

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
// char is not signed
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

/* C Declaration Grammar Reference Notes

	copied from "The C Programming Language: Second Editon" pp. 169-174

	declaration:
	declaration-specifiers init-declarator-list?;

	declaration-specifiers:
	storage-class-specifier declaration-specifiers?
	type-specifier declaration-specifiers?
	type-qualifier declaration-specifiers?

	init-declarator-list:
	init-declarator
	init-declarator-list , init-declarator

	init-declarator:
	declarator
	declarator = initializer

	storage-class specifier:
	auto
	register
	static
	extern
	typedef

	type specifier:
	void
	char
	short
	int
	long
	float
	double
	signed
	unsigned
	struct-or-union-specifier
	enum-specifier
	typedef-name

	type-qualifier:
	const
	volatile

	declarator:
	pointer? direct-declarator

	direct-declarator:
	identifier
	(declarator)
	direct-declarator [ constant-expression? ]
	direct-declarator ( parameter-type-list )
	direct-declarator ( identifier-list? )

	parameter-type-list:
	parameter-list
	parameter-list , ...

	parameter-list:
	parameter-declaration
	parameter-list , parameter-declaration

	parameter-declaration:
	declaration-specifiers declarator
	declaration-specifiers abstract-declarator?

	pointer:
	* type-qualifier-list?
	* type-qualifier-list? pointer

	type-qualifier-list:
	type-qualifier
	type-qualifier-list type-qualifier

*/

/* cut: Struct, Union, Enum Stuff	NOT CURRENTLY AVAILABLE

	struct-or-union-specifier:
	struct-or-union identifier? { struct-declaration-list }
	struct-or-union identifier

	struct-or-union:
	struct
	union

	struct-declaration-list:
	struct declaration
	struct-declaration-list struct declaration

	struct-declaration:
	specifier-qualifier-list struct-declarator-list;

	specifier-qualifier-list:
	type-specifier specifier-qualifier-list?
	type-qualifier specifier-qualifier-list?

	struct-declarator-list:
	struct-declarator
	struct-declarator-list , struct-declarator

	struct-declarator:
	declarator declarator? : constant-expression

	enum-specifier:
	enum identifier? { enumerator-list }
	enum identifier

	enumerator-list:
	enumerator
	enumerator-list , enumerator

	enumerator:
	identifier
	identifier = constant-expression

*/

function Declaration() {}

Declaration.syntax = {

	lparen:'(',		rparen:')',
	lbracket:'[',	rbracket:']',
	lbrace:'{',		rbrace:'}',
	asterisk:'*',	comma:',',
	assign:'=',		semicolon:';',
	
	identifier:'variant identifier', number:'constant number', ellipsis:'ellipsis',

	termination:"end of statement",

	storage:'storage class', storageClass: {
		auto:true, register:true, 'static':true, extern:true, typedef:true
	},

	qualifier:'type qualifier', typeQualifier: {
		'const':true, volatile:true
	},

	type:'type specifier', typeSpecifier: { // not used -/-/--> JSNative.Type[]
		'void':true, char:true, short:true, int:true, long:true,
		float:true, double:true, signed:true, unsigned:true,
		struct:true, union:true, 'enum':true
	}

}

Declaration.parse = function(source) {

	if (typeof source == 'string') this.source = source
	else throw new TypeError('unable to parse declaration: expected source type string')
	if (source.length == 0) throw new SyntaxError('unable to parse declaration: source data empty')

	var syntax = Declaration.syntax, tokenizer = new JSNative.Tokenizer();
	tokenizer.originalCallback = tokenizer.callback; // we'll use this to init our callback...
	tokenizer.load(source);

	var selectWord = /\W|^$/

	tokenizer.recognize(/[\(\)\[\]\{\}\,\*\=]/)
	tokenizer.recognize(syntax.storageClass, syntax.storage, selectWord)
	tokenizer.recognize(syntax.typeQualifier, syntax.qualifier, selectWord)
	tokenizer.recognize(/\.\.\./, syntax.ellipsis, 3)
	tokenizer.recognize(/^([0-9]+)(UL|U|L)?/i, syntax.number, /\s|;|\]|^$/)
	tokenizer.recognize(['struct','union','enum'], syntax.type, selectWord)

	tokenizer.callback = function() {
		tokenizer.originalCallback();
		if (this.element.match(/[a-z_]/i) != null) {
			tokenizer.unGetChar();
			var c = 1, words, ok;
			while ((words = this.wordPeek(c)) && words && words.trim() in JSNative.Type) { 
				ok = words; c++;
			}
			if (ok) {
				this.advance(ok.length);
				this.element = syntax.type; this.scanned = ok.trim();
			} else {
				this.advance(words.length);
				this.element = syntax.identifier; this.scanned = words.trim();
			}
		}
	}

	var anonymousDeclarator = false; // when true, directDeclarator does not require names..
	var declaration = {};

	// CSV list handler
	list = function(name, processor) {
		this[name] = new Array();
		do { var unit = new Object(); processor.call(unit); this[name].push(unit);
		} while (tokenizer.accept(syntax.comma))
	},

	declarationSpecifiers = function(){
		if (tokenizer.accept(syntax.storage)) {
			storageClassSpecifier.call(this); declarationSpecifiers.call(this);
		} else if (tokenizer.accept(syntax.qualifier)) {
			typeQualifier.call(this); declarationSpecifiers.call(this);
		} else if (tokenizer.accept(syntax.type)) {
			typeSpecifier.call(this); declarationSpecifiers.call(this);
		}
	},

	initDeclarator = function(){
		declarator.call(this);
		if (tokenizer.accept(syntax.assign)) {
			throw new Error("declarator initializers are not supported")
		}
	},

	initDeclaratorList = function(){ list.call(this, 'declarators', initDeclarator) },

	storageClassSpecifier = function(){
		/* At most one storage class specifier may be given in a declaration. */
		if ('storage' in this) throw new Error("multiple storage class specifiers in declaration")
		this.storage = tokenizer.token
	},

	typeQualifier = function(){
		this[tokenizer.token] = true
	},

	typeQualifierList = function(){
		while (tokenizer.accept(syntax.qualifier)) typeQualifier.call(this);
	},

	typeSpecifier = function(){
		if (this.type == undefined) this.type = new Object();
		if (tokenizer.token.match(/struct|union|enum/) != null) throw new ReferenceError("`"+tokenizer.token+"' is not an acceptable type specifier in this release")
		else if (tokenizer.token.match(/^signed$|^unsigned$/) != null) {
			this.type[tokenizer.token] = true
			// to do: validate ! signed && unsigned
		}
		this.type.reference = tokenizer.token;
	},

	parameterDeclaration = function(){
		declarationSpecifiers.call(this);
		// todo: validate those specifiers for parameter context
		declarator.call(this);
	},

	parameterList = function(){},

	parameterTypeList = function(){
		anonymousDeclarator = true; 	// enable 'abstract' declarator
		this.arguments = new Array();
		do { var unit = new Object();
			if (tokenizer.accept(syntax.ellipsis)) {
				// has to be n+1 ?
				this.from = tokenizer.token
			} else parameterDeclaration.call(unit);
			this.arguments.push(unit);
		} while (tokenizer.accept(syntax.comma))
		anonymousDeclarator = false;
	},

	declarator = function(){
		pointer.call(this); directDeclarator.call(this);
	},

	directDeclarator = function(){
		if (tokenizer.accept(syntax.identifier)) {
			this.from = 'variant'
			identifier.call(this);
		} else if (tokenizer.accept(syntax.lparen)) {
			var d = new declarator();
			if (this.name === undefined && (d.pointer === undefined )) extend(this, d);
			else this.declarator = d;
			tokenizer.expect(syntax.rparen);
		} else if (tokenizer.accept(syntax.termination)) {
			throw new SyntaxError("Failed to parse native declarator: useless type name in empty declaration");
		} else if (anonymousDeclarator == false) {
			throw new SyntaxError("Failed to parse native declarator: expected `*', identifier, or `(' before `"+tokenizer.element+"' token");
		}

		if (tokenizer.accept(syntax.lbracket)) {
			this.from = 'array'
			this.dimensions = new Array();
			if (tokenizer.accept(syntax.rbracket)) {
				this.dimensions.push('');
			} else if (tokenizer.accept(syntax.number)) {
				this.dimensions.push(tokenizer.token)
				tokenizer.expect(syntax.rbracket)
			}
			while (tokenizer.accept(syntax.lbracket)) {
				if (tokenizer.accept(syntax.number)) {
					this.dimensions.push(tokenizer.token);
					tokenizer.expect(syntax.rbracket)
				}
			}			
		} else if (tokenizer.accept(syntax.lparen)) {
			this.from = 'function'
			parameterTypeList.call(this);
			tokenizer.expect(syntax.rparen);
		} else {
			if (this.from === undefined) {
				var x = this.declarator; delete this.declarator;
				extend(this, x);
			}
		}
	},

	pointer = function(){
		if (tokenizer.accept(syntax.asterisk)) {
			for (this.pointer = 1; tokenizer.accept(syntax.asterisk); this.pointer++);
			typeQualifierList.call(this);
			pointer.call(this);
		}
	},

	identifier = function(){
		this.name = tokenizer.token;
	},

	identifierList = function(){}, // what the fuck is this for?

	constantExpression = function(){}; // we took a shortcut in directDeclarator

	tokenizer.getToken();
		declarationSpecifiers.call(declaration);
		initDeclaratorList.call(declaration);
	tokenizer.expect(syntax.semicolon);

	return declaration;

}

var dcl = Declaration.parse('typedef char (*(*x(void))[14u])(void);');

echo(JSON.stringify(dcl))

exit(0);

