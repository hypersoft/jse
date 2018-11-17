#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct sGhtmlConfiguration {
    const char * path;
    const char * name;
    gboolean 
        modal,
        maximize,
        execute,
        resizable,
        auto_hide_titlebar,
        stay_hidden,
        force_focus,
        center,
        center_on_parent,
        center_on_mouse,
        disable_decorations, 
        transparent, 
        stay_on_top, 
        stay_on_bottom, 
        no_pager, 
        no_taskbar,
        with_inspector,
        with_javascript;
    GtkWindow * window, * parent, * attachment;
    WebKitWebView * view;
    const char * file;
    GdkPixbuf * icon;
    GdkRectangle geometry;
    int type_hint;
} GhtmlConfiguration;

GhtmlConfiguration Ghtml;

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window);
static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window);
static void window_geometry_changed(WebKitWindowProperties *windowProperties, GtkWindow *window);
static void webViewTitleChanged(WebKitWebView *webView, GParamSpec *pspec, GtkWindow *window);
static void faviconChanged(GObject *object, GParamSpec *paramSpec, gpointer *window);
static void screen_changed (GtkWidget *window,
                         GdkScreen *old_screen,
                         GtkWidget *label);

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

    WebKitWindowProperties *windowProperties = webkit_web_view_get_window_properties(web_view);
    window_geometry_changed(windowProperties, Ghtml.window);

    if (load_event == WEBKIT_LOAD_STARTED) {
        if (!(Ghtml.type_hint & GDK_WINDOW_TYPE_HINT_DOCK))  gtk_widget_grab_focus(GTK_WIDGET(Ghtml.view));
    }
    
    if (load_event == WEBKIT_LOAD_COMMITTED) {
        char buf[8192];
        sprintf(buf, "Ghtml.window = %p; Ghtml.view = %p; Ghtml.parent = %p; Ghtml.attachment = %p; Ghtml.typeHint = %i;", Ghtml.window, Ghtml.view, Ghtml.parent, Ghtml.attachment, Ghtml.type_hint);
        webkit_web_view_run_javascript(web_view, buf, NULL, NULL, NULL);
        if (Ghtml.transparent) {
            gtk_widget_set_app_paintable(GTK_WIDGET(Ghtml.window), TRUE);
            g_signal_connect(G_OBJECT(Ghtml.window), "screen-changed", G_CALLBACK(screen_changed), NULL);
            screen_changed(GTK_WIDGET(Ghtml.window), NULL, NULL);
            const GdkRGBA color = {.0, .0, .0, .0};
            webkit_web_view_set_background_color(Ghtml.view, &color);
        }
        if (!Ghtml.stay_hidden) gtk_widget_show_all(GTK_WIDGET(Ghtml.window));
    }

    if (load_event == WEBKIT_LOAD_FINISHED) {
        if (Ghtml.force_focus) gtk_window_present(Ghtml.window);
    }

}

#define STREQUAL(A, B) ((strcmp(A, B)) == 0)

