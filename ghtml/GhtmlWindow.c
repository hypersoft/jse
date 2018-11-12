#include "GhtmlWindow.h"
#include <gdk/gdkkeysyms.h>
#include <string.h>

enum {
    PROP_0,

    PROP_VIEW
};

struct _GhtmlWindow {
    GtkWindow parent;
    GtkAccelGroup *accelGroup;
    GtkWidget *mainBox;
    GtkWidget *statusLabel;
    WebKitWebView *webView;
    GdkPixbuf *favicon;
    GtkWindow *parentWindow;
};

struct _GhtmlWindowClass {
    GtkWindowClass parent;
};

static const char *defaultWindowTitle = "ghtml";
static const char *miniGhtmlAboutScheme = "ghtml-about";
static const gdouble minimumZoomLevel = 0.5;
static const gdouble maximumZoomLevel = 3;
static const gdouble zoomStep = 1.2;
static gint windowCount = 0;

G_DEFINE_TYPE(GhtmlWindow, ghtml_window, GTK_TYPE_WINDOW)

static char *getInternalURI(const char *uri)
{
    // Internally we use ghtml-about: as about: prefix is ignored by WebKit.
    if (g_str_has_prefix(uri, "about:") && !g_str_equal(uri, "about:blank"))
        return g_strconcat(miniGhtmlAboutScheme, uri + strlen ("about"), NULL);

    return g_strdup(uri);
}

static char *getExternalURI(const char *uri)
{
    // From the user point of view we support about: prefix.
    if (g_str_has_prefix(uri, miniGhtmlAboutScheme))
        return g_strconcat("about", uri + strlen(miniGhtmlAboutScheme), NULL);

    return g_strdup(uri);
}

static void ghtmlWindowSetStatusText(GhtmlWindow *window, const char *text)
{
    gtk_label_set_text(GTK_LABEL(window->statusLabel), text);
    gtk_widget_set_visible(window->statusLabel, !!text);
}

static void resetStatusText(GtkWidget *widget, GhtmlWindow *window)
{
    ghtmlWindowSetStatusText(window, NULL);
}

static void webViewTitleChanged(WebKitWebView *webView, GParamSpec *pspec, GhtmlWindow *window)
{
    const char *title = webkit_web_view_get_title(webView);
    gtk_window_set_title(GTK_WINDOW(window), title ? title : defaultWindowTitle);
}

static void geolocationRequestDialogCallback(GtkDialog *dialog, gint response, WebKitPermissionRequest *request)
{
    switch (response) {
    case GTK_RESPONSE_YES:
        webkit_permission_request_allow(request);
        break;
    default:
        webkit_permission_request_deny(request);
        break;
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
    g_object_unref(request);
}

static void webViewClose(WebKitWebView *webView, GhtmlWindow *window)
{
    gtk_widget_destroy(GTK_WIDGET(window));
}

static void webViewRunAsModal(WebKitWebView *webView, GhtmlWindow *window)
{
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window), window->parentWindow);
}

static void webViewReadyToShow(WebKitWebView *webView, GhtmlWindow *window)
{
    WebKitWindowProperties *windowProperties = webkit_web_view_get_window_properties(webView);

    GdkRectangle geometry;
    webkit_window_properties_get_geometry(windowProperties, &geometry);
    if (geometry.x >= 0 && geometry.y >= 0)
        gtk_window_move(GTK_WINDOW(window), geometry.x, geometry.y);
    if (geometry.width > 0 && geometry.height > 0)
        gtk_window_resize(GTK_WINDOW(window), geometry.width, geometry.height);

    if (!webkit_window_properties_get_resizable(windowProperties))
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    gtk_widget_show(GTK_WIDGET(window));
}

static GtkWidget *webViewCreate(WebKitWebView *webView, WebKitNavigationAction *navigation, GhtmlWindow *window)
{
    WebKitWebView *newWebView = WEBKIT_WEB_VIEW(webkit_web_view_new_with_related_view(webView));
    webkit_web_view_set_settings(newWebView, webkit_web_view_get_settings(webView));
    GtkWidget *newWindow = ghtml_window_new(newWebView, GTK_WINDOW(window));
    g_signal_connect(newWebView, "ready-to-show", G_CALLBACK(webViewReadyToShow), newWindow);
    g_signal_connect(newWebView, "run-as-modal", G_CALLBACK(webViewRunAsModal), newWindow);
    g_signal_connect(newWebView, "close", G_CALLBACK(webViewClose), newWindow);
    return GTK_WIDGET(newWebView);
}

