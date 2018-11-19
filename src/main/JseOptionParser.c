#include <jse.h>

static const char * Error = "parse option error";
GQuark JseOptionParseError = 0;

static JseOptionParseResult * create_parse_result(int argc, char ** argv)
{
	
	if (! JseOptionParseError )
		JseOptionParseError = g_quark_from_static_string(Error);
	
	if (argc < 2) return NULL;
	
	JseOptionParseResult * result = g_malloc0 (
		( sizeof (JseOptionParseResult) + (sizeof (char *) * argc) )
	);
	
	result->argc = argc;
	
	int i; for (i = 0; i < argc; i++)
		result->argv[i] = argv[i];
	
	return result;
	
}

void jse_option_parse_result_free(JseOptionParseResult * result)
{
	if (! result ) return;
	if (result->error) g_error_free(result->error);
	g_free(result);
}

static JseOption * jse_option_find_word(char * word, JseOption config[])
{
	int i; for (i = 0; config[i].flag; i++) {
		if (config[i].flag & OPT_WORD)
			if (g_str_has_prefix(word, config[i].longName))
				return &config[i];
	}
	return NULL;
}

static JseOption * jse_option_find_short(JseOptionType flag, char item, JseOption config[])
{
	int i; for (i = 0; config[i].flag; i++) {
		if (config[i].flag & flag)
			if (config[i].shortName == item)
				return &config[i];
	}
	return NULL;
}

static JseOption * jse_option_find_long(JseOptionType flag, char * item, JseOption config[])
{
	item += 2;
	int i; for (i = 0; config[i].flag; i++) {
		if (config[i].flag & flag)
			if (g_str_has_prefix(item, config[i].longName))
				return &config[i];
	}
	return NULL;
}

static char * jse_option_value(char * action) {
	if (! action ) return NULL;
	if (action[0] == '+') action++;
	while (*action)
		if (action[0] == ':' || action[0] == '+') return ++action;
		else action++;
	return NULL;
}

static GError * jse_option_parse_prefix(char * action, JseOptionType flag, char * value, gboolean * acceptedValue, JseOption config[], JseOptionCallback handler)
{
	if (flag & (OPT_PLUS | OPT_MINUS)) {
		char * actionValue = jse_option_value(action);
		int itemIndex, itemLength, lastItemIndex;
		if (actionValue) itemLength = actionValue - action;
		else itemLength = strlen(action);
		lastItemIndex = itemLength - 1;
		for (itemIndex = 1; itemIndex < itemLength; itemIndex++) {
			char shortID = action[itemIndex];
			JseOption * option = jse_option_find_short(flag, shortID, config);
			if (! option ) return g_error_new (
				JseOptionParseError, 2,
				"option `%c' in `%s' is not a command option", shortID, action
			);
			void * error;
			if (itemIndex != lastItemIndex) {
				if ((option->flag & OPT_COMBO) == 0) return g_error_new (
					JseOptionParseError, 2,
					"option `%c' in `%s' may not be combined with other options", shortID, action
				);
				if (option->flag & OPT_ARG) return g_error_new (
					JseOptionParseError, 2,
					"option `%c' requires argument and must be the last option in `%s'", shortID, action
				);
				if (error = handler(flag, option, NULL)) return error;
			} else {
				if (option->flag & OPT_ARG) {
					if (! actionValue && ! value) return g_error_new (
						JseOptionParseError, 2,
						"option `%c' in `%s' requires argument", shortID, action
					);
					if (error = handler(flag, option, (actionValue)?actionValue:value)) return error;
					if (! actionValue ) *acceptedValue = TRUE;
				} else {
					if (error = handler(flag, option, NULL)) return error;
				}
				return NULL;
			}
		} /* end loop */
		g_assert_not_reached();
	} else { /* --long-option */
		JseOption * option = jse_option_find_long(flag, action, config);
		if (! option ) return g_error_new (
			JseOptionParseError, 2,
			"option `%s' is not a command option", action
		);
		char * actionValue;
		if (option->flag & OPT_ARG) {
			void * error = NULL;
			actionValue = jse_option_value(action);
			if (! actionValue && ! value) return g_error_new (
				JseOptionParseError, 2,
				"option `%s' requires argument", action
			);
			if (error = handler(flag, option, (actionValue)?actionValue:value)) return error;
			if (! actionValue ) *acceptedValue = TRUE;
			return NULL;
		} else {
			return handler(flag, option, NULL);
		}
		g_assert_not_reached();
	}
	g_assert_not_reached();
}

JseOptionParseResult * jse_option_parse_main(int argc, char ** argv, JseOption config[], JseOptionCallback handler)
{
	JseOptionParseResult * result = create_parse_result(argc, argv);
	if (! result ) return NULL;
	for(result->argi = 1; result->argi < result->argc; result->argi++) {
		char * action = result->argv[result->argi];
		JseOption * option = jse_option_find_word(action, config);
		if (option) {
			unsigned wordLength = strlen(option->longName);
			if (option->flag & OPT_ARG) {
				if (action[wordLength]) {
					char * value = &action[wordLength];
					if (value[0] == '=' || value[0] == ':') value++;
					if (result->error = handler(OPT_WORD, option, value))
						return result;
				} else {
					char * value = ((result->argi + 1) < result->argc)
						? result->argv[++result->argi]
						: NULL;
					if (! value ) {
						result->error = g_error_new (
							JseOptionParseError, 1,
							"option `%s' requires argument", action
						);
						return result;
					} else if (result->error = handler(OPT_WORD, option, value))
						return result;					
				}
				continue;
			} else {
				if (result->error = handler(OPT_WORD, option, NULL))
					return result;
			}
			continue;
		} else {
			gboolean accepted = FALSE;
			char * value = ((result->argi + 1) < result->argc)
			? result->argv[result->argi + 1]
			: NULL;
			if (g_str_has_prefix(action, "--")) {
				if (result->error = jse_option_parse_prefix (
					action, OPT_LONG, value, &accepted, config, handler
				)) return result;
			} else if (action[0] == '-') {
				if (result->error = jse_option_parse_prefix (
					action, OPT_MINUS, value, &accepted, config, handler
				)) return result;
			} else if (action[0] == '+') {
				if (result->error = jse_option_parse_prefix (
					action, OPT_PLUS, value, &accepted, config, handler
				)) return result;
			} else {
				result->error = g_error_new (
					JseOptionParseError, 0,
					"unknown command option `%s'", action
				);
				return result;
			}
			if (accepted) result->argi++;
		}
	}
	
	return result;
	
}