int ghtml_parse_option_with_value(char * opt, char * val) {
    if (STREQUAL(opt, "--parent-window")) {
        sscanf(val, "%p", &Ghtml.parent);
        return 2;
    }
    if (STREQUAL(opt, "--attached-to")) {
        sscanf(val, "%p", &Ghtml.attachment);
        return 2;
    }
    if (STREQUAL(opt, "--type-hint")) {
        sscanf(val, "%i", &Ghtml.type_hint);
        return 2;
    }
    if (STREQUAL(opt, "--icon")) {
        Ghtml.icon = gdk_pixbuf_new_from_file(val, NULL);
        return 2;
    }
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
    if (STREQUAL(opt, "--auto-hide-titlebar")) {
        Ghtml.auto_hide_titlebar= true;
        return 1;
    }

    if (STREQUAL(opt, "--desktop-widget")) {
        Ghtml.no_pager = true;
        Ghtml.no_taskbar = true;
        Ghtml.stay_on_bottom = true;
        Ghtml.disable_decorations = true;
        Ghtml.type_hint = GDK_WINDOW_TYPE_HINT_DOCK || GDK_WINDOW_TYPE_HINT_UTILITY;
        return 1;
    }

    if (STREQUAL(opt, "--dialog")) {
        Ghtml.no_pager = true;
        Ghtml.no_taskbar = true;
        Ghtml.stay_on_top = true;
        Ghtml.type_hint = GDK_WINDOW_TYPE_HINT_DIALOG;
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

    if (STREQUAL(opt, "--maximize")) {
        Ghtml.maximize = true;
        return 1;
    }

    if (STREQUAL(opt, "--resizeable")) {
        Ghtml.resizable = true;
        return 1;
    }

    if (STREQUAL(opt, "--force-focus")) {
        Ghtml.force_focus = true;
        return 1;
    }

    if (STREQUAL(opt, "--stay-hidden")) {
        Ghtml.stay_hidden = true;
        return 1;
    }

    if (STREQUAL(opt, "--modal")) {
        Ghtml.modal = true;
        return 1;
    }

    if (STREQUAL(opt, "--center")) {
        Ghtml.center = true;
        return 1;
    }

    if (STREQUAL(opt, "--center-on-parent")) {
        Ghtml.center_on_parent = true;
        return 1;
    }

    if (STREQUAL(opt, "--center-on-mouse")) {
        Ghtml.center_on_mouse = true;
        return 1;
    }

    if (STREQUAL(opt, "--with-inspector")) {
        Ghtml.with_inspector = true;
        return 1;
    }

    if (STREQUAL(opt, "--with-javascript")) {
        Ghtml.with_javascript = true;
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

    Ghtml.window = GTK_WINDOW(window);
    Ghtml.execute = g_file_test(Ghtml.file, G_FILE_TEST_IS_EXECUTABLE);

    gtk_window_set_default_size(Ghtml.window, 0, 0);
    gtk_window_move(Ghtml.window, 0, 0);

    if (Ghtml.icon) {
        gtk_window_set_icon(Ghtml.window, Ghtml.icon);
    }

    gtk_window_set_resizable(Ghtml.window, Ghtml.resizable);

    if (Ghtml.parent) {
        gtk_window_set_transient_for(Ghtml.window, Ghtml.parent);
        gtk_window_set_destroy_with_parent(Ghtml.window, true);
    }

    if (Ghtml.attachment) {
        gtk_window_set_attached_to(Ghtml.window, GTK_WIDGET(Ghtml.attachment));
        gtk_window_set_destroy_with_parent(Ghtml.window, true);
    }

    gtk_window_set_modal(Ghtml.window, Ghtml.modal);

    if (Ghtml.disable_decorations) {
        gtk_window_set_decorated(Ghtml.window, false);
    } else if (Ghtml.auto_hide_titlebar) {
        gtk_window_set_hide_titlebar_when_maximized(Ghtml.window, true);
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

    if (Ghtml.type_hint) {
        gtk_window_set_type_hint(Ghtml.window, Ghtml.type_hint);    
    }
    
    if (Ghtml.center_on_mouse) gtk_window_set_position(Ghtml.window, GTK_WIN_POS_MOUSE);

    if (Ghtml.center) {
        gtk_window_set_position(Ghtml.window, GTK_WIN_POS_CENTER);
    }
    
    if (Ghtml.center_on_parent) {
        gtk_window_set_position(Ghtml.window, GTK_WIN_POS_CENTER_ON_PARENT);
    }

    if (Ghtml.execute) { // ONLY LOAD PLUGINS IF THE FILE IS EXECUTABLE
        webkit_web_context_set_web_extensions_directory(
            webkit_web_context_get_default(),
                "/usr/share/jse/plugin");
    }

    // Create a browser instance
    WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new());

    Ghtml.view = webView;

    WebKitSettings * webkitSettings = webkit_web_view_get_settings(webView);

    WebKitWindowProperties *windowProperties = webkit_web_view_get_window_properties(webView);
    g_signal_connect (windowProperties, "notify::geometry",
                      G_CALLBACK (window_geometry_changed), window);

    g_signal_connect(webView, "notify::title", G_CALLBACK(webViewTitleChanged), window);
    g_signal_connect(webView, "notify::favicon", G_CALLBACK(faviconChanged), window);

    // Set up callbacks so that if either the main window or the browser instance is
    // closed, the program will exit
    g_signal_connect(window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
    g_signal_connect(webView, "close", G_CALLBACK(closeWebViewCb), window);
    g_signal_connect(webView, "load-changed", G_CALLBACK(load_changed), window);
    
    // allow this to be turned on for basic web functionality.
    if (Ghtml.with_javascript) webkit_settings_set_enable_javascript(webkitSettings, TRUE);

    if (Ghtml.execute) { // ONLY ENABLE SECURE FEATURES IF THE FILE IS EXECUTABLE

        webkit_settings_set_enable_javascript(webkitSettings, TRUE);
        webkit_settings_set_enable_plugins(webkitSettings, TRUE);

        webkit_settings_set_javascript_can_access_clipboard(webkitSettings, true);
        webkit_settings_set_allow_file_access_from_file_urls(webkitSettings, true);
        webkit_settings_set_allow_universal_access_from_file_urls(webkitSettings, true);
        webkit_settings_set_enable_java(webkitSettings, TRUE);

    }

    if (Ghtml.with_inspector) {
        webkit_settings_set_enable_developer_extras(webkitSettings, TRUE);
    }

    if (Ghtml.maximize) {
        gtk_window_maximize(Ghtml.window);
    }


    // Put the browser area into the main window
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));

    char * fileCap = get_file_name_as_file_uri(Ghtml.file);
    webkit_web_view_load_uri(webView, fileCap);
    g_free(fileCap);

    // Run the main GTK+ event loop
    gtk_main();

    g_assert_not_reached();

}

#define shift(C) argc -= C; argv+= C; argno += C

int main(int argc, char* argv[])
{
    memset(&Ghtml, 0, sizeof(GhtmlConfiguration));
    int argno = 0;
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

    if (argc) {
        g_printerr("%s: error: foreign command parameter at position %i: %s\n", Ghtml.name, argno, argv[0]);

    } else {
        g_printerr("%s: error: no action was specified\n", Ghtml.name);
    }

    return 1;

}


static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
    ghtml_die(0);
}

static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window)
{
    gtk_widget_destroy(window);
    ghtml_die(0);
    return TRUE;
}

void ghtml_die(int code) {

    gtk_main_quit();
	exit(code);

}

static void window_geometry_changed(WebKitWindowProperties *windowProperties, GtkWindow *window)
{

    /* I'm not sure how this is supposed to work-out, never found a javascript method for
        "make window [not ]resizable"
    */
    //gtk_window_set_resizable(GTK_WINDOW
    //    (Ghtml.window),
    //        webkit_window_properties_get_resizable (windowProperties));

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

static void faviconChanged(GObject *object, GParamSpec *paramSpec, gpointer *window)
{
    GdkPixbuf *favicon = NULL;
    cairo_surface_t *surface = webkit_web_view_get_favicon(Ghtml.view);

    if (surface) {
        int width = cairo_image_surface_get_width(surface);
        int height = cairo_image_surface_get_height(surface);
        favicon = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);
        gtk_window_set_icon(Ghtml.window, favicon);
    }

}