static gboolean webViewDecidePolicy(WebKitWebView *webView, WebKitPolicyDecision *decision, WebKitPolicyDecisionType decisionType, GhtmlWindow *window)
{
    switch (decisionType) {
    case WEBKIT_POLICY_DECISION_TYPE_NAVIGATION_ACTION: {
        WebKitNavigationAction *navigationAction = webkit_navigation_policy_decision_get_navigation_action(WEBKIT_NAVIGATION_POLICY_DECISION(decision));
        if (webkit_navigation_action_get_navigation_type(navigationAction) != WEBKIT_NAVIGATION_TYPE_LINK_CLICKED
            || webkit_navigation_action_get_mouse_button(navigationAction) != GDK_BUTTON_MIDDLE)
            return FALSE;

        // Opening a new window if link clicked with the middle button.
        WebKitWebView *newWebView = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(webkit_web_view_get_context(webView)));
        GtkWidget *newWindow = ghtml_window_new(newWebView, GTK_WINDOW(window));
        webkit_web_view_load_request(newWebView, webkit_navigation_action_get_request(navigationAction));
        gtk_widget_show(newWindow);

        webkit_policy_decision_ignore(decision);
        return TRUE;
    }
    case WEBKIT_POLICY_DECISION_TYPE_RESPONSE: {
        WebKitResponsePolicyDecision *responseDecision = WEBKIT_RESPONSE_POLICY_DECISION(decision);
        if (webkit_response_policy_decision_is_mime_type_supported(responseDecision))
            return FALSE;

        WebKitWebResource *mainResource = webkit_web_view_get_main_resource(webView);
        WebKitURIRequest *request = webkit_response_policy_decision_get_request(responseDecision);
        const char *requestURI = webkit_uri_request_get_uri(request);
        if (g_strcmp0(webkit_web_resource_get_uri(mainResource), requestURI))
            return FALSE;

        webkit_policy_decision_download(decision);
        return TRUE;
    }
    case WEBKIT_POLICY_DECISION_TYPE_NEW_WINDOW_ACTION:
    default:
        return FALSE;
    }
}

static gboolean webViewDecidePermissionRequest(WebKitWebView *webView, WebKitPermissionRequest *request, GhtmlWindow *window)
{
    if (!WEBKIT_IS_GEOLOCATION_PERMISSION_REQUEST(request))
        return FALSE;

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_QUESTION,
                                               GTK_BUTTONS_YES_NO,
                                               "Geolocation request");

    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "Allow geolocation request?");
    g_signal_connect(dialog, "response", G_CALLBACK(geolocationRequestDialogCallback), g_object_ref(request));
    gtk_widget_show(dialog);

    return TRUE;
}

static void webViewMouseTargetChanged(WebKitWebView *webView, WebKitHitTestResult *hitTestResult, guint mouseModifiers, GhtmlWindow *window)
{
    if (!webkit_hit_test_result_context_is_link(hitTestResult)) {
        ghtmlWindowSetStatusText(window, NULL);
        return;
    }
    ghtmlWindowSetStatusText(window, webkit_hit_test_result_get_link_uri(hitTestResult));
}

static void faviconChanged(GObject *object, GParamSpec *paramSpec, GhtmlWindow *window)
{
    GdkPixbuf *favicon = NULL;
    cairo_surface_t *surface = webkit_web_view_get_favicon(window->webView);

    if (surface) {
        int width = cairo_image_surface_get_width(surface);
        int height = cairo_image_surface_get_height(surface);
        favicon = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);
    }

    if (window->favicon)
        g_object_unref(window->favicon);
    window->favicon = favicon;

}

static void reloadPage(GhtmlWindow *window, gpointer user_data)
{
    webkit_web_view_reload(window->webView);
}

static void reloadPageIgnoringCache(GhtmlWindow *window, gpointer user_data)
{
    webkit_web_view_reload_bypass_cache(window->webView);
}

static void ghtmlWindowFinalize(GObject *gObject)
{
    GhtmlWindow *window = GHTML_WINDOW(gObject);
    if (window->favicon) {
        g_object_unref(window->favicon);
        window->favicon = NULL;
    }

    if (window->accelGroup) {
        g_object_unref(window->accelGroup);
        window->accelGroup = NULL;
    }

    G_OBJECT_CLASS(ghtml_window_parent_class)->finalize(gObject);

    if (g_atomic_int_dec_and_test(&windowCount))
        gtk_main_quit();
}

static void ghtmlWindowGetProperty(GObject *object, guint propId, GValue *value, GParamSpec *pspec)
{
    GhtmlWindow *window = GHTML_WINDOW(object);

    switch (propId) {
    case PROP_VIEW:
        g_value_set_object(value, ghtml_window_get_view(window));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
    }
}

