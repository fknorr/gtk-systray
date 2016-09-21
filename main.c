#include <gtk/gtk.h>
#include "systray-box.h"

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *tray = systray_box_new();
    gtk_container_add(GTK_CONTAINER(win), tray);
    gtk_widget_show_all(win);
    systray_box_update(SYSTRAY_BOX(tray));

    gtk_main();
    return 0;
}

