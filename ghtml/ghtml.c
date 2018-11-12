#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct sGhtmlConfiguration {
    const char * path;
    const char * name;
    GtkWindow * window;
    WebKitWebView * view;
    const char * file;
    int width, height;
    GdkRectangle geometry;
} GhtmlConfiguration;

GhtmlConfiguration Ghtml;

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window);
static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window);
static void webViewReadyToShow(WebKitWebView *webView, GtkWindow *window);
static void window_geometry_changed(WebKitWindowProperties *windowProperties, GtkWindow *window);
static void webViewTitleChanged(WebKitWebView *webView, GParamSpec *pspec, GtkWindow *window);

int ghtml_parse_file_option(char * opt, char * val);

void ghtml_die(int);

void ghtml_check_required_file(char *type, char *file, bool checkExec) {

	/* Test if file is null */
	if (! file || ! *file) {
		g_printerr ("%s: error: %s was expected, yet not fully specified\n", Ghtml.name, type);
		ghtml_die(1);
	}

	/* Test if file exists. */
	if (!g_file_test(file, G_FILE_TEST_EXISTS)) {
		g_printerr ("%s: error: %s `%s' was not found\n", Ghtml.name, type, file);
		ghtml_die(1);
	}

	/* Test if file is directory */
	if (g_file_test(file, G_FILE_TEST_IS_DIR)) {
		g_printerr (
			"%s: error: `%s' is not a %s, it is a directory\n", 
			Ghtml.name, file, type
		);
		ghtml_die(1);
	}

	/* Possibly test if the file is executable */
	if (checkExec && !g_file_test(file, G_FILE_TEST_IS_EXECUTABLE)) {
		g_printerr (
			"%s: error: `%s' is not an executable %s file.\n", 
			Ghtml.name, file, type
		);
		ghtml_die(1);
	}

}

char * get_file_name_as_file_uri(register const char * file) {
    char buffer1[16384], buffer2[16384];
    file = realpath(file, buffer1);
    sprintf(buffer2, "%s%s", "file://", file);
    return g_strdup(buffer2);
}

int main(int argc, char* argv[])
{
    memset(&Ghtml, 0, sizeof(GhtmlConfiguration));

    Ghtml.name = "ghtml";
    Ghtml.file = NULL;

    // Initialize GTK+
    gtk_init(&argc, &argv);

    // Create an 800x600 window that will contain the browser instance
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    Ghtml.window = GTK_WINDOW(window);

	webkit_web_context_set_web_extensions_directory(
        webkit_web_context_get_default(),
		    "/usr/share/jse/plugin");

    // Create a browser instance
    WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
    Ghtml.view = webView;

    WebKitWindowProperties *windowProperties = webkit_web_view_get_window_properties(webView);
    g_signal_connect (windowProperties, "notify::geometry",
                      G_CALLBACK (window_geometry_changed), window);

    g_signal_connect(webView, "notify::title", G_CALLBACK(webViewTitleChanged), window);

    // Set up callbacks so that if either the main window or the browser instance is
    // closed, the program will exit
    g_signal_connect(window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
    g_signal_connect(webView, "close", G_CALLBACK(closeWebViewCb), window);
    g_signal_connect(webView, "ready-to-show", G_CALLBACK(webViewReadyToShow), window);

    WebKitSettings * webkitSettings = webkit_web_view_get_settings(webView);

    webkit_settings_set_enable_java(webkitSettings, TRUE);
    webkit_settings_set_enable_javascript(webkitSettings, TRUE);
    webkit_settings_set_enable_developer_extras(webkitSettings, TRUE);
    webkit_settings_set_enable_plugins(webkitSettings, TRUE);

    // Put the browser area into the main window
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));
    
    if (ghtml_parse_file_option(argv[1], argv[2]) == 2) {
        argc-=2; argv+=2;
    }

    if (Ghtml.file) {
        char * fileCap = get_file_name_as_file_uri(Ghtml.file);
        webkit_web_view_load_uri(webView, fileCap);
        g_free(fileCap);
    } else {
    // Load a web page into the browser instance
    webkit_web_view_load_uri(webView, "file:///usr/share/jse/ghtml");

    }


    // Make sure that when the browser area becomes visible, it will get mouse
    // and keyboard events
    gtk_widget_grab_focus(GTK_WIDGET(webView));

    // Make sure the main window and all its contents are visible
    gtk_widget_show_all(window);

    // Run the main GTK+ event loop
    gtk_main();

    return 0;
}


static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
    gtk_main_quit();
}

static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window)
{
    gtk_widget_destroy(window);
    return TRUE;
}

void ghtml_die(int code) {

    gtk_main_quit();
	exit(code);

}

#define STREQUAL(A, B) ((strcmp(A, B)) == 0)

int ghtml_parse_file_option(char * opt, char * val) {
    if (STREQUAL(opt, "--html-application") || STREQUAL(opt, "-f")) {
        ghtml_check_required_file("GHTML Application File", val, false);
        Ghtml.file = val;
        return 2;
    }
    return 0;
}

static void window_geometry_changed(WebKitWindowProperties *windowProperties, GtkWindow *window)
{

    gtk_window_set_resizable(GTK_WINDOW
        (Ghtml.window),
            webkit_window_properties_get_resizable (windowProperties));

    GdkRectangle geometry;
    webkit_window_properties_get_geometry (windowProperties, &geometry);

    if (Ghtml.geometry.x != geometry.x || Ghtml.geometry.x != geometry.y) {
        gtk_window_move (GTK_WINDOW
            (Ghtml.window), 
                geometry.x, geometry.y);
    }

    if (Ghtml.geometry.width != geometry.width || Ghtml.geometry.height != geometry.height) {
        gtk_window_resize (GTK_WINDOW (Ghtml.window), geometry.width, geometry.height);
    }

    Ghtml.geometry = geometry;

}

static void webViewReadyToShow(WebKitWebView *webView, GtkWindow *window)
{
    g_printerr("signal: ready-to-show\n");
    WebKitWindowProperties *windowProperties = webkit_web_view_get_window_properties(webView);
    g_signal_connect (windowProperties, "notify::geometry",
                      G_CALLBACK (window_geometry_changed), window);

     if (webkit_window_properties_get_fullscreen (windowProperties)) {
        gtk_window_fullscreen (GTK_WINDOW (window));
    } else {
        window_geometry_changed(windowProperties, window);
    }

    gtk_widget_show_all(GTK_WIDGET(window));

}

static void webViewTitleChanged(WebKitWebView *webView, GParamSpec *pspec, GtkWindow *window)
{
    const char *title = webkit_web_view_get_title(webView);
    gtk_window_set_title(GTK_WINDOW(Ghtml.window), title ? title : "Ghtml Application");

}