static void ghtmlWindowSetProperty(GObject *object, guint propId, const GValue *value, GParamSpec *pspec)
{
    GhtmlWindow* window = GHTML_WINDOW(object);

    switch (propId) {
    case PROP_VIEW:
        window->webView = g_value_get_object(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
    }
}

static void ghtml_window_init(GhtmlWindow *window)
{
    g_atomic_int_inc(&windowCount);

    gtk_window_set_title(GTK_WINDOW(window), defaultWindowTitle);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    window->mainBox = vbox;

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);

}

static void ghtmlWindowConstructed(GObject *gObject)
{
    GhtmlWindow *window = GHTML_WINDOW(gObject);
	GdkScreen *screen = gtk_widget_get_screen (GTK_WIDGET (window));
	GdkVisual *visual = gdk_screen_get_rgba_visual (screen);

	if (visual == NULL)
	visual = gdk_screen_get_system_visual (screen);

	gtk_widget_set_visual (GTK_WIDGET (window), visual);

    g_signal_connect(window->webView, "notify::title", G_CALLBACK(webViewTitleChanged), window);
    g_signal_connect(window->webView, "create", G_CALLBACK(webViewCreate), window);
    g_signal_connect(window->webView, "decide-policy", G_CALLBACK(webViewDecidePolicy), window);
    g_signal_connect(window->webView, "permission-request", G_CALLBACK(webViewDecidePermissionRequest), window);
    g_signal_connect(window->webView, "mouse-target-changed", G_CALLBACK(webViewMouseTargetChanged), window);
    g_signal_connect(window->webView, "notify::favicon", G_CALLBACK(faviconChanged), window);
    g_signal_connect(window->webView, "close", G_CALLBACK(webViewClose), window);

    GtkWidget *overlay = gtk_overlay_new();
    gtk_box_pack_start(GTK_BOX(window->mainBox), overlay, TRUE, TRUE, 0);
    gtk_widget_show(overlay);

    window->statusLabel = gtk_label_new(NULL);
    gtk_widget_set_halign(window->statusLabel, GTK_ALIGN_START);
    gtk_widget_set_valign(window->statusLabel, GTK_ALIGN_END);
    gtk_widget_set_margin_start(window->statusLabel, 1);
    gtk_widget_set_margin_end(window->statusLabel, 1);
    gtk_widget_set_margin_top(window->statusLabel, 1);
    gtk_widget_set_margin_bottom(window->statusLabel, 1);
//    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), window->statusLabel);

    gtk_container_add(GTK_CONTAINER(overlay), GTK_WIDGET(window->webView));

    gtk_widget_show(GTK_WIDGET(window->webView));
}

static void ghtml_window_class_init(GhtmlWindowClass *klass)
{
    GObjectClass *gobjectClass = G_OBJECT_CLASS(klass);

    gobjectClass->constructed = ghtmlWindowConstructed;
    gobjectClass->get_property = ghtmlWindowGetProperty;
    gobjectClass->set_property = ghtmlWindowSetProperty;
    gobjectClass->finalize = ghtmlWindowFinalize;

    g_object_class_install_property(gobjectClass,
                                    PROP_VIEW,
                                    g_param_spec_object("view",
                                                        "View",
                                                        "The web view of this window",
                                                        WEBKIT_TYPE_WEB_VIEW,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

// Public API.
GtkWidget *ghtml_window_new(WebKitWebView *view, GtkWindow *parent)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_VIEW(view), 0);

    GhtmlWindow *window = GHTML_WINDOW(g_object_new(GHTML_TYPE_WINDOW,
        "type", GTK_WINDOW_TOPLEVEL, "view", view, NULL));

    if (parent) {
        window->parentWindow = parent;
        g_object_add_weak_pointer(G_OBJECT(parent), (gpointer *)&window->parentWindow);
    }

    return GTK_WIDGET(window);
}

WebKitWebView *ghtml_window_get_view(GhtmlWindow *window)
{
    g_return_val_if_fail(GHTML_IS_WINDOW(window), 0);

    return window->webView;
}

void ghtml_window_load_uri(GhtmlWindow *window, const char *uri)
{
    g_return_if_fail(GHTML_IS_WINDOW(window));
    g_return_if_fail(uri);

    if (!g_str_has_prefix(uri, "javascript:")) {
        char *internalURI = getInternalURI(uri);
        webkit_web_view_load_uri(window->webView, internalURI);
        g_free(internalURI);
        return;
    }

    webkit_web_view_run_javascript(window->webView, strstr(uri, "javascript:"), NULL, NULL, NULL);
}
