#!bin/jse

// tokenizer needs a SyntaxError method that automatically reports stream position.

/* JS NATIVE TYPE */ (function JSNativeTypeClass() {

/* new JSNative.Type() == Typedef */
var classConstruct = function(reference, name){
	var refType = typeof reference;
	if (refType == 'string') {
		// parse that into a type code
	} else if (refType == 'number') {
		this.value = reference;
	} else if (refType == 'object') {
		
	} else {
		throw new TypeError("unhandled reference type model");
	}
}

var classInvoke = function(code) {return this.Type[code]}

new JSNative.Class
(
	"JSNative.Type",
	{ /* class instance methods (constructor prototype) */
		toString: function() {return JSNative.Type.def.toString(this.value) },
		valueOf: function() { return this.value },
		sizeOf: function() {  return JSNative.api.getTypeSize(this.value) },
	},
	{ /* class methods (constructor methods) */
		classConstruct:classConstruct,
		classInvoke:classInvoke,
		def: {
			flag: { // these flags mix with type codes
				const: 2,
				volatile: 65536,	// typically ignored operationally, but the value has weight
				storage: { 	
					// storage is a native value property, each flag is unique for exactness
					auto: 2,
					register: 4,
					'static': 8,
					extern: 16,
					typedef: 32,
					isAuto: function(code){ return Boolean(this.storage.auto & code) },
					isRegister: function(code){ return Boolean(this.storage.register & code) },
					isStatic: function(code){ return Boolean(this.storage.static & code) },
					isExtern: function(code){ return Boolean(this.storage.extern & code) },
					isTypedef: function(code){ return Boolean(this.storage.typedef & code) },
				}
			}, // the following values determine how we interpret a memory address value
			/* pointer, enum, function, struct, union are not significant therfore they are aliases */
			unsigned: 4,
			void: 8,		// each type is an object with a pointer, so this means void *
			boolean: 16,
			char: 32 | 4,	// chars are default unsigned
			short: 64,
			int: 128,
			long: 256,
			int64: 512,
			'long long': 512,
			float: 1024,
			double: 2048,
			/* our version of dyncall doesn't support long double /*
			/*Type.def.*/toString: function(code) {
				// generate a human readable string from full type code

			},
			isUnsigned: function(code){ return Boolean(this.def.unsigned & code) },
			isSigned: function(code){ return (! Boolean(this.def.unsigned & code)) },
			isConst: function(code){ return Boolean(this.def.flag.const & code) },
			isVolatile: function(code){ return Boolean(this.def.flag.volatile & code) },
			sizeOf: JSNative.api.getTypeSize.bind,
		},
	}

)

Object.seal(JSNative.Type.def.flag.storage);
Object.seal(JSNative.Type.def.flag);
Object.seal(JSNative.Type.def);

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

var Declaration = function() {}

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

	tokenizer.recognize(/^([0-9]+)(UL|U|L)?/i, syntax.number, /[^\dul]|^$/i)
	/* need to add more of the above for other number systems */

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
	var access = 'variant'

	var declaration = {};

	// CSV list handler
	list = function(name, processor) {
		this[name] = new Array();
		do { var unit = new Object(); processor.call(unit); this[name].push(unit);
		} while (tokenizer.accept(syntax.comma))
	},

	declarationSpecifiers = function(){
		if (this.type == undefined) this.type = new Object();
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
		if (this.storage == 'typedef') access = 'type'
	},

	typeQualifier = function(){
		this.type[tokenizer.token] = true
	},

	typeQualifierList = function(){
		while (tokenizer.accept(syntax.qualifier)) typeQualifier.call(this);
	},

	typeSpecifier = function(){
		if (tokenizer.token.match(/struct|union|enum/) != null) throw new ReferenceError("`"+tokenizer.token+"' is not an acceptable type specifier in this release")
		else if (tokenizer.token.match(/^signed$|^unsigned$/) != null) {
			if (tokenizer.token in this.type) {
				throw new SyntaxError("duplicate '"+tokenizer.token+"'");
			}
			this.type[tokenizer.token] = true
			if (this.type.signed == true && this.type.unsigned == true) {
				throw new SyntaxError("both 'signed' and 'unsigned' in declaration specifiers");
			}
			return
		}
		if (this.type.reference != undefined) {
				throw new SyntaxError("two or more data types in declaration specifiers");
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
				// must have a prior named argument
				if (this.arguments.length == 0 || this.arguments[0].name == undefined) 
					throw new SyntaxError("ISO C requires a named argument before '...'");
				// ellipsis requires special handling
				unit.from = 'list'; identifier.call(unit)
				// ellispis is the last acceptable argument.
				this.arguments.push(unit)
				break;
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
			this.from = access;
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

	identifierList = function(){}, // Q: what the fuck is this for? A: procedure call

	constantExpression = function(){}; // we took a shortcut in directDeclarator

	tokenizer.getToken();
		declarationSpecifiers.call(declaration);
		initDeclaratorList.call(declaration);
	tokenizer.expect(syntax.semicolon);

	declaration.source = source;

	return declaration;

}

JSNative.Type.Declaration = Declaration;

})();

exit(0);

