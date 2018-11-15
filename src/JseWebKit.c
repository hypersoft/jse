#include <JavaScriptCore/JavaScript.h>
#include <webkit2/webkit-web-extension.h>
#include "jse.h"

static void
window_object_cleared_callback (WebKitScriptWorld *world,
                                WebKitWebPage     *web_page,
                                WebKitFrame       *frame,
                                gpointer           value)
{
	if (! g_str_has_prefix(webkit_web_page_get_uri(web_page), "file")) return;

    JSContext   ctx = webkit_frame_get_javascript_context_for_script_world (frame, world);

    JSInit("jseWebKit", ctx, true);
    JSInlineEval(ctx, "loadPlugin('Ghtml.jso');", NULL, NULL);

}

void webkit_web_extension_initialize (WebKitWebExtension *extension)
{
    g_signal_connect (webkit_script_world_get_default (),
                      "window-object-cleared",
                      G_CALLBACK (window_object_cleared_callback),
                      NULL);
}

void webkit_web_extension_initialize_with_user_data(WebKitWebExtension * ext, GVariant * data)
{
    g_signal_connect (webkit_script_world_get_default (),
                      "window-object-cleared",
                      G_CALLBACK (window_object_cleared_callback),
                      data);
}