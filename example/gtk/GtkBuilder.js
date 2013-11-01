
var Gtk3 = new SharedLibrary('libgtk-3.so');

var GTK_BUILDER_ERROR_INVALID_TYPE_FUNCTION = 0,
GTK_BUILDER_ERROR_UNHANDLED_TAG = 1,
GTK_BUILDER_ERROR_MISSING_ATTRIBUTE = 2,
GTK_BUILDER_ERROR_INVALID_ATTRIBUT = 3,
GTK_BUILDER_ERROR_INVALID_TAG = 4,
GTK_BUILDER_ERROR_MISSING_PROPERTY_VALUE = 5,
GTK_BUILDER_ERROR_INVALID_VALUE = 6,
GTK_BUILDER_ERROR_VERSION_MISMATCH = 7,
GTK_BUILDER_ERROR_DUPLICATE_ID = 8,
GTK_BUILDER_ERROR_OBJECT_TYPE_REFUSED = 9,
GTK_BUILDER_ERROR_TEMPLATE_MISMATCH = 10;

js.extend(js.type, {
	'GtkBuilder *': js.type.pointer,
	'GObject *': js.type.pointer,
	'gchar *': js.type.utf8 | js.type.pointer;
	'const gchar *': js.type.utf8 | js.type.pointer;
	'gchar **': js.type.utf8 | js.type.pointer;
	'gssize': (js.type.size - 1),
	'GCallback': js.type.pointer,
	'GError **': js.type.pointer,
	'gsize': js.type.pointer,
	'gpointer': js.type.pointer,
	'GtkBuilderConnectFunc': js.type.pointer,
	'GParamSpec *': js.type.pointer,
	'GType': js.type.int,
	'GValue *': js.type.pointer
	'guint': js.type.int | js.type.unsigned,
	'gboolean': js.type.boolean,
})

gtk_builder_new = new Procedure(Gtk3, 'GtkBuilder *', 'gtk_builder_new', ['void']);
gtk_builder_new_from_file = new Procedure(Gtk3, 'GtkBuilder *', 'gtk_builder_new_from_file', ['const gchar *']);
gtk_builder_new_from_resource = new Procedure(Gtk3, 'GtkBuilder *', 'gtk_builder_new_from_resource', ['const gchar *']);
gtk_builder_new_from_string = new Procedure(Gtk3, 'GtkBuilder *', 'gtk_builder_new_from_string', ['const gchar *', 'gssize']);
gtk_builder_add_callback_symbol = new Procedure(Gtk3, 'void', 'gtk_builder_add_callback_symbol', ['GtkBuilder *', 'const gchar *', 'GCallback']);
gtk_builder_add_callback_symbols = new Procedure(Gtk3, 'void', 'gtk_builder_add_callback_symbols', ['GtkBuilder *', 'const gchar *', 'GCallback', '...']);
gtk_builder_lookup_callback_symbol = new Procedure(Gtk3, 'GCallback', 'gtk_builder_lookup_callback_symbol', ['GtkBuilder *', 'const gchar *']);
gtk_builder_add_from_file = new Procedure(Gtk3, 'guint', 'gtk_builder_add_from_file', ['GtkBuilder *', 'const gchar *', 'GError **']);
gtk_builder_add_from_resource = new Procedure(Gtk3, 'guint', 'gtk_builder_add_from_resource', ['GtkBuilder *', 'const gchar *', 'GError **']);
gtk_builder_add_from_string = new Procedure(Gtk3, 'guint', 'gtk_builder_add_from_string', ['GtkBuilder *', 'const gchar *', 'gsize', 'GError **']);
gtk_builder_add_objects_from_file = new Procedure(Gtk3, 'guint', 'gtk_builder_add_objects_from_file', ['GtkBuilder *', 'const gchar *', 'gchar **', 'GError **']);
gtk_builder_add_objects_from_string = new Procedure(Gtk3, 'guint', 'gtk_builder_add_objects_from_string', ['GtkBuilder *', 'const gchar *', 'gsize', 'gchar **', 'GError **']);
gtk_builder_add_objects_from_resource = new Procedure(Gtk3, 'guint', 'gtk_builder_add_objects_from_resource', ['GtkBuilder *', 'const gchar *', 'gchar **', 'GError **']);
gtk_builder_get_object = new Procedure(Gtk3, 'GObject *', 'gtk_builder_get_object', ['GtkBuilder *', 'const gchar *']);
gtk_builder_get_objects = new Procedure(Gtk3, 'GSList *', 'gtk_builder_get_objects', ['GtkBuilder *']);
gtk_builder_expose_object = new Procedure(Gtk3, 'void', 'gtk_builder_expose_object', ['GtkBuilder *', 'const gchar *', 'GObject *']);
gtk_builder_connect_signals = new Procedure(Gtk3, 'void', 'gtk_builder_connect_signals', ['GtkBuilder *', 'gpointer']);
gtk_builder_connect_signals_full = new Procedure(Gtk3, 'void', 'gtk_builder_connect_signals_full', ['GtkBuilder *', 'GtkBuilderConnectFunc', 'gpointer']);
gtk_builder_set_translation_domain = new Procedure(Gtk3, 'void', 'gtk_builder_set_translation_domain', ['GtkBuilder *', 'const gchar *']);
gtk_builder_get_translation_domain = new Procedure(Gtk3, 'const gchar *', 'gtk_builder_get_translation_domain', ['GtkBuilder *']);
gtk_builder_get_type_from_name = new Procedure(Gtk3, 'GType', 'gtk_builder_get_type_from_name', ['GtkBuilder *', 'const char *']);
gtk_builder_value_from_string = new Procedure(Gtk3, 'gboolean', 'gtk_builder_value_from_string', ['GtkBuilder *', 'GParamSpec *', 'const gchar *', 'GValue *', 'GError **']);
gtk_builder_value_from_string_type = new Procedure(Gtk3, 'gboolean', 'gtk_builder_value_from_string_type', ['GtkBuilder *', 'GType type', 'const gchar *', 'GValue *', 'GError **']);

