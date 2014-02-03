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

static JSTDeclareSetProperty(jst_class_set) {
	bool result = false;
	JSTObject private = JSTObjectGetPrivate(object);
	JSTValue property = JSTValueFromString(propertyName, false);
	JSTObject setProperty = JSTObjectGetProperty(private, "set");
	if (JSTValueIsFunction(setProperty)) result = JSTValueToBoolean(
		JSTFunctionCall(setProperty, private, property, value)
	);
	return result;
}

static JSTDeclareGetProperty(jst_class_get) {
	JSTValue result = NULL;
	JSTObject private = JSTObjectGetPrivate(object);
	JSTValue property = JSTValueFromString(propertyName, false);
	JSTObject getProperty = JSTObjectGetProperty(private, "get");
	if (JSTValueIsFunction(getProperty)) {
		result = (JSTFunctionCall(getProperty, private, property));
		if (JSTValueIsNull(result)) result = NULL;
	}
	return result;
}

static JSTDeclareDeleteProperty(jst_class_delete) {
	bool result = false;
	JSTObject private = JSTObjectGetPrivate(object);
	JSTValue property = JSTValueFromString(propertyName, false);
	JSTObject deleteProperty = JSTObjectGetProperty(private, "delete");
	if (JSTValueIsFunction(deleteProperty))	result = JSTValueToBoolean(
		JSTFunctionCall(deleteProperty, private, property)
	);
	return result;
}

static JSTDeclareGetPropertyNames(jst_class_enumerate) {
	size_t count = 0, i; JSTString copy;
	void * e = NULL, * exception = &e;
	JSTObject private = JSTObjectGetPrivate(object); JSTValue result;
	JSTObject getPropertyNames = JSTObjectGetProperty(
		private, "list"
	); if (JSTValueIsFunction(getPropertyNames)) {
		result = (JSTFunctionCall(getPropertyNames, private));
		count = JSTValueToDouble(JSTObjectGetProperty(result, "length"));
	} else count = 0;
	for (i = 0; i < count; i++) {
		copy = JSTValueToString(JSTObjectGetPropertyAtIndex(result, i));
		JSPropertyNameAccumulatorAddName(propertyNames, copy);
		JSTStringRelease(copy);
	}
}

static JSTDeclareHasProperty(jst_class_query) {
	bool result = false;
	void * e = NULL, * exception = &e;
	JSTObject private = JSTObjectGetPrivate(object);
	JSTValue property = JSTValueFromString(propertyName, false);
	JSTObject hasProperty = JSTObjectGetProperty(private, "test");
	if (JSTValueIsFunction(hasProperty)) result = JSTValueToBoolean(
		JSTFunctionCall(hasProperty, private, property)
	);
	return result;
}

static JSTValue jst_class_define JSTDeclareFunction() {
	JSTClassDefinition jsClass = JSTClassEmptyDefinition;
	JSTObject prototype = argo[0];
	utf8 * className = JSTValueToUTF8(JSTObjectGetProperty(prototype, "name"));
	jsClass.className = className;
	if (JSTObjectHasProperty(prototype, "attributes"))
		jsClass.attributes = JSTValueToDouble(
			JSTObjectGetProperty(prototype, "attributes")
		);
	if (JSTObjectHasProperty(prototype, "parent"))
		jsClass.parentClass = JSTObjectGetPrivate(
			JSTObjectGetProperty(prototype, "parent")
		);
	if (JSTObjectHasProperty(prototype, "set"))
		jsClass.setProperty = &jst_class_set;
	if (JSTObjectHasProperty(prototype, "get"))
		jsClass.getProperty = &jst_class_get;
	if (JSTObjectHasProperty(prototype, "delete"))
		jsClass.deleteProperty = &jst_class_delete;
	if (JSTObjectHasProperty(prototype, "test"))
		jsClass.hasProperty = &jst_class_query;
	if (JSTObjectHasProperty(prototype, "list"))
		jsClass.getPropertyNames = &jst_class_enumerate;
	void * class = JSClassRetain(JSClassCreate(&jsClass));
	g_free(className);	
	JSTObject private = JSTClassRoot(class, prototype);
	JSTObjectSetProperty(private, "create",
		JSTScriptNativeEval("sys.class_instance;", NULL), JSTObjectPropertyAPI
	); return private;

}

static JSTValue jst_class_instance JSTDeclareFunction() {
	JSTClass class = JSTObjectGetPrivate(this);
	JSTObject instance = JSTClassInstance(class, this),
	interface = JSTObjectGetPrototype(this),	
	initialize = JSTObjectGetProperty(interface, "create");
	if (JSTValueIsFunction(initialize)) JSObjectCallAsFunction(
		ctx, initialize, this, argc, argv, exception
	); return instance;
}