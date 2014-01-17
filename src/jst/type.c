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


#define JSTTypeRequest(p) JSTStringCompareToUTF8(propertyName, p)

#define JSTTypeApply(d, f) (d->code |= (f))
#define JSTTypeExact(d, f) JSTCodeTypeExact(d->code)
#define JSTTypeAlias(d) (d->alias)
#define JSTTypeArray(d) (d->array)
#define JSTTypeWidth(d) JSTCodeTypeWidth(d->code)
#define JSTTypeUTF(d) JSTCodeTypeUTF(d->code)
#define JSTTypeFloat(d) ((d->floating) ? JSTCodeTypeFloat(d->code) : 0)
#define JSTTypeStructure(d) (JSTTypeIsStructure(d) ? d->structure : NULL)
#define JSTTypeReference(d) (JSTTypeIsReference(d) ? d->reference : NULL)
#define JSTTypeUnion(d) (JSTTypeIsUnion(d) ? d->abstract : NULL)

#define JSTTypeIsSignable(d) (d->autoSign)
#define JSTTypeIsSizeable(d) (d->autoWidth)
#define JSTTypeIsFloat(d) (d->floating)
#define JSTTypeIsReadOnly(d) (d->readOnly)

#define JSTTypeIsArray(d) (JSTCodeTypeIsArray(d->code))
#define JSTTypeIsValue(d) JSTCodeTypeIsValue(d->code)
#define JSTTypeIsInteger(d) JSTCodeTypeIsInteger(d->code)
#define JSTTypeIsBoolean(d) JSTCodeTypeIsBoolean(d->code)
#define JSTTypeIsSigned(d) JSTCodeTypeIsSigned(d->code)
#define JSTTypeIsUnsigned(d) JSTCodeTypeIsUnsigned(d->code)
#define JSTTypeIsVoid(d) JSTCodeTypeIsVoid(d->code)
#define JSTTypeIsConstant(d) JSTCodeTypeIsConstant(d->code)
#define JSTTypeIsDynamic(d) JSTCodeTypeIsDynamic(d->code)
#define JSTTypeIsStructure(d) JSTCodeTypeIsStructure(d->code)
#define JSTTypeIsReference(d) JSTCodeTypeIsReference(d->code)
#define JSTTypeIsUnion(d) JSTCodeTypeIsUnion(d->code)
#define JSTTypeIsUTF(d) JSTCodeTypeIsUTF(d->code)

JSTClass jst_type_class = NULL;

const utf8 * jst_type_error_no_data = "no type data available";

typedef struct jst_type_data {
	utf8 * alias;
	unsigned int code;
	JSTObject reference;
	JSTObject structure;
	JSTObject abstract; // union
	unsigned int dimensions[2];
	bool floating, autoWidth, autoSign, readOnly;
} jst_type_data;

