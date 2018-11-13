#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct sGhtmlConfiguration {
    const char * path;
    const char * name;
    gboolean 
        center,
        disable_decorations, 
        transparent, 
        desktop_widget, 
        stay_on_top, 
        stay_on_bottom, 
        no_pager, 
        no_taskbar,
        with_inspector;
    GtkWindow * window;
    WebKitWebView * view;
    const char * file;
    GdkRectangle geometry;
} GhtmlConfiguration;

GhtmlConfiguration Ghtml;

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window);
static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window);
static void window_geometry_changed(WebKitWindowProperties *windowProperties, GtkWindow *window);
static void webViewTitleChanged(WebKitWebView *webView, GParamSpec *pspec, GtkWindow *window);

int ghtml_parse_option_with_value(char * opt, char * val);

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

void
load_changed (WebKitWebView  *web_view,
               WebKitLoadEvent load_event,
               gpointer        user_data)
{
    if (load_event == WEBKIT_LOAD_COMMITTED) {
//        g_printerr("load finished\n");
        webkit_web_view_run_javascript(web_view, ";", NULL, NULL, NULL);
    }
}

#define STREQUAL(A, B) ((strcmp(A, B)) == 0)

int ghtml_parse_option_with_value(char * opt, char * val) {
    return 0;
}

int ghtml_parse_file_option(char * opt, char * val) {
    if (STREQUAL(opt, "--html-application") || STREQUAL(opt, "-f")) {
        ghtml_check_required_file("GHTML Application File", val, false);
        Ghtml.file = val;
        return 2;
    }
    return 0;
}

int ghtml_parse_option(char * opt) {

    if (STREQUAL(opt, "--no-decor")) {
        Ghtml.disable_decorations = true;
        return 1;
    }
    if (STREQUAL(opt, "--transparent")) {
        Ghtml.transparent = true;
        return 1;
    }

    if (STREQUAL(opt, "--desktop-widget")) {
        Ghtml.no_pager = true;
        Ghtml.no_taskbar = true;
        Ghtml.stay_on_bottom = true;
        Ghtml.disable_decorations = true;
        Ghtml.desktop_widget = true;
        return 1;
    }

    if (STREQUAL(opt, "--stay-on-top")) {
        Ghtml.stay_on_top = true;
        Ghtml.stay_on_bottom = false;
        return 1;
    }

    if (STREQUAL(opt, "--stay-on-bottom")) {
        Ghtml.stay_on_bottom = true;
        Ghtml.stay_on_top = false;
        return 1;
    }

    if (STREQUAL(opt, "--no-pager")) {
        Ghtml.no_pager = true;
        return 1;
    }

    if (STREQUAL(opt, "--center")) {
        Ghtml.center = true;
        return 1;
    }

    if (STREQUAL(opt, "--with-inspector")) {
        Ghtml.with_inspector = true;
        return 1;
    }

    if (STREQUAL(opt, "--no-taskbar")) {
        Ghtml.no_taskbar = true;
        return 1;
    }

    return 0;
}

gboolean supports_alpha = TRUE;

static void
screen_changed (GtkWidget *window,
                         GdkScreen *old_screen,
                         GtkWidget *label)
{

  GdkScreen *screen = gtk_widget_get_screen (GTK_WIDGET (window));

  GdkVisual *visual = gdk_screen_get_rgba_visual (screen);
  if (visual == NULL) visual = gdk_screen_get_system_visual (screen);
  gtk_widget_set_visual (window, visual);

}

