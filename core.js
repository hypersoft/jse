#!bin/jse

JSNative.Type.parse = (function() {

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

var main, result;

var parse = function parse(source) {

	// It would be nice to pack the source, but that creates error reporting issues...

	var element, charPosition = 0, token, accepted;

	main = Object.create(parse.prototype); result = main;

	var depth = 0;
	var types = JSNative.Type, qualifiers = {'const':true,'static':true,'volatile':true,'extern':true};

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
				// these things need some validation against legality, tho' we don't use them in JSNI
				for(c = 1; (words = accepted + wordPeek(c)).replace(/\s+/gm, ' ').trim().split(' ').pop() in qualifiers; ok = words, c++);
				element = syntax.qualifier;
			} else if (accepted in types && types[accepted].constructor == types) {
				// type registry must be robust and stupid, containing all possible type designations
				// such as: char, signed char, char signed, unsigned char, char unsigned, etc...
				// the constructor of each type, must also point to the registry.
				for(c = 1; (words = accepted + wordPeek(c)).replace(/\s+/gm, ' ').trim() in types; ok = words, c++);
				element = syntax.type;
			} else element = syntax.identifier;
			if (ok) { advance(ok.length - accepted.length); accepted = ok.trim() }; return
		} else if (element.match(/[0-9]/) != null) {
			for (accepted = element; (element = getChar()) != undefined && element.match(/[0-9]/) != null; accepted += element);
			unGetChar(); element = syntax.number; return
		} else if (element == '.' && peek(2) == '..') {
			advance(2); accepted = '...'; element = syntax.ellipsis; return
		} else throw new SyntaxError("unused characters in parse stream at char "+charPosition);
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
		throw new SyntaxError("char "+charPosition+": expected '"+s+"', found '"+element+"'");
	}

	function identifier() {
		this.identifier = token;
	}

	function pointer() {
		if (accept(syntax.asterisk)) {
			for(this.pointer = 1; accept(syntax.asterisk); this.pointer++);
		} else delete this.pointer;
	}

	function declarator() {
		this.pointer = 0;
		pointer.call(this);
		directDeclarator.call(this);
	}

	function directDeclarator() {
		if (accept(syntax.identifier)) {
			this.from = 'variant'
			identifier.call(this);
		} else if (accept(syntax.lparen)) {
			this.declarator = new declarator();
			expect(syntax.rparen);
		} else if (accept(syntax.termination)) {
			throw new InvokeError("useless type name in empty declaration");
		} else {
			throw new InvokeError("expected identifier, ‘*’ or ‘(’ before ‘"+element+"’ token");
		}

		if (accept(syntax.lbracket)) {
			this.from = 'array'
			expect(syntax.rbracket)
		} else if (accept(syntax.lparen)) {
			this.from = 'function'
			this.arguments = new Array()
			expect(syntax.rparen);
		}

	}

	function declaration() {
		getSyntax();
		this.expect = expect(syntax.type);
		declarator.call(this);
		expect(syntax.termination)
	}

	b = new declaration();

	echo(source)
	echo(JSON.stringify(b, undefined, '....'))

//	echo(JSON.stringify(b, undefined, '    '))
}

return parse;

})()

//JSNative.Type.parse('char (data);')
//JSNative.Type.parse('char foo[];')
//JSNative.Type.parse('char *(foo);')
JSNative.Type.parse('char *(*foo)();')
JSNative.Type.parse('char (*(*x())[])();')