static JSTDeclareSetProperty(jst_type_set) {

	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) {
		JSTScriptNativeError(JST_REFERENCE_ERROR, jst_type_error_no_data);
		return true;
	}
	
	bool result = false, ok = JSTValueToBoolean(value);

	/* all properties must convert to okay */
	if (!ok) {
		JSTScriptNativeError(JST_TYPE_ERROR,
			"invalid property value: setting must be true"
		);
	} else if (JSTTypeIsReadOnly(d)) {
		JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type data: type is read only"
		);
	} else if (JSTTypeRequest(jst_prop_constant)) {
		if (!JSTTypeIsConstant(d)) {
			JSTTypeApply(d, jst_type_constant);
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to constant: constant property already set"
		);
	} else if (JSTTypeRequest(jst_prop_dynamic)) {
		if (!JSTTypeIsDynamic(d)) {
			JSTTypeApply(d, jst_type_dynamic);
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to dynamic: dynamic property already set"
		);
	} else if (JSTTypeRequest(jst_prop_float)) {
		unsigned int precision = JSTValueToDouble(value);
		if (precision == 1 || precision == 2) {
			if (!JSTTypeWidth(d)) {
				JSTTypeApply(d, precision << 2);
				d->autoSign = d->autoWidth = false, d->floating = true;
			} else JSTScriptNativeError(JST_TYPE_ERROR,
				"cannot set type to floating point: type already defined"
			);
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"%u is not a valid floating point precision", precision
		);
	} else if (JSTTypeRequest(jst_prop_integer)) {
		if (!JSTTypeIsFloat(d) && !JSTTypeIsInteger(d)) {
			JSTTypeApply(d,	jst_type_integer |
				(JSTTypeIsSignable(d) ? jst_type_signed : 0) |
				(JSTTypeIsSizeable(d) ? sizeof(gint) : 0)			
			);
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to integer: type already defined"
		);
	} else if (JSTTypeRequest(jst_prop_reference)) {
		if (!JSTTypeWidth(d)) {
			JSTValueProtect(value);
			d->reference = (JSTObject) value,
			d->code |= (sizeof(guintptr)|jst_type_integer|jst_type_reference),
			d->autoSign = d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type reference: type already defined"
		);
	} else if (JSTTypeRequest(jst_prop_signed)) {
		if (JSTTypeIsSignable(d)) {
			d->code |= (jst_type_signed | jst_type_integer);
			if (d->autoWidth) d->code |= (sizeof(gint));
			d->autoSign = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot modify type sign: type already defined"
		);
	} else if (JSTTypeRequest(jst_prop_struct)) {
		if (!JSTTypeWidth(d)) {
			JSTValueProtect(value);
			d->structure = (JSTObject) value,
			d->code |= (sizeof(guintptr) | jst_type_integer | jst_type_struct),
			d->autoSign = d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to structure: type already defined"
		);
	} else if (JSTTypeRequest(jst_prop_union)) {
		if (!JSTTypeWidth(d)) {
			JSTValueProtect(value);
			d->abstract = (JSTObject) value,
			d->code |= (sizeof(guintptr) | jst_type_integer | jst_type_union),
			d->autoSign = d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type union: type already defined"
		);
	} else if (JSTTypeRequest(jst_prop_unsigned)) {
		if (JSTTypeIsSignable(d)) {
			d->code |= jst_type_integer;
			if (d->autoWidth) d->code |= sizeof(guint);
			d->autoSign = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot modify type sign: type already defined"
		);
	} else if (JSTTypeRequest(jst_prop_utf)) {
		unsigned int utfx = JSTValueToDouble(value);
		if (utfx == 8 || utfx == 16 || utfx == 32) {
			if (!JSTTypeWidth(d)) {
				d->code |= ((utfx >> 3) | jst_type_utf | jst_type_integer);
				d->autoWidth = false;
			} else JSTScriptNativeError(JST_TYPE_ERROR,
				"cannot set type to UTF-%u: type already defined", utfx
			);
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"%u is not a valid UTF specification", utfx
		);
	} else if (JSTTypeRequest(jst_prop_value)) {
		if (!JSTTypeWidth(d)) {
			d->code |= (jst_type_value | sizeof(guintptr) | jst_type_integer),
			d->autoSign = d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to %s: type already defined",
			jst_prop_value
		);
	} else if (JSTTypeRequest(jst_prop_boolean)) {
		if (!JSTTypeWidth(d) && !JSTTypeIsInteger(d)) {
			d->code |= 1,
			d->autoSign = d->autoWidth = false;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to %s: type already defined",
			jst_prop_boolean
		);
	} else if (JSTTypeRequest(jst_prop_array)) {
		if (!JSTTypeIsArray(d)) {
			JSTObject arrayConstructor = JSTObjectGetProperty(
				NULL, "Array"
			);
			if (JSTValueIsNumber(value)) {
				d->dimensions[0] = JSTValueToDouble(value);
			} else if (
				JSTValueIsObject(value) &&
				JSTValueIsConstructorInstance(value, arrayConstructor)
			) {
				size_t len = JSTValueToDouble(
					JSTObjectGetProperty(value, jst_prop_length)
				);
				JSTValue x, y;
				if (len) {
					guint
						* xp = &(d->dimensions[0]),
						* yp = &(d->dimensions[1]);
					x = JSTObjectGetPropertyAtIndex(value, 0);
					if (len > 1 && JSTValueToInt(x, xp)) {
						y = JSTObjectGetPropertyAtIndex(value, 1);
						JSTValueToInt(y, yp);					
					}
				}
			} else JSTScriptNativeError(JST_TYPE_ERROR,
				"cannot set type to array: expect number or [x, y]"
			);
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"cannot set type to array: type already defined"
		);
	}

	return result;

}

