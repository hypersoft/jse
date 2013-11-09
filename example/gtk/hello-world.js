
var echo = Command('echo');
echo('Be quiet for a second, you might learn something..');

js.extend(native.type, {
	'GtkWidget *': native.type.pointer,
	'void *': native.type.void.pointer,
	'utf8 *': native.type.utf8.pointer,
	'ulong': native.type.long.unsigned,
})

var gtk3 = new SharedLibrary('libgtk-3.so');
var gtk_init = Procedure(gtk3, 'int', "gtk_init", ['void *', 'void *']);
var gtk_main = Procedure(gtk3, 'int', 'gtk_main', []);
var gtk_widget_show_all = Procedure(gtk3, 'void', 'gtk_widget_show_all', ['void *']);
var gtk_window_new = Procedure(gtk3, 'void *', "gtk_window_new", ['int']);
var gtk_window_set_title = Procedure(gtk3, 'void', "gtk_window_set_title", ['void *', 'utf8 *']);
var gtk_container_set_border_width = Procedure(gtk3, 'void', 'gtk_container_set_border_width', ['void *', 'int']);
var gtk_button_new_with_label = Procedure(gtk3, 'void *', 'gtk_button_new_with_label', ['utf8 *']);
var gtk_container_add = Procedure(gtk3, 'void', 'gtk_container_add', ['pointer', 'pointer']);
var gtk_window_set_default_size = Procedure(gtk3, 'void', 'gtk_window_set_default_size', ['void *', 'int', 'int']);
var gtk_window_set_position = Procedure(gtk3, 'void', 'gtk_window_set_position', ['void *', 'int']);

var gobj = new SharedLibrary('libgobject-2.0.so');
var G_CONNECT_SWAPPED = 1 << 1;
var GTK_WIN_POS_CENTER = 1;

var g_signal_connect_data = Procedure(gobj, 'ulong', 'g_signal_connect_data', [
	'pointer', 'utf8 *', 'pointer', 'pointer', 'pointer', 'int'
]);
var g_signal_connect = function(instance, detailed_signal, c_handler, data) {
	return g_signal_connect_data(instance, detailed_signal, c_handler, data, 0, 0);
}

var g_signal_connect_swapped = function(instance, detailed_signal, c_handler, data) {
	return g_signal_connect_data(instance, detailed_signal, c_handler, data, 0, G_CONNECT_SWAPPED);
}

function print_hello() {
	echo("Its your turn to say you've earned your way...");
	return 0;
}

gtk_init(0, 0);

window = gtk_window_new(0);
gtk_window_set_title(window, "JSE GTK+ 3.0 Example");
g_signal_connect(window, "destroy", gtk3.find('gtk_main_quit'), 0);
gtk_container_set_border_width(window, 10);
gtk_window_set_default_size(window, 230, 150);
gtk_window_set_position(window, GTK_WIN_POS_CENTER);

button = gtk_button_new_with_label ("Hello World");
g_signal_connect (button, "clicked", Callback(this, 'void', print_hello, ['void *', 'void *']), 0);
g_signal_connect_swapped(button, "clicked", gtk3.find('gtk_widget_destroy'), window);

gtk_container_add(window, button)

gtk_widget_show_all(window);

gtk_main ();

echo("If you didn't know before you're going to learn today...");

