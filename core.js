#!bin/jse

// current type system does not do anything! :(
// need methods to extract types from parsed sources, or define types,
// as parsed sources...

// parsing algo needs to remove redundant declarators to relax info access burdens

/* JS NATIVE TYPE */ (function JSNativeTypeClass() {

function getUnsignedType() {
	var unsigned;
	if ((unsigned = JSNative.Type[this.name+' unsigned'])) return unsigned;
	if ((unsigned = JSNative.Type['unsigned '+this.name])) return unsigned;
	return JSNative.Type[this.type | JSNative.api.typeUnsigned]
}

function getSignedType() {
	var signed;
	if ((signed = JSNative.Type[this.name+' signed'])) return signed;
	if ((signed = JSNative.Type['signed '+this.name])) return signed;
	return JSNative.Type[this & ~JSNative.api.typeUnsigned]
}

function getArrayType() {
	if (this.type == JSNative.api.typeVoid) return;
	var array;
	if ((array = JSNative.Type[this.name+'[]'])) return array;
	if ((array = JSNative.Type[this.name+' **'])) return array;
	return JSNative.Type[this | JSNative.api.typeArray]
}

function getTypeNames() {
	var names = [];
	for (name in JSNative.Type) if (isNaN(name) && JSNative.Type[name].type == this.type) names.push(name)
	return names;
}

function getPointerType() {
	var pointer;
	if ((pointer = JSNative.Type[this.name+' *'])) return pointer;
	if ((pointer = JSNative.Type[this.name+'*'])) return pointer;
	return JSNative.Type['void'].pointer;
};

function isIntegerType(code) { return (code & (8|16|32|64|128|1024))}

var classConstruct = function(code, name){

	code = Number(code);

	if ((code & JSNative.api.typeEllipsis) != 0) { // That's an error
		throw new InvokeError("TypeError", "invalid type code "+code+" (ellipsis)");		
	}

	// Validate unsigned flag
	if ((code & (JSNative.api.typeUnsigned)) != 0) {
		if ((code & (JSNative.api.typeVoid)) || code == JSNative.api.typeUnsigned)
		throw new InvokeError("TypeError", "invalid type code "+code+" (unsigned void)");		
		if ((code & (JSNative.api.typeBoolean)))
		throw new InvokeError("TypeError", "invalid type code "+code+" (unsigned boolean)");		
		if ((code & (JSNative.api.typeFloat)))
		throw new InvokeError("TypeError", "invalid type code "+code+" (unsigned float)");		
		if ((code & (JSNative.api.typeDouble)))
		throw new InvokeError("TypeError", "invalid type code "+code+" (unsigned double)");		
	}

	this.type = code, this.name = name;

	// type size is a constant defined on the 'type definition' for optimal performance
	Object.defineProperty(this, 'size', {value: JSNative.api.getTypeSize(this), enumerable:true})

	// The first definition of a code is its "native representation"
	// don't overwrite codes that have been written.
	if (code in this.constructor); else this.constructor[code] = this;

	// The name of a type is its alias
	// don't overwrite an alias that has been written
	if (name in this.constructor) {
		throw new InvokeError("TypeError", "type name '"+name+"' is already registered");
	} else {
		this.constructor[name] = this;
	}

	// I'm deprecating the auto pointer and array definitions, as we (now) have more advanced
	// algorithms in place to help sort out the mess.

	if ((code & (JSNative.api.typePointer | JSNative.api.typeArray) ) == 0) {
		//var ptrType = new JSNative.Type(code | JSNative.api.typePointer, name+' *');
		//var arrType;
		//if (code != JSNative.api.typeVoid) arrType = new JSNative.Type(code | JSNative.api.typeArray, name+'[]');
		//var ptrArrayType = new JSNative.Type(code | JSNative.api.typePointer | JSNative.api.typeArray, name+' *[]')
		//var ptrPointerType = new JSNative.Type(code | JSNative.api.typePointer | JSNative.api.typeArray, name+' **')
		if (code & (JSNative.api.typeShort | JSNative.api.typeLong | JSNative.api.typeLongLong)) {
			JSNative.Type["int "+name] = this;
			JSNative.Type[name+" int"] = this;
			//JSNative.Type["int "+name+' *'] = ptrType;
			//JSNative.Type[name+" int *"] = ptrType;
			//JSNative.Type["int "+name+'[]'] = arrType;
			//JSNative.Type[name+" int[]"] = arrType;
			//JSNative.Type["int "+name+' *[]'] = ptrArrayType;
			//JSNative.Type[name+" int *[]"] = ptrArrayType;
			//JSNative.Type[name+" int **"] = ptrPointerType;
		} else {
			//JSNative.Type[name+" **"] = ptrPointerType;
		}
	}

	// is it an integer type?
	Object.defineProperty(this, "integer", {value: isIntegerType(code), enumerable:true})

	if (this.integer) {
		if (name.match(/(un)?signed|\*$|\[\]$/));
		else {
			var unsignedName = "unsigned "+name
			if (unsignedName in JSNative.Type); else new JSNative.Type(code | JSNative.api.typeUnsigned, unsignedName);

			unsignedName = name+" unsigned"
			if (unsignedName in JSNative.Type); else new JSNative.Type(code | JSNative.api.typeUnsigned, unsignedName);
		}
		if (name.match(/(un)?signed|\*$|\[\]$/));
		else {
			var signedName = "signed "+name
			if (signedName in JSNative.Type); else new JSNative.Type(code, signedName);

			signedName = name+" signed"
			if (signedName in JSNative.Type); else new JSNative.Type(code, signedName);
		}
		Object.defineProperty(this, 'unsigned', {get:getUnsignedType, enumerable:true})
		Object.defineProperty(this, 'signed', {get:getSignedType, enumerable:true})
	}


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
		sizeOf: function(typeId) {return this[typeId].size},
		isInteger:function(typeId){return this[typeId].integer},
		isUnsigned:function(typeId){return (this[typeId] & JSNative.api.typeUnsigned)}
	}
)

Object.defineProperties(JSNative.Type.prototype, {
	pointer:{get:getPointerType, enumerable:true},
	array:{get:getArrayType, enumerable:true},
	names:{get:getTypeNames, enumerable:true}
})

})();

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

var parse = function parse(source) {

	// It would be nice to pack the source, but that creates error reporting issues...

	var element, charPosition = 0, token, accepted;

	var types = JSNative.Type, qualifiers = {'const':true,'static':true,'volatile':true,'extern':true};

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
		this.identifier = token;
	}

	function pointer() {
		if (accept(syntax.asterisk)) {
			for(this.pointer = 1; accept(syntax.asterisk); this.pointer++);
		}
	}

	function declarator() {
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
		declarator.call(this);
		dcls.push(this);
	}

	try { // we use internal functions, so we need to refactor any reference to an error
		getSyntax();
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

})()

// Initialize Native Types
new JSNative.Type(JSNative.api.typeVoid,		"void")
new JSNative.Type(JSNative.api.typeBoolean,		"bool")
new JSNative.Type(JSNative.api.typeChar,		"char")
new JSNative.Type(JSNative.api.typeShort,		"short")
new JSNative.Type(JSNative.api.typeInt,			"int")
new JSNative.Type(JSNative.api.typeLong,		"long")
new JSNative.Type(JSNative.api.typeLongLong,	"long long")
new JSNative.Type(JSNative.api.typeFloat,		"float")
new JSNative.Type(JSNative.api.typeDouble,		"double")

//for (name in JSNative.Type) if (isNaN(name) && JSNative.Type[name].constructor == JSNative.Type) echo(name)
result = JSNative.Type.parse('char foo(char[4], char[][4][4][2]), data;')
echo(JSON.stringify(result, undefined, '....'))

//JSNative.Type.parse('char (data);')
//JSNative.Type.parse('char foo[];')
//JSNative.Type.parse('char *(foo);')
//JSNative.Type.parse('char *(*foo)();')
//JSNative.Type.parse('char (*(*x())[][13])();')