static JSTDeclareGetProperty(jst_type_get) {

	JSTValue result = JSTValueUndefined;

	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) {
		JSTScriptNativeError(JST_REFERENCE_ERROR, jst_type_error_no_data);
		return JSTValueUndefined;
	} else d->readOnly = true;

	if (JSTTypeRequest(jst_prop_constant)) {
		result = JSTValueFromBoolean(JSTTypeIsConstant(d));
	} else if (JSTTypeRequest(jst_prop_dynamic)) {
		result = JSTValueFromBoolean(JSTTypeIsDynamic(d));
	} else if (JSTTypeRequest(jst_prop_float)) {
		result = JSTValueFromDouble(JSTTypeFloat(d));
	} else if (JSTTypeRequest(jst_prop_boolean)) {
		result = JSTValueFromBoolean(JSTTypeIsBoolean(d));
	} else if (JSTTypeRequest(jst_prop_integer)) {
		result = JSTValueFromDouble((JSTTypeIsInteger(d) ? JSTTypeWidth(d) : 0));
	} else if (JSTTypeRequest(jst_prop_reference)) {
		result = JSTTypeIsReference(d)?JSTTypeReference(d):JSTValueFromBoolean(false);
	} else if (JSTTypeRequest(jst_prop_signed)) {
		result = JSTValueFromBoolean(JSTTypeIsSigned(d));
	} else if (JSTTypeRequest(jst_prop_struct)) {
		result = JSTTypeIsStructure(d)?JSTTypeStructure(d):JSTValueFromBoolean(false);
	} else if (JSTTypeRequest(jst_prop_union)) {
		result = JSTTypeIsUnion(d)?JSTTypeUnion(d):JSTValueFromBoolean(false);
	} else if (JSTTypeRequest(jst_prop_unsigned)) {
		result = JSTValueFromBoolean((d->code & jst_type_signed) == 0);
	} else if (JSTTypeRequest(jst_prop_utf)) {
		result = JSTValueFromDouble(JSTTypeUTF(d));
	} else if (JSTTypeRequest(jst_prop_value)) {
		result = JSTValueFromBoolean(JSTTypeIsValue(d));
	} else if (JSTTypeRequest(jst_prop_array)) {
		if (JSTTypeIsArray(d)) {
			JSTValue array[2] = {
				JSTValueFromDouble(d->dimensions[0]),
				JSTValueFromDouble(d->dimensions[1]),		
			};
			result = JSTObjectArray(2, array);
		}
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
	
	if (JSTTypeIsArray(d)) {	
		copy = JSTStringFromUTF8(jst_prop_array);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	}

	if (JSTTypeIsConstant(d)) {	
		copy = JSTStringFromUTF8(jst_prop_constant);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	}

	if (JSTTypeIsDynamic(d)) {	
		copy = JSTStringFromUTF8(jst_prop_dynamic);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	}
	
	if (JSTTypeIsBoolean(d)) {
		copy = JSTStringFromUTF8(jst_prop_boolean);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);		
	} else if (JSTTypeIsReference(d)) {	
		copy = JSTStringFromUTF8(jst_prop_reference);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	} else if (JSTTypeIsStructure(d)) {	
		copy = JSTStringFromUTF8(jst_prop_struct);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	} else if (JSTTypeIsUnion(d)) {	
		copy = JSTStringFromUTF8(jst_prop_union);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	} else if (JSTTypeIsUTF(d)) {	
		copy = JSTStringFromUTF8(jst_prop_utf);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	} else if (JSTTypeIsValue(d)) {	
		copy = JSTStringFromUTF8(jst_prop_value);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	} else if (JSTTypeFloat(d)) {	
		copy = JSTStringFromUTF8(jst_prop_float);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	} else if (JSTTypeIsInteger(d)) {	
		copy = JSTStringFromUTF8(jst_prop_integer);
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
		if (JSTTypeIsSigned(d)) {	
			copy = JSTStringFromUTF8(jst_prop_signed);
			JSPropertyNameAccumulatorAddName(propertyNames, copy);
			JSTStringRelease(copy);
		} else if (JSTTypeIsUnsigned(d)) {	
			copy = JSTStringFromUTF8(jst_prop_unsigned);
			JSPropertyNameAccumulatorAddName(propertyNames, copy);
			JSTStringRelease(copy);
		}
	}

}