void ghtml_start_application(int argc, char * argv[]) {

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_window_set_default_geometry(GTK_WINDOW(window),)

    Ghtml.window = GTK_WINDOW(window);

    if (Ghtml.disable_decorations) {
        gtk_window_set_decorated(Ghtml.window, false);
    }

    if (Ghtml.no_pager) {
        gtk_window_set_skip_pager_hint(Ghtml.window, true);
    }

    if (Ghtml.no_taskbar) {
        gtk_window_set_skip_taskbar_hint(Ghtml.window, true);
    }

    if (Ghtml.stay_on_bottom) {
        gtk_window_set_keep_below(Ghtml.window, true);
    } 
    
    else if (Ghtml.stay_on_top) {
        gtk_window_set_keep_above(Ghtml.window, true);
    }

    if (Ghtml.desktop_widget) {
        gtk_window_set_type_hint(Ghtml.window, GDK_WINDOW_TYPE_HINT_DOCK || GDK_WINDOW_TYPE_HINT_UTILITY);
    } 
    
    if (Ghtml.center) {
        gtk_window_set_position(Ghtml.window, GTK_WIN_POS_CENTER);
    }
    
    webkit_web_context_set_web_extensions_directory(
        webkit_web_context_get_default(),
            "/usr/share/jse/plugin");

    // Create a browser instance
    WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new());

    Ghtml.view = webView;

    WebKitSettings * webkitSettings = webkit_web_view_get_settings(webView);

    WebKitWindowProperties *windowProperties = webkit_web_view_get_window_properties(webView);
    g_signal_connect (windowProperties, "notify::geometry",
                      G_CALLBACK (window_geometry_changed), window);

    g_signal_connect(webView, "notify::title", G_CALLBACK(webViewTitleChanged), window);

    // Set up callbacks so that if either the main window or the browser instance is
    // closed, the program will exit
    g_signal_connect(window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
    g_signal_connect(webView, "close", G_CALLBACK(closeWebViewCb), window);
    g_signal_connect(webView, "load-changed", G_CALLBACK(load_changed), window);
    
    webkit_settings_set_enable_javascript(webkitSettings, TRUE);
    webkit_settings_set_enable_java(webkitSettings, TRUE);

    if (Ghtml.with_inspector) {
        webkit_settings_set_enable_developer_extras(webkitSettings, TRUE);
    }

    webkit_settings_set_enable_plugins(webkitSettings, TRUE);

    if (Ghtml.transparent) {
        gtk_widget_set_app_paintable(window, TRUE);
        g_signal_connect(G_OBJECT(window), "screen-changed", G_CALLBACK(screen_changed), NULL);
        screen_changed(GTK_WIDGET(Ghtml.window), NULL, NULL);
        const GdkRGBA color = {.0, .0, .0, .0};
        webkit_web_view_set_background_color(webView, &color);
    }

    // Put the browser area into the main window
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));

    // Make sure that when the browser area becomes visible, it will get mouse
    // and keyboard events
    if (!Ghtml.desktop_widget) {
        gtk_widget_grab_focus(GTK_WIDGET(webView));
    }

    // Make sure the main window and all its contents are visible
    gtk_widget_show_all(GTK_WIDGET(Ghtml.window));

    char * fileCap = get_file_name_as_file_uri(Ghtml.file);
    webkit_web_view_load_uri(webView, fileCap);
    g_free(fileCap);

    // Run the main GTK+ event loop
    gtk_main();

    g_assert_not_reached();

}

#define shift(C) argc -= C; argv+= C

int main(int argc, char* argv[])
{
    memset(&Ghtml, 0, sizeof(GhtmlConfiguration));

    Ghtml.name = "ghtml";
    Ghtml.path = argv[0];
    Ghtml.file = NULL;

    // Initialize GTK+
    gtk_init(&argc, &argv);

    shift(1);

    int parsed = 0;

    while (argc) {
        
        if ((parsed = ghtml_parse_option(argv[0]))) {
            shift(parsed); continue;
        }

        if (argc < 1) break;

        if ((parsed = ghtml_parse_option_with_value(argv[0], argv[1]))) {
            shift(parsed); continue;
        }

        if ((parsed = ghtml_parse_file_option(argv[0], argv[1]))) {
            shift(1);
            ghtml_start_application(argc, argv);
        }

        break;

    }

    g_printerr("%s: error: no action was specified\n", Ghtml.name);

    return 1;

}


static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
    ghtml_die(0);
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

static void webViewTitleChanged(WebKitWebView *webView, GParamSpec *pspec, GtkWindow *window)
{
    const char *title = webkit_web_view_get_title(webView);
    gtk_window_set_title(GTK_WINDOW(Ghtml.window), title ? title : "Ghtml Application");

}

