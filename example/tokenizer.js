
// This is a demo of the tokenizer that comes with jse.
// it parses C declarations into a json object.

// Created by Triston J. Taylor (pc.wiz.tt@gmail.com)

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

	type:'type specifier', typeSpecifier: {
		'void':true, char:true, short:true, int:true, long:true,
		float:true, double:true, signed:true, unsigned:true,
		struct:true, union:true, 'enum':true
	}

}

Declaration.parse = function(source) {

	if (typeof source == 'string') this.source = source
	else throw new TypeError('unable to parse declaration: expected source type string')
	if (source.length == 0) throw new SyntaxError('unable to parse declaration: source data empty')

	var syntax = Declaration.syntax, tokenizer = new js.Tokenizer();
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
			while ((words = this.wordPeek(c)) && words && words.trim() in Declaration.syntax.typeSpecifier) { 
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
			if (this.type.signed == true && this.type.unsigned == true) {
				throw new SyntaxError("both 'signed' and 'unsigned' in declaration specifiers");
			}
			return
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
				/* has to be n+1
				test.c:3:11: error: ISO C requires a named argument before ‘...’ char f(...);
				*/
				if (this.arguments.length == 0 || this.arguments[0].name == undefined) 
					throw new SyntaxError("ISO C requires a named argument before '...'");
				identifier.call(unit)
				/*
				test.c:3: char crap(char g, ..., ...);
				22: error: expected ‘)’ before ‘,’ token
				*/
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
				js.extend(this, x);
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

var dcl = Declaration.parse('typedef char (*(*x(char g, ...))[14u])(void);');

new Command('echo', '-E')(JSON.stringify(dcl, undefined, '....'));