static JSTDeclareHasProperty(jst_type_query) {

	jst_type_data * d = JSTObjectGetPrivate(object);
	if (!d) return false;

	bool result = false;
	
	if (JSTTypeRequest(jst_prop_reference)) result = JSTTypeReference(d);
	else if (JSTTypeRequest(jst_prop_array)) result = JSTTypeIsArray(d);
	else if (JSTTypeRequest(jst_prop_boolean)) result = JSTTypeIsBoolean(d);
	else if (JSTTypeRequest(jst_prop_constant)) result = JSTTypeIsConstant(d);
	else if (JSTTypeRequest(jst_prop_dynamic)) result = JSTTypeIsDynamic(d);
	else if (JSTTypeRequest(jst_prop_float)) result = JSTTypeFloat(d);
	else if (JSTTypeRequest(jst_prop_integer)) result = JSTTypeIsInteger(d);
	else if (JSTTypeRequest(jst_prop_signed)) result = JSTTypeIsSigned(d);
	else if (JSTTypeRequest(jst_prop_struct)) result = JSTTypeIsStructure(d);
	else if (JSTTypeRequest(jst_prop_union)) result = JSTTypeIsUnion(d);
	else if (JSTTypeRequest(jst_prop_unsigned)) result = JSTTypeIsUnsigned(d);
	else if (JSTTypeRequest(jst_prop_utf)) result = JSTTypeUTF(d);
	else if (JSTTypeRequest(jst_prop_value)) JSTTypeIsValue(d);

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
		if (d->alias) return JSTValueFromUTF8(d->alias);
		return JSTObjectGetProperty(object, jst_prop_name);
	}
	
	return false;
}

static JSTDeclareGetProperty(jst_type_get_state) {
	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) return JSTScriptNativeError(
		JST_REFERENCE_ERROR, jst_type_error_no_data
	);

	JSTValue result = JSTValueUndefined;
	void * data = NULL;
	
	if (JSTTypeRequest(jst_prop_read_only))
		result = JSTValueFromBoolean((d)? JSTTypeIsReadOnly(d) : false);
	else if (JSTTypeRequest(jst_prop_alias) && (data = JSTTypeAlias(d)))
		result = JSTValueFromUTF8(data);
	else if (JSTTypeRequest(jst_prop_name)) {
		result = JSTValueFromUTF8(data);
	} else if (JSTTypeRequest(jst_prop_native)) {
		result = JSTValueFromUTF8(data);
	}

	return result;
}

static JSTDeclareGetProperty(jst_type_get_width) {
	
	jst_type_data * d = JSTObjectGetPrivate(object); JSTObject result;
	
	if (!d) return JSTScriptNativeError(
		JST_REFERENCE_ERROR, jst_type_error_no_data
	);

	unsigned int width = JSTTypeWidth(d);	
	result = JSTValueToObject(JSTValueFromDouble(width));
	
	JSTObjectSetProperty(result, jst_prop_bits,
		JSTValueFromDouble(
			(width) ? (width << 3) : 0
		),  JSTObjectPropertyState
	);
	
	return result;
	
}

static JSTDeclareSetProperty(jst_type_set_width) {
	
	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) return JSTScriptNativeError(
		JST_REFERENCE_ERROR, jst_type_error_no_data
	);

	if (d->autoWidth) {
		unsigned int width = JSTValueToDouble(value);
		if (JSTCodeTypeWidth(width)) {
			d->code |= width;
		} else JSTScriptNativeError(JST_TYPE_ERROR,
			"%u is not a valid native type width", width
		);
		d->autoWidth = false;
	} else JSTScriptNativeError(JST_TYPE_ERROR,
		"cannot set type width: property already defined"
	);

	return true;
}

jst_type_data * jst_type_data_new(const utf8 * alias, unsigned int code) {
	jst_type_data * d = g_malloc0(sizeof(jst_type_data));
	d->alias = (alias) ? g_strdup(alias) : NULL, d->code = code,
	d->autoSign = d->autoWidth = true;
	return d;
}

