/*

Hypersoft Systems JST AND Hypersoft Systems JSE
Copyright (c) 2014, Triston J. Taylor

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

JSTClass jst_type_class = NULL;

const utf8 * jst_type_error_no_data = "no type data available";

const utf8 * jst_type_prop_array = "array";
const utf8 * jst_type_prop_constant = "constant";
const utf8 * jst_type_prop_dynamic = "dynamic";
const utf8 * jst_type_prop_float = "float";
const utf8 * jst_type_prop_name = "name";
const utf8 * jst_type_prop_integer = "integer";
const utf8 * jst_type_prop_reference = "reference";
const utf8 * jst_type_prop_signed = "signed";
const utf8 * jst_type_prop_struct = "struct";
const utf8 * jst_type_prop_union = "union";
const utf8 * jst_type_prop_unsigned = "unsigned";
const utf8 * jst_type_prop_utf = "utf";
const utf8 * jst_type_prop_value = "value";
const utf8 * jst_type_prop_width = "width";
const utf8 * jst_type_prop_bits = "bits";

typedef struct jst_type_data {
	utf8 * name;
	unsigned int code;
	JSTObject reference;
	JSTObject structure;
	JSTObject abstract; // union
	unsigned int dimensions[2];
	bool autoWidth, autoSign;
	gushort readOnly;
} jst_type_data;

static char * jst_type_validate(char * key) {
	return NULL;
}

static JSTDeclareSetProperty(jst_type_set) {

	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) {
		JSTScriptNativeError(JST_REFERENCE_ERROR, jst_type_error_no_data);
		return true;
	}
	
	bool result = false, ok = JSTValueToBoolean(value),
	nInt = (d->autoWidth && (d->code & (1| 2 | 4 | 8 | jst_type_integer)) == 0);

	/* all properties must convert to okay */
	if (!ok) {
		JSTScriptNativeError(JST_TYPE_ERROR,
			"invalid property value: setting must be true"
		);
	} else if (d->readOnly) {
		JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type data: type is read only"
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_array)) {
		g_error(
			"jst_type_set property: %s is not yet implemented",
			jst_type_prop_array
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_constant)) {
		if ((d->code & jst_type_constant) == 0) {
			d->code |= jst_type_constant;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to constant: constant property already set"
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_dynamic)) {
		if ((d->code & jst_type_dynamic) == 0) {
			d->code |= jst_type_dynamic;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to dynamic: dynamic property already set"
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_float)) {
		unsigned int precision = JSTValueToDouble(value);
		if (precision == 1 || precision == 2) {
			if (d->autoSign && nInt) {
				d->code |= (precision << 2);
				d->autoSign = d->autoWidth = false;
			} else JSTScriptNativeError(JST_TYPE_ERROR,
				"cannot set type to floating point: type already defined"
			);
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"%u is not a valid floating point precision", precision
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_name)) {
		g_error(
			"jst_type_set property: %s is not yet implemented",
			jst_type_prop_name
		);		
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_integer)) {
		if ((d->code & jst_type_integer) == 0) {
			d->code |= jst_type_integer;
			if (d->autoSign) d->code |= jst_type_signed;
			if (d->autoWidth) d->code |= sizeof(gint);
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to integer: type already defined"
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_reference)) {
		if (d->autoSign && nInt) {
			JSTValueProtect(value);
			d->reference = (JSTObject) value,
			d->code |= sizeof(guintptr) | jst_type_integer,
			d->autoSign = d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type reference: type already defined"
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_signed)) {
		if (d->autoSign) {
			d->code |= (jst_type_signed | jst_type_integer);
			if (d->autoWidth) d->code |= (sizeof(gint));
			d->autoSign = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot modify type sign: type already defined"
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_struct)) {
		if (d->autoSign && nInt) {
			JSTValueProtect(value);
			d->structure = (JSTObject) value,
			d->code |= sizeof(guintptr) | jst_type_integer,
			d->autoSign = d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type structure: type already defined"
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_union)) {
		if (d->autoSign && nInt) {
			JSTValueProtect(value);
			d->abstract = (JSTObject) value,
			d->code |= sizeof(guintptr) | jst_type_integer,
			d->autoSign = d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type union: type already defined"
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_unsigned)) {
		if (d->autoSign) {
			d->code |= jst_type_integer;
			if (d->autoWidth) d->code |= sizeof(guint);
			d->autoSign = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot modify type sign: type already defined"
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_utf)) {
		unsigned int utfx = JSTValueToDouble(value);
		if (utfx == 8 || utfx == 16 || utfx == 32) {
			if (nInt) {
				d->code |= ((utfx >> 3) | jst_type_utf | jst_type_integer);
				d->autoWidth = false;
			} else JSTScriptNativeError(JST_TYPE_ERROR,
				"cannot set type to UTF-%u: type already defined", utfx
			);
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"%u is not a valid UTF specification", utfx
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_value)) {
		if (d->autoSign && nInt) {
			d->code |= (jst_type_value | sizeof(guintptr) | jst_type_integer),
			d->autoSign = d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to %s: type already defined",
			jst_type_prop_value
		);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_width)) {
		if (d->autoWidth) {
			unsigned int width = JSTValueToDouble(value);
			if (width == 1 || width == 2 || width == 4 || width == 8) {
				d->code |= width;
			} else JSTScriptNativeError(JST_TYPE_ERROR,
				"%u is not a valid native type width", width
			);
			d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type %s: property already defined", jst_type_prop_width
		);
	}

	return result;

}

