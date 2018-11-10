
#include "GhtmlWindow.h"
#include <errno.h>
#include <gtk/gtk.h>
#include <string.h>
#include <webkit2/webkit2.h>

#include <inttypes.h>

#define GHTML_ERROR (ghtmlErrorQuark())

static const gchar * file = NULL;
static const gchar **uriArguments = NULL;
static const char *ghtmlAboutScheme = "ghtml-about";

typedef enum {
    GHTML_ERROR_INVALID_ABOUT_PATH
} GhtmlError;

static GQuark ghtmlErrorQuark()
{
    return g_quark_from_string("ghtml-quark");
}

static gchar *argumentToURL(const char *filename)
{
    GFile *gfile = g_file_new_for_commandline_arg(filename);
    gchar *fileURL = g_file_get_uri(gfile);
    g_object_unref(gfile);

    return fileURL;
}

static void createGhtmlWindow(const gchar *uri, WebKitSettings *webkitSettings)
{
    GtkWidget *webView = webkit_web_view_new();
    GtkWidget *mainWindow = ghtml_window_new(WEBKIT_WEB_VIEW(webView), NULL);
    gchar *url = argumentToURL(uri);

    if (webkitSettings)
        webkit_web_view_set_settings(WEBKIT_WEB_VIEW(webView), webkitSettings);

    ghtml_window_load_uri(GHTML_WINDOW(mainWindow), url);
    g_free(url);

    gtk_widget_grab_focus(webView);
    gtk_widget_show(mainWindow);
}

static const GOptionEntry commandLineOptions[] =
{
  { "file", 'f', 0, G_OPTION_ARG_FILENAME, &file, "HTML File", "FILE" },
 //   { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &uriArguments, 0, "[URL…]" },
    { 0, 0, 0, 0, 0, 0, 0 }
};

static gboolean parseOptionEntryCallback(const gchar *optionNameFull, const gchar *value, WebKitSettings *webSettings, GError **error)
{
    if (strlen(optionNameFull) <= 2) {
        g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, "Invalid option %s", optionNameFull);
        return FALSE;
    }

    /* We have two -- in option name so remove them. */
    const gchar *optionName = optionNameFull + 2;
    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(webSettings), optionName);
    if (!spec) {
        g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, "Cannot find web settings for option %s", optionNameFull);
        return FALSE;
    }

    switch (G_PARAM_SPEC_VALUE_TYPE(spec)) {
    case G_TYPE_BOOLEAN: {
        gboolean propertyValue = !(value && g_ascii_strcasecmp(value, "true") && strcmp(value, "1"));
        g_object_set(G_OBJECT(webSettings), optionName, propertyValue, NULL);
        break;
    }
    case G_TYPE_STRING:
        g_object_set(G_OBJECT(webSettings), optionName, value, NULL);
        break;
    case G_TYPE_INT: {
        glong propertyValue;
        gchar *end;

        errno = 0;
        propertyValue = g_ascii_strtoll(value, &end, 0);
        if (errno == ERANGE || propertyValue > G_MAXINT || propertyValue < G_MININT) {
            g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, "Integer value '%s' for %s out of range", value, optionNameFull);
            return FALSE;
        }
        if (errno || value == end) {
            g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, "Cannot parse integer value '%s' for %s", value, optionNameFull);
            return FALSE;
        }
        g_object_set(G_OBJECT(webSettings), optionName, propertyValue, NULL);
        break;
    }
    case G_TYPE_FLOAT: {
        gdouble propertyValue;
        gchar *end;

        errno = 0;
        propertyValue = g_ascii_strtod(value, &end);
        if (errno == ERANGE || propertyValue > G_MAXFLOAT || propertyValue < G_MINFLOAT) {
            g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, "Float value '%s' for %s out of range", value, optionNameFull);
            return FALSE;
        }
        if (errno || value == end) {
            g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, "Cannot parse float value '%s' for %s", value, optionNameFull);
            return FALSE;
        }
        g_object_set(G_OBJECT(webSettings), optionName, propertyValue, NULL);
        break;
    }
    default:
        g_assert_not_reached();
    }

    return TRUE;
}

static gboolean isValidParameterType(GType gParamType)
{
    return (gParamType == G_TYPE_BOOLEAN || gParamType == G_TYPE_STRING || gParamType == G_TYPE_INT
            || gParamType == G_TYPE_FLOAT);
}

