#include <gtk/gtk.h>
#include "systray.h"

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *tray = systray_new();
    gtk_container_add(GTK_CONTAINER(win), tray);
    gtk_widget_show_all(win);

    g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_main();
    return 0;
}

