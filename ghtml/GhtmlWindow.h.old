
#ifndef GhtmlWindow_h
#define GhtmlWindow_h

#include <webkit2/webkit2.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GHTML_TYPE_WINDOW            (ghtml_window_get_type())
#define GHTML_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GHTML_TYPE_WINDOW, GhtmlWindow))
#define GHTML_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  GHTML_TYPE_WINDOW, GhtmlWindowClass))
#define GHTML_IS_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GHTML_TYPE_WINDOW))
#define GHTML_IS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  GHTML_TYPE_WINDOW))
#define GHTML_WINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  GHTML_TYPE_WINDOW, GhtmlWindowClass))

typedef struct _GhtmlWindow        GhtmlWindow;
typedef struct _GhtmlWindowClass   GhtmlWindowClass;

GType ghtml_window_get_type(void);

GtkWidget* ghtml_window_new(WebKitWebView*, GtkWindow*);
WebKitWebView* ghtml_window_get_view(GhtmlWindow*);
void ghtml_window_load_uri(GhtmlWindow *, const char *uri);

G_END_DECLS

#endif
