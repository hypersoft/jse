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
	semicolon:';',	

	qualifier:'qualifier', type:'type', identifier:'identifier', number:'number', ellipsis:'ellipsis',

	termination:undefined

}

var parse = function parse(source) {

	// It would be nice to pack the source, but that creates error reporting issues...

	var element, charPosition = 0, token, accepted;

	var result = Object.create(parse.prototype);

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
		if (element === undefined) return;
		if (element.match(/[\(\)\[\]{},\*;]/) != null) return // we parse simple elements here, not constructs
		if (element.match(/[a-z_]/i) != null) {
			/* "Smart Ass Subversive Descent Parsing" -- pc.wiz.tt */
			for (accepted = element; (element = getChar()) != undefined && element.match(/[a-z0-9_]/i) != null; accepted += element);
			unGetChar();
			if (accepted in qualifiers) {
				// these things need some validation against legality, tho' we don't use them in JSNI
				for(c = 1; (words = accepted + wordPeek(c)).replace(/\s+/gm, ' ').trim().split(' ').pop() in qualifiers; ok = words, c++);
				element = syntax.qualifier;
			} else if (accepted in types) {
				// type registry must be robust and stupid, containing all possible type designations
				// such as: char, signed char, char signed, unsigned char, char unsigned, etc...
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
		if (accept(s)) return 1;
		throw new SyntaxError("char "+charPosition+": expected '"+s+"'");
	}

	function declarator() {
		var count = 0;
		while (accept(syntax.asterisk)) count++;
		result.indirection = count;
		directDeclarator()
		while (count-- > 0) result.description += " pointer to %reference"
	}

	function directDeclarator() {
		if (accept(syntax.lparen)) { declarator(); expect(syntax.rparen) }
		else if (accept(syntax.identifier)) {
			result.identifier = token;
		}
		while (element == syntax.lparen || element == syntax.lbracket) {
			if (accept(syntax.lparen)) {
				result.type = 'function'; if (! result.arguments ) result.arguments = [];
				if (accept(syntax.rparen)) {
					result.description += " function expecting no arguments, returning %reference";
				} else {
					result.description += " function that returns %reference, and expects arguments of:"
				}
			} else {
				result.type = 'array'; if (! result.dimensions ) result.dimensions = [];
				expect(syntax.lbracket);
				result.description += " array["
				if ((result.dimensions.length && expect(syntax.number)) || accept(syntax.number)) {
					result.dimensions.push(token); result.description += token
				} else result.dimensions.push(null)
				expect(syntax.rbracket);
				result.description += "] of"
			}
		}
	}


	getSyntax()

	if (accept(syntax.type)) {
		result.reference = token;
		declarator()
	}

	echo(JSON.stringify(result));
}

parse.prototype = {
	reference:undefined, description:'', identifier:undefined, type:'number'
}
return parse;

})()


JSNative.Type.parse('int *foo[][8]')