static JSTDeclareGetProperty(jst_type_get) {

	JSTValue result = NULL;

	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) {
		JSTScriptNativeError(JST_REFERENCE_ERROR, jst_type_error_no_data);
		return JSTValueUndefined;
	} else d->readOnly = true;

	if (JSTStringCompareToUTF8(propertyName, jst_type_prop_array)) {
		g_error(
			"jst_type_get property: %s is not yet implemented",
			jst_type_prop_array
		);		
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_constant)) {
		result = JSTValueFromBoolean(d->code & jst_type_constant);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_dynamic)) {
		result = JSTValueFromBoolean(d->code & jst_type_dynamic);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_float)) {
		result = JSTValueFromBoolean((d->code & jst_type_integer) == 0);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_name)) {
		result = JSTValueFromUTF8(d->name);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_integer)) {
		result = JSTValueFromBoolean(d->code & jst_type_integer);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_reference)) {
		if (d->reference) result = d->reference;
		else result = JSTValueUndefined;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_signed)) {
		result = JSTValueFromBoolean(d->code & jst_type_signed);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_struct)) {
		if (d->structure) result = d->structure;
		else result = JSTValueUndefined;		
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_union)) {
		if (d->abstract) result = d->abstract;
		else result = JSTValueUndefined;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_unsigned)) {
		result = JSTValueFromBoolean((d->code & jst_type_signed) == 0);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_utf)) {
		result = JSTValueFromBoolean(d->code & jst_type_utf);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_value)) {
		result = JSTValueFromBoolean(d->code & jst_type_value);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_width)) {
		unsigned int width = 0;
		if (d->code & jst_type_1) width = 1;
		else if (d->code & jst_type_2) width = 2;
		else if (d->code & jst_type_4) width = 4;
		else if (d->code & jst_type_8) width = 8;
		result = JSTValueFromDouble(width);
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_bits)) {
		unsigned int bits = 0;
		if (d->code & jst_type_1) bits = 8;
		else if (d->code & jst_type_2) bits = 16;
		else if (d->code & jst_type_4) bits = 32;
		else if (d->code & jst_type_8) bits = 64;
		result = JSTValueFromDouble(bits);
	}

	return result;

}

static JSTDeclareDeleteProperty(jst_type_delete) {

	bool result = false;
	
	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) {
		JSTScriptNativeError(JST_REFERENCE_ERROR, jst_type_error_no_data);
		return true;
	}

	return result;

}

static JSTDeclareGetPropertyNames(jst_type_enumerate) {

	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) return;

	JSTString copy;

	copy = JSTStringFromUTF8(jst_type_prop_bits);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_constant);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_dynamic);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_float);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_name);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_integer);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_reference);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_signed);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_struct);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_union);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_unsigned);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_utf);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_value);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

	copy = JSTStringFromUTF8(jst_type_prop_width);
	JSPropertyNameAccumulatorAddName(propertyNames, copy);
	JSTStringRelease(copy);

}

static JSTDeclareHasProperty(jst_type_query) {

	jst_type_data * d = JSTObjectGetPrivate(object);
	if (!d) return false;

	bool result = false;
	
	if (JSTStringCompareToUTF8(propertyName, jst_type_prop_array)) {
		result = false;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_bits)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_constant)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_dynamic)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_float)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_name)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_integer)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_reference)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_signed)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_struct)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_union)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_unsigned)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_utf)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_value)) {
		result = true;
	} else if (JSTStringCompareToUTF8(propertyName, jst_type_prop_width)) {
		result = true;
	}

	return result;

}

static JSTDeclareConvertor(jst_type_convert) {
	
	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) return JSTScriptNativeError(
		JST_REFERENCE_ERROR, jst_type_error_no_data
	);

	if (type == JSTValueTypeNumber) {
		return JSTValueFromDouble(d->code);
	} else if (type == JSTValueTypeString) {
		return JSTValueFromUTF8(d->name);
	}
	
	return false;
}

jst_type_data * jst_type_data_new(const utf8 * name, unsigned int code) {
	jst_type_data * d = g_malloc0(sizeof(jst_type_data));
	d->name = g_strdup(name), d->code = code,
	d->autoSign = d->autoWidth = true;
	return d;
}

void jst_type_data_free(jst_type_data * d) {
	if (d) {
		g_free(d->name); g_free(d);
	}
}

static JSTDeclareFinalizer(jst_type_finalize) {
	jst_type_data_free(JSTObjectGetPrivate(object));
	JSTObjectSetPrivate(object, NULL);
}

static JSTClass jst_type_init() {

	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	jsClass.className = "type",
	jsClass.attributes = JSTClassPropertyManualPrototype,
	jsClass.setProperty = &jst_type_set,
	jsClass.getProperty = &jst_type_get,
	jsClass.deleteProperty = &jst_type_delete,
	jsClass.hasProperty = &jst_type_query,
	jsClass.getPropertyNames = &jst_type_enumerate,
	jsClass.convertToType = &jst_type_convert,
	jsClass.finalize = &jst_type_finalize;
	jst_type_class = JSClassRetain(JSClassCreate(&jsClass));

	return jst_type_class;

}

static JSTValue jst_type_constructor JSTDeclareFunction(name, code) {

	if (argc != 2) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"expected arguments: name, code"
	);

	utf8 * name = JSTValueToUTF8(argv[0]);
	size_t code = JSTValueToDouble(argv[1]);
	
	jst_type_data * private = jst_type_data_new(name, code);
	
	JSTObject object = JSTClassInstance(jst_type_class, private);

	return object;

}
