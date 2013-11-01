
var gobj = new SharedLibrary('libgobject-2.0.so');

var g_signal_connect_data = Procedure(gobj, 'ulong', 'g_signal_connect_data', ['pointer', 'utf8 *', 'pointer', 'pointer', 'pointer', 'int']);

var g_signal_connect = function(instance, detailed_signal, c_handler, data) {
	return g_signal_connect_data(instance, detailed_signal, c_handler, data, 0, 0);
}
var g_signal_connect_swapped = function(instance, detailed_signal, c_handler, data) {
	return g_signal_connect_data(instance, detailed_signal, c_handler, data, 0, G_CONNECT_SWAPPED);
}

var gtk3 = new SharedLibrary('libgtk-3.so');
var gtk_init = Procedure(gtk3, 'int', "gtk_init", ['void *', 'void *']);
var gtk_message_dialog_new = Procedure(gtk3, 'pointer', 'gtk_message_dialog_new', ['pointer', 'int', 'int', 'int', 'utf8 *']);
var gtk_dialog_run = Procedure(gtk3, 'int', 'gtk_dialog_run', ['pointer']);

gtk_init(0, 0);

var G_CONNECT_SWAPPED = 1 << 1,
GTK_DIALOG_DESTROY_WITH_PARENT = 1 << 1, GTK_MESSAGE_INFO = 0, GTK_BUTTONS_CLOSE = 2;

dialog = gtk_message_dialog_new (0, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "This is a silly billy info box");
g_signal_connect_swapped (dialog, "response", gtk3.find('gtk_widget_destroy'), dialog);
gtk_dialog_run(dialog);