static GOptionEntry* getOptionEntriesFromWebKitSettings(WebKitSettings *webSettings)
{
    GParamSpec **propertySpecs;
    GOptionEntry *optionEntries;
    guint numProperties, numEntries, i;

    propertySpecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(webSettings), &numProperties);
    if (!propertySpecs)
        return NULL;

    optionEntries = g_new0(GOptionEntry, numProperties + 1);
    numEntries = 0;
    for (i = 0; i < numProperties; i++) {
        GParamSpec *param = propertySpecs[i];

        /* Fill in structures only for writable and not construct-only properties. */
        if (!param || !(param->flags & G_PARAM_WRITABLE) || (param->flags & G_PARAM_CONSTRUCT_ONLY))
            continue;

        GType gParamType = G_PARAM_SPEC_VALUE_TYPE(param);
        if (!isValidParameterType(gParamType))
            continue;

        GOptionEntry *optionEntry = &optionEntries[numEntries++];
        optionEntry->long_name = g_param_spec_get_name(param);

        /* There is no easy way to figure our short name for generated option entries.
           optionEntry.short_name=*/
        /* For bool arguments "enable" type make option argument not required. */
        if (gParamType == G_TYPE_BOOLEAN && (strstr(optionEntry->long_name, "enable")))
            optionEntry->flags = G_OPTION_FLAG_OPTIONAL_ARG;
        optionEntry->arg = G_OPTION_ARG_CALLBACK;
        optionEntry->arg_data = parseOptionEntryCallback;
        optionEntry->description = g_param_spec_get_blurb(param);
        optionEntry->arg_description = g_type_name(gParamType);
    }
    g_free(propertySpecs);

    return optionEntries;
}

static gboolean addSettingsGroupToContext(GOptionContext *context, WebKitSettings* webkitSettings)
{
    GOptionEntry *optionEntries = getOptionEntriesFromWebKitSettings(webkitSettings);
    if (!optionEntries)
        return FALSE;

    GOptionGroup *webSettingsGroup = g_option_group_new("websettings",
                                                        "WebKitSettings writable properties for default WebKitWebView",
                                                        "WebKitSettings properties",
                                                        webkitSettings,
                                                        NULL);
    g_option_group_add_entries(webSettingsGroup, optionEntries);
    g_free(optionEntries);

    /* Option context takes ownership of the group. */
    g_option_context_add_group(context, webSettingsGroup);

    return TRUE;
}

static void
aboutURISchemeRequestCallback(WebKitURISchemeRequest *request, gpointer userData)
{
    GInputStream *stream;
    gsize streamLength;
    const gchar *path;
    gchar *contents;
    GError *error;

    path = webkit_uri_scheme_request_get_path(request);
    if (!g_strcmp0(path, "/ghtml")) {
        contents = g_strdup_printf("<html><body><h1>ghtml</h1><p>The WebKit2 ghtml assembly.</p><p>WebKit version: %d.%d.%d</p></body></html>",
            webkit_get_major_version(),
            webkit_get_minor_version(),
            webkit_get_micro_version());
        streamLength = strlen(contents);
        stream = g_memory_input_stream_new_from_data(contents, streamLength, g_free);

        webkit_uri_scheme_request_finish(request, stream, streamLength, "text/html");
        g_object_unref(stream);
    } else {
        error = g_error_new(GHTML_ERROR, GHTML_ERROR_INVALID_ABOUT_PATH, "Invalid about:%s page.", path);
        webkit_uri_scheme_request_finish_error(request, error);
        g_error_free(error);
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

	webkit_web_context_set_web_extensions_directory(webkit_web_context_get_default(),
		"/usr/share/jse/plugin"
	);

    const gchar *multiprocess = g_getenv("GHTML_MULTIPROCESS");
    if (multiprocess && *multiprocess) {
        webkit_web_context_set_process_model(webkit_web_context_get_default(),
            WEBKIT_PROCESS_MODEL_MULTIPLE_SECONDARY_PROCESSES);
    }

    GOptionContext *context = g_option_context_new(NULL);
	g_option_context_set_ignore_unknown_options(context, true);
    g_option_context_add_main_entries(context, commandLineOptions, 0);
    g_option_context_add_group(context, gtk_get_option_group(TRUE));

    WebKitSettings *webkitSettings = webkit_settings_new();
    webkit_settings_set_enable_developer_extras(webkitSettings, TRUE);
    webkit_settings_set_enable_webgl(webkitSettings, TRUE);
    if (!addSettingsGroupToContext(context, webkitSettings))
        g_clear_object(&webkitSettings);

    g_option_context_parse(context, &argc, &argv, NULL);
    g_option_context_free (context);
	if (!file) file = "about:ghtml";

	char buffer[64];
	sprintf(buffer, "%i", argc);
	g_setenv("GHTML_ARGC", buffer, true);
	sprintf(buffer, "%u", (uintptr_t)argv);
	g_setenv("GHTML_ARGV", buffer, true);

/*
	webkit_web_context_set_web_extensions_initialization_user_data(
		webkit_web_context_get_default(),
        NULL
	);
*/

    // Enable the favicon database, by specifying the default directory.
    webkit_web_context_set_favicon_database_directory(webkit_web_context_get_default(), NULL);

    webkit_web_context_register_uri_scheme(webkit_web_context_get_default(), ghtmlAboutScheme, aboutURISchemeRequestCallback, NULL, NULL);

    createGhtmlWindow(file, webkitSettings);


    gtk_main();

   g_clear_object(&webkitSettings);

   return 0;
}