#!/usr/bin/jse --shell-script

echo('Be quiet for a second, you might learn something..');

var gtk3 = new SharedLibrary('libgtk-3.so');
var gtk_init = new SharedFunction(gtk3, Int, "gtk_init", Pointer, Pointer);
var gtk_main = new SharedFunction(gtk3, Int, 'gtk_main');
var gtk_widget_show_all = new SharedFunction(gtk3, Void, 'gtk_widget_show_all', Pointer);
var gtk_window_new = new SharedFunction(gtk3, Pointer, "gtk_window_new", Int);
var gtk_window_set_title = new SharedFunction(gtk3, Void, "gtk_window_set_title", Pointer, Pointer);
var gtk_container_set_border_width = new SharedFunction(gtk3, Void, 'gtk_container_set_border_width', Pointer, Int);
var gtk_button_new_with_label = new SharedFunction(gtk3, Pointer, 'gtk_button_new_with_label', Pointer);
var gtk_container_add = new SharedFunction(gtk3, Void, 'gtk_container_add', Pointer, Pointer);
var gtk_window_set_default_size = new SharedFunction(gtk3, Void, 'gtk_window_set_default_size', Pointer, Int, Int);
var gtk_window_set_position = new SharedFunction(gtk3, Void, 'gtk_window_set_position', Pointer, Int);

var gobj = new SharedLibrary('libgobject-2.0.so');
var G_CONNECT_SWAPPED = 1 << 1;
var GTK_WIN_POS_CENTER = 1;

var g_signal_connect_data = new SharedFunction(gobj, ULong, 'g_signal_connect_data', Pointer, UInt8.toPointer(), Pointer, Pointer, Pointer, Int);

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
g_signal_connect(window, "destroy", gtk3.gtk_main_quit, 0);
gtk_container_set_border_width(window, 10);
gtk_window_set_default_size(window, 230, 150);
gtk_window_set_position(window, GTK_WIN_POS_CENTER);

button = gtk_button_new_with_label ("Hello World");
g_signal_connect (button, "clicked", new FunctionCallback(this, Void, print_hello, Pointer, Pointer), 0);
g_signal_connect_swapped(button, "clicked", gtk3.gtk_widget_destroy, window);

gtk_container_add(window, button)

gtk_widget_show_all(window);

gtk_main ();

echo("If you didn't know before you're going to learn today...");

