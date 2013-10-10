#!bin/jse

// exceptions will hurt you unless you develop an allocation stack to keep track of pointers
// and catch those exceptions!

var lib = new SharedLibrary("libc.so.6");
var cvm = new CallVM(8);
var utf8 = js.native.toUTF8("Hello World");
cvm.push([4], utf8);      /* 4 == int_t */
cvm.call(4, lib.find("puts"));

// yeah.. so clean that shit up...

js.native.freeUTF8(utf8);
cvm.free();
lib.release();

Gtk = imports.gi.Gtk;
Gtk.init(null, null);

var window = new Gtk.Window();
window.signal.hide.connect(function () { Gtk.main_quit(); });
window.show_all();

Gtk.main();

