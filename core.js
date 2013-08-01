#!bin/jse

/* C "dcl" grammar

	declarator:
		pointer? direct-declarator

	direct-declarator:
		identifier
		(declarator)
		direct-declarator [ constant-expression? ]
		direct-declarator ( identifier-list? )
		direct-declarator ( parameter-type-list )

	pointer:
		* type-qualifier-list?
		* type-qualifier-list? pointer

	type-qualifier-list:
		type-qualifier
		type-qualifier-list type-qualifier

Adopted from
"The C Programming Language 2nd Ed." (Prentice Hall) pp. 101, 174

Merged with
http://cboard.cprogramming.com/c-programming/157885-k-r-exercise-5-20-bsearch-error-when-compiled.html

Translated to JS By Triston J. Taylor (pc.wiz.tt@gmail.com)

Types are relaxed, meaning all type names are assumed to be a known definition

*/

JSNative.Type.parse = function nativeTypeDescription(declaration) {

	var NAME = 2, PARENS = 4, BRACKETS = 8, YES = 16, NO = 32;

	var dataType, out = '', name = '', token ='', prevToken = '', tokenType = 0, charPos = 0;

	var pointerCount = 0;

	function getChar() {return declaration[charPos++]}
	function unGetChar() {charPos--; return}
	function isAlpha(c) {return (c != undefined && c.match(/[a-zA-Z]|_/) != null)}
	function isAlphaNumeric(c) {return (c != undefined && c.match(/[a-zA-Z0-9]|_/) != null)}

	function parseDeclaration() {
		var ns;
		for (ns = 0; getToken() == '*'; ns++); /* count *'s */
		parseDirectDeclaration();
		while (ns-- > 0) out += " pointer to"		
	}

	function parseDirectDeclaration() {
		var type;
		if (tokenType == '(') {
			/* ( parseDeclaration ) */
			parseDeclaration();
			if (tokenType != ')')
			throw new SyntaxError("expected: ')' in native declaration at char " + charPos);
		} else if (tokenType == NAME) {
			if (name == '') /* variable name */ name = token;
			else out += ' '+token+':'
		} else prevToken = YES;
		while ((type=getToken()) == PARENS || type == BRACKETS || type == '(') {
		    if(type == PARENS)
		        out += " function expecting no arguments, returning";
		    else if (type == '(') {
		        out += " function that returns type "+dataType+", and expects arguments of:";
		        parseParameterDeclaration();
		    } else {
		        out += " array";
		        out += token;
		        out += " of";
		    }
		}
	}

	function parseParameterDeclaration() {
	    do {
	        parseDeclarationSpec();
	    } while(tokenType == ',');
	    if(tokenType != ')')
	        throw new SyntaxError("missing ')' in native function parameter declaration at char " +charPos);
	}

	var typeQualifiers = {'const':true, 'static':true, 'extern':true, 'volatile':true, signed:true, unsigned:true};
	function typeQualifier() {
		if (token in typeQualifiers) return YES;
		return NO;
	}

	var typeHash = this;
	function registeredType(name) {
		if (name in typeHash) return YES;
		return NO
	}

	function parseDeclarationSpec () {
		var temp = '', tq = '', name = '';
		getToken();
		do {
			if (tokenType == '...') {
				//echo("ellipsis");
				temp = "followed by a variadic argument list"
				getToken()
		    } else if (tokenType != NAME) {
		        prevToken = YES;
		        parseDeclaration();
		    } else if (typeQualifier() == YES) {
				tq += token+' '
	            getToken();
		    } else if(registeredType(temp+token) == YES) {
				temp += token+" "
	            getToken();
			} else parseDirectDeclaration();

		} while (tokenType != ',' && tokenType != ')' && tokenType != undefined);
		if (tq != '') tq = ' '+tq
		else temp = ' '+temp.trim()
		out += tq+temp;
		if(tokenType == ',') out+=tokenType
	}

	function getToken() {
		var c; token = ''

		if (prevToken == YES) {
		    prevToken = NO;
		    return tokenType;
		}

		while ((c = getChar()) == ' ' || c == '\t' || c == '\n');
		if (c == '(') {
			if ((c = getChar()) == ')') {
				token = "()"; return tokenType = PARENS;
			} else {
				unGetChar(); return tokenType = '(';
			}
		} else if (c == '[') {
			for (token += c; (token += getChar(), declaration[charPos - 1]) != ']'; );
			return tokenType = BRACKETS;
		} else if (isAlpha(c)) {
			for (token += c; isAlphaNumeric(c = getChar()); )
			token += c; unGetChar();
			return tokenType = NAME;
		} else if (c == '.') {
			for (token += c; (c = getChar()) == '.'; ) token+=c; unGetChar()
			return tokenType = token;
		} else return tokenType = c;
	}

	function parseDeclarationList() {
		parseDeclaration();
		if (out == '') out = ' an alias of '+dataType
		else if (out.match(/pointer to$|returning$/) != null) out +=' '+dataType
		if (tokenType == ',') {
			printf("%s is%s %s\n", name, out);
			name = out = '';
			parseDeclarationList();
		}
	}

	getToken(); dataType = token;
	parseDeclarationList()
	if (tokenType != undefined && tokenType != ';')
		throw new SyntaxError("expected end of input found: "+tokenType);
	printf("%s is%s %s\n", name, out);
	
}

JSNative.Type.parse('int *dare, foo, comp(), (*comp)(char * a, ...), (*(*x())[])();')