void jst_type_data_free(jst_type_data * d) {
	if (d) {
		g_free(d->alias), g_free(d);
	}
}

static JSTDeclareFinalizer(jst_type_finalize) {
	jst_type_data_free(JSTObjectGetPrivate(object));
	JSTObjectSetPrivate(object, NULL);
}

static JSTClass jst_type_init() {

	JSTClassAccessor properties[] = {
		{
			jst_prop_alias,
			&jst_type_get_state, NULL,
			JSTObjectPropertyState
		},
		{
			jst_prop_read_only,
			&jst_type_get_state, NULL,
			JSTObjectPropertyState
		},
		{
			jst_prop_name,
			&jst_type_get_state, NULL,
			JSTObjectPropertyState
		},
		{
			jst_prop_native,
			&jst_type_get_state, NULL,
			JSTObjectPropertyState
		},
		{
			jst_prop_width,
			&jst_type_get_width, &jst_type_set_width,
			JSTObjectPropertyRequired
		},
		{NULL, NULL, NULL, 0}
	};

	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	jsClass.className = "type",
	jsClass.attributes = JSTClassPropertyManualPrototype,
	jsClass.setProperty = &jst_type_set,
	jsClass.getProperty = &jst_type_get,
	jsClass.deleteProperty = &jst_type_delete,
	jsClass.staticValues = properties,
	jsClass.hasProperty = &jst_type_query,
	jsClass.getPropertyNames = &jst_type_enumerate,
	jsClass.convertToType = &jst_type_convert,
	jsClass.finalize = &jst_type_finalize;
	jst_type_class = JSClassRetain(JSClassCreate(&jsClass));

	return jst_type_class;

}

static void * jst_type_parse_code JSTUtility(JSTObject object, size_t code) {

	jst_type_data * d = JSTObjectGetPrivate(object);

	if (!d) {
		JSTScriptNativeError(JST_REFERENCE_ERROR, jst_type_error_no_data);
		return NULL;
	}
		
	if (
		JSTCodeTypeIsReference(code) ||
		JSTCodeTypeIsStructure(code) ||
		JSTCodeTypeIsUnion(code) ||
		JSTCodeTypeIsArray(code)
	) {
		JSTScriptNativeError(JST_REFERENCE_ERROR,
			"unable to complete type request: no object data available"
		);
		return NULL;
	}
	
	if (JSTCodeTypeIsConstant(code)) d->code |= jst_type_constant;
	if (JSTCodeTypeIsDynamic(code)) d->code |= jst_type_dynamic;
	
	size_t floater = JSTCodeTypeFloat(code);

	if (floater) {
		d->code |= floater;
		d->autoSign = d->autoWidth = false, d->floating = true;
		return object;
	}

	if (JSTCodeTypeIsBoolean(code)) {
		d->code |= 1;
		d->autoSign = d->autoWidth = false;
		return object;
	}

	if (JSTCodeTypeIsInteger(code)) {
		d->code |= (sizeof(gint) | jst_type_integer);
		if (JSTCodeTypeIsValue(code)) {
			d->code |= jst_type_value,
			d->autoSign = d->autoWidth = false;
			return object;
		}
		if (JSTCodeTypeIsSigned(code)) {
			d->code |= (jst_type_signed);
			d->autoSign = false;			
		} else {
			d->autoSign = false;
		}
		if (JSTCodeTypeUTF(code)) {
			d->code |= (JSTCodeTypeWidth(code) | jst_type_utf);
			d->autoWidth = false;
			return object;
		}
		size_t width = JSTCodeTypeWidth(code);
		if (width) {
			d->code |= width;
			d->autoWidth = false;
		}
	}

	return object;
	
}

static JSTValue jst_type_constructor JSTDeclareFunction() {

	if (argc == 0 || argc > 2) return JSTScriptNativeError(JST_REFERENCE_ERROR,
		"expected arguments: name, code"
	);

	utf8 * name = JSTValueToUTF8(argv[0]);
	size_t code = 0;
	
	if (argc > 1 && !JSTValueToInt(argv[1], &code)) {
		return NULL;
	}
	
	jst_type_data * private = jst_type_data_new(name, 0);
	
	JSTObject object = jst_type_parse_code(
		ctx, JSTClassInstance(jst_type_class, private), code, exception
	);

	return object;

}
