/*
 * Copyright (c) 2005-2007 Jasper Huijsmans <jasper@xfce.org>
 * Copyright (c) 2007-2010 Nick Schermer <nick@xfce.org>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include "systray.h"
#include "systray-box.h"
#include "systray-socket.h"
#include "systray-manager.h"

#include <string.h>

#define ICON_SIZE     (22)
#define BUTTON_SIZE   (16)
#define FRAME_SPACING (1)


static void     systray_get_property                 (GObject               *object,
                                                             guint                  prop_id,
                                                             GValue                *value,
                                                             GParamSpec            *pspec);
static void     systray_set_property                 (GObject               *object,
                                                             guint                  prop_id,
                                                             const GValue          *value,
                                                             GParamSpec            *pspec);
static void     systray_construct                    (GtkWidget       *panel_plugin);
static void     systray_free_data                    (GtkWidget       *panel_plugin);
static void     systray_orientation_changed          (GtkWidget       *panel_plugin,
                                                             GtkOrientation         orientation);
static gboolean systray_size_changed                 (GtkWidget       *panel_plugin,
                                                             gint                   size);
static void     systray_configure_plugin             (GtkWidget       *panel_plugin);
static void     systray_box_expose_event             (GtkWidget             *box,
                                                            cairo_t *cr);
static void     systray_button_toggled               (GtkWidget             *button,
                                                             Systray         *plugin);
static void     systray_button_set_arrow             (Systray         *plugin);
static void     systray_names_collect_visible        (gpointer               key,
                                                             gpointer               value,
                                                             gpointer               user_data);
static void     systray_names_collect_hidden         (gpointer               key,
                                                             gpointer               value,
                                                             gpointer               user_data);
static gboolean systray_names_remove                 (gpointer               key,
                                                             gpointer               value,
                                                             gpointer               user_data);
static void     systray_names_update                 (Systray         *plugin);
static gboolean systray_names_get_hidden             (Systray         *plugin,
                                                             const gchar           *name);
static void     systray_icon_added                   (SystrayManager        *manager,
                                                             GtkWidget             *icon,
                                                             Systray         *plugin);
static void     systray_icon_removed                 (SystrayManager        *manager,
                                                             GtkWidget             *icon,
                                                             Systray         *plugin);
static void     systray_lost_selection               (SystrayManager        *manager,
                                                             Systray         *plugin);
static void     systray_dialog_add_application_names (gpointer               key,
                                                             gpointer               value,
                                                             gpointer               user_data);
static void     systray_dialog_hidden_toggled        (GtkCellRendererToggle *renderer,
                                                             const gchar           *path_string,
                                                             Systray         *plugin);
static void     systray_dialog_clear_clicked         (GtkWidget             *button,
                                                             Systray         *plugin);



struct _SystrayClass
{
  GtkBoxClass __parent__;
};

struct _Systray
{
  GtkBoxClass __parent__;

  /* systray manager */
  SystrayManager *manager;

  guint           idle_startup;

  /* widgets */
  GtkWidget      *box;

  /* settings */
  GHashTable     *names;
};

enum
{
  PROP_0,
  PROP_SIZE_MAX,
  PROP_NAMES_HIDDEN,
  PROP_NAMES_VISIBLE
};

enum
{
  COLUMN_PIXBUF,
  COLUMN_TITLE,
  COLUMN_HIDDEN,
  COLUMN_INTERNAL_NAME
};


G_DEFINE_TYPE(Systray, systray, GTK_TYPE_BOX)


/* known applications to improve the icon and name */
static const gchar *known_applications[][3] =
{
  /* application name, icon-name, understandable name */
  { "networkmanager applet", "network-workgroup", "Network Manager Applet" },
  { "thunar", "Thunar", "Thunar Progress Dialog" },
  { "workrave", NULL, "Workrave" },
  { "workrave tray icon", NULL, "Workrave Applet" },
  { "audacious2", "audacious", "Audacious" },
  { "wicd-client.py", "wicd-gtk", "Wicd" },
  { "xfce4-power-manager", "xfpm-ac-adapter", "Xfce Power Manager" },
};



static void
systray_class_init (SystrayClass *klass)
{
  GtkWidgetClass *plugin_class;
  GObjectClass         *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->get_property = systray_get_property;
  gobject_class->set_property = systray_set_property;

  g_object_class_install_property (gobject_class,
                                   PROP_SIZE_MAX,
                                   g_param_spec_uint ("size-max",
                                                      NULL, NULL,
                                                      SIZE_MAX_MIN,
                                                      SIZE_MAX_MAX,
                                                      SIZE_MAX_DEFAULT,
                                                      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_NAMES_HIDDEN,
                                   g_param_spec_boxed ("names-hidden",
                                                       NULL, NULL,
                                                       G_TYPE_STRV,
                                                       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_NAMES_VISIBLE,
                                   g_param_spec_boxed ("names-visible",
                                                       NULL, NULL,
                                                       G_TYPE_STRV,
                                                       G_PARAM_READWRITE));
}


static void
systray_init (Systray *plugin)
{
  GtkRcStyle *style;

  plugin->manager = NULL;
  plugin->idle_startup = 0;
  plugin->names = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

  plugin->box = systray_box_new ();
  systray_box_set_show_hidden(SYSTRAY_BOX(plugin->box), TRUE);
  gtk_container_add (GTK_CONTAINER (plugin), plugin->box);
  g_signal_connect (G_OBJECT (plugin->box), "draw",
      G_CALLBACK (systray_box_expose_event), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (plugin->box), FRAME_SPACING);
  gtk_widget_show (plugin->box);

  g_signal_connect_after(G_OBJECT(plugin), "draw", G_CALLBACK(systray_construct), NULL);
}


GtkWidget *
systray_new(void)
{
    return g_object_new(TYPE_SYSTRAY, NULL);
}


static void
systray_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  Systray *plugin = SYSTRAY (object);
  GPtrArray     *array;

  switch (prop_id)
    {
    case PROP_SIZE_MAX:
      g_value_set_uint (value,
          systray_box_get_size_max (SYSTRAY_BOX (plugin->box)));
      break;

   case PROP_NAMES_VISIBLE:
      array = g_ptr_array_new ();
      g_hash_table_foreach (plugin->names, systray_names_collect_visible, array);
      g_ptr_array_add(array, NULL);
      g_value_set_boxed (value, array->pdata);
      g_ptr_array_free (array, TRUE);
      break;

    case PROP_NAMES_HIDDEN:
      array = g_ptr_array_new ();
      g_hash_table_foreach (plugin->names, systray_names_collect_hidden, array);
      g_ptr_array_add(array, NULL);
      g_value_set_boxed (value, array->pdata);
      g_ptr_array_free (array, TRUE);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
systray_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  Systray *plugin = SYSTRAY (object);
  gboolean       hidden = TRUE;
  GPtrArray     *array;
  const GValue  *tmp;
  gchar         *name;
  guint          i;
  GtkRcStyle    *style;

  switch (prop_id)
    {
    case PROP_SIZE_MAX:
      systray_box_set_size_max (SYSTRAY_BOX (plugin->box),
                                g_value_get_uint (value));
      break;

    case PROP_NAMES_VISIBLE:
      hidden = FALSE;
      /* fall-though */

    case PROP_NAMES_HIDDEN:
      /* remove old names with this state */
      g_hash_table_foreach_remove (plugin->names,
                                   systray_names_remove,
                                   GUINT_TO_POINTER (hidden));

      /* add new values */
      array = g_value_get_boxed (value);
      if (G_LIKELY (array != NULL))
        {
          for (i = 0; i < array->len; i++)
            {
              tmp = g_ptr_array_index (array, i);
              g_assert (G_VALUE_HOLDS_STRING (tmp));
              name = g_value_dup_string (tmp);
              g_hash_table_replace (plugin->names, name, GUINT_TO_POINTER (hidden));
            }
        }

      /* update icons in the box */
      systray_names_update (plugin);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static gboolean
systray_screen_changed_idle (gpointer user_data)
{
  Systray *plugin = SYSTRAY (user_data);
  GdkScreen     *screen;
  GError        *error = NULL;

  /* create a new manager and register this screen */
  plugin->manager = systray_manager_new ();
  g_signal_connect (G_OBJECT (plugin->manager), "icon-added",
      G_CALLBACK (systray_icon_added), plugin);
  g_signal_connect (G_OBJECT (plugin->manager), "icon-removed",
      G_CALLBACK (systray_icon_removed), plugin);
  g_signal_connect (G_OBJECT (plugin->manager), "lost-selection",
      G_CALLBACK (systray_lost_selection), plugin);

  /* try to register the systray */
  screen = gtk_widget_get_screen (GTK_WIDGET (plugin));
  if (systray_manager_register (plugin->manager, screen, &error))
    {
        systray_orientation_changed(GTK_WIDGET(plugin), GTK_ORIENTATION_HORIZONTAL);
      /* send the plugin orientation
      systray_orientation_changed (PANEL_PLUGIN (plugin),
         xfce_panel_plugin_get_orientation (PANEL_PLUGIN (plugin)));
        */
    }
  else
    {
      g_error("Unable to start the notification area");
      g_error_free (error);
    }

  return FALSE;
}



static void
systray_screen_changed_idle_destroyed (gpointer user_data)
{
  SYSTRAY (user_data)->idle_startup = 0;
}



static void
systray_screen_changed (GtkWidget *widget,
                               GdkScreen *previous_screen)
{
  Systray *plugin = SYSTRAY (widget);

  if (G_UNLIKELY (plugin->manager != NULL))
    {
      /* unregister this screen screen */
      systray_manager_unregister (plugin->manager);
      g_object_unref (G_OBJECT (plugin->manager));
      plugin->manager = NULL;
    }

  /* schedule a delayed startup */
  if (plugin->idle_startup == 0)
    plugin->idle_startup = g_idle_add_full (G_PRIORITY_LOW, systray_screen_changed_idle,
                                            plugin, systray_screen_changed_idle_destroyed);
}


static void
systray_composited_changed (GtkWidget *widget)
{
  /* restart the manager to add the sockets again */
  systray_screen_changed (widget, gtk_widget_get_screen (widget));
}



static void
systray_construct (GtkWidget *plugin)
{
  /* monitor screen changes */
  g_signal_connect (G_OBJECT (plugin), "screen-changed",
      G_CALLBACK (systray_screen_changed), NULL);
  systray_screen_changed (GTK_WIDGET (plugin), NULL);

  /* restart internally if compositing changed */
  g_signal_connect (G_OBJECT (plugin), "composited-changed",
     G_CALLBACK (systray_composited_changed), NULL);

  systray_screen_changed (GTK_WIDGET (plugin), NULL);

  g_signal_handlers_disconnect_by_func(G_OBJECT(plugin), systray_construct, NULL);
}



static void
systray_free_data (GtkWidget *panel_plugin)
{
  Systray *plugin = SYSTRAY (panel_plugin);

  /* stop pending idle startup */
  if (plugin->idle_startup != 0)
    g_source_remove (plugin->idle_startup);

  /* disconnect screen changed signal */
  g_signal_handlers_disconnect_by_func (G_OBJECT (plugin),
      systray_screen_changed, NULL);

  g_hash_table_destroy (plugin->names);

  if (G_LIKELY (plugin->manager != NULL))
    {
      systray_manager_unregister (plugin->manager);
      g_object_unref (G_OBJECT (plugin->manager));
    }
}



static void
systray_orientation_changed (GtkWidget *panel_plugin,
                                    GtkOrientation   orientation)
{
  Systray *plugin = SYSTRAY (panel_plugin);

  //xfce_hvbox_set_orientation (HVBOX (plugin->hbox), orientation);
  systray_box_set_orientation (SYSTRAY_BOX (plugin->box), orientation);

  if (G_LIKELY (plugin->manager != NULL))
    systray_manager_set_orientation (plugin->manager, orientation);

  /*
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    gtk_widget_set_size_request (plugin->button, BUTTON_SIZE, -1);
  else
    gtk_widget_set_size_request (plugin->button, -1, BUTTON_SIZE);

  systray_button_set_arrow (plugin);
  */
}



static gboolean
systray_size_changed (GtkWidget *panel_plugin,
                             gint             size)
{
  Systray *plugin = SYSTRAY (panel_plugin);
  systray_box_set_size_alloc (SYSTRAY_BOX (plugin->box), size);

  return TRUE;
}



static void
systray_configure_plugin (GtkWidget *panel_plugin)
{
}



static void
systray_box_expose_event_icon (GtkWidget *child,
                                      gpointer   user_data)
{
  cairo_t       *cr = user_data;
  GtkAllocation alloc;

  if (systray_socket_is_composited (SYSTRAY_SOCKET (child)))
    {
        gtk_widget_get_allocation(child, &alloc);

      /* skip hidden (see offscreen in box widget) icons */
      if (alloc.x > -1 && alloc.y > -1)
        {
          gdk_cairo_set_source_window (cr, gtk_widget_get_window (child),
                                       alloc.x, alloc.y);
          cairo_paint (cr);
        }
    }
}



static void
systray_box_expose_event (GtkWidget      *box,
                                 cairo_t *cr)
{
  if (!gtk_widget_is_composited (box))
    return;

  if (G_LIKELY (cr != NULL))
    {
      /* separately draw all the composed tray icons after gtk
       * handled the expose event */
      gtk_container_foreach (GTK_CONTAINER (box),
          systray_box_expose_event_icon, cr);
    }
}



static void
systray_button_toggled (GtkWidget     *button,
                               Systray *plugin)
{
/*
  g_return_if_fail (IS_SYSTRAY (plugin));
  g_return_if_fail (GTK_IS_TOGGLE_BUTTON (button));
  g_return_if_fail (plugin->button == button);

  systray_box_set_show_hidden (SYSTRAY_BOX (plugin->box),
      gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)));
  systray_button_set_arrow (plugin);
  */
}



static void
systray_button_set_arrow (Systray *plugin)
{
  /*
  GtkArrowType   arrow_type;
  gboolean       show_hidden;
  GtkOrientation orientation;

  g_return_if_fail (IS_SYSTRAY (plugin));

  show_hidden = systray_box_get_show_hidden (SYSTRAY_BOX (plugin->box));
  orientation = xfce_panel_plugin_get_orientation (PANEL_PLUGIN (plugin));
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    arrow_type = show_hidden ? GTK_ARROW_LEFT : GTK_ARROW_RIGHT;
  else
    arrow_type = show_hidden ? GTK_ARROW_UP : GTK_ARROW_DOWN;

  xfce_arrow_button_set_arrow_type (ARROW_BUTTON (plugin->button), arrow_type);
  */
}



static void
systray_names_collect (GPtrArray   *array,
                              const gchar *name)
{
  GValue *tmp;

  tmp = g_new0 (GValue, 1);
  g_value_init (tmp, G_TYPE_STRING);
  g_value_set_string (tmp, name);
  g_ptr_array_add (array, tmp);
}



static void
systray_names_collect_visible (gpointer key,
                                      gpointer value,
                                      gpointer user_data)
{
  /* add all the visible names */
  if (!GPOINTER_TO_UINT (value))
    systray_names_collect (user_data, key);
}



static void
systray_names_collect_hidden (gpointer key,
                                     gpointer value,
                                     gpointer user_data)
{
  /* add all the hidden names */
  if (GPOINTER_TO_UINT (value))
    systray_names_collect (user_data, key);
}



static gboolean
systray_names_remove (gpointer key,
                             gpointer value,
                             gpointer user_data)
{
  return GPOINTER_TO_UINT (value) == GPOINTER_TO_UINT (user_data);
}



static void
systray_names_update_icon (GtkWidget *icon,
                                  gpointer   data)
{
  Systray *plugin = SYSTRAY (data);
  SystraySocket *socket = SYSTRAY_SOCKET (icon);
  const gchar   *name;

  g_return_if_fail (IS_SYSTRAY (plugin));
  g_return_if_fail (IS_SYSTRAY_SOCKET (icon));

  name = systray_socket_get_name (socket);
  systray_socket_set_hidden (socket,
      systray_names_get_hidden (plugin, name));
}



static void
systray_names_update (Systray *plugin)
{
  g_return_if_fail (IS_SYSTRAY (plugin));

  gtk_container_foreach (GTK_CONTAINER (plugin->box),
     systray_names_update_icon, plugin);
  systray_box_update (SYSTRAY_BOX (plugin->box));
}



static void
systray_names_set_hidden (Systray *plugin,
                                 const gchar   *name,
                                 gboolean       hidden)
{
  g_return_if_fail (IS_SYSTRAY (plugin));
  g_return_if_fail (name && name[0]);

  g_hash_table_replace (plugin->names, g_strdup (name),
                        GUINT_TO_POINTER (hidden ? 1 : 0));

  systray_names_update (plugin);

  g_object_notify (G_OBJECT (plugin), "names-visible");
  g_object_notify (G_OBJECT (plugin), "names-hidden");
}



static gboolean
systray_names_get_hidden (Systray *plugin,
                                 const gchar   *name)
{
  gpointer p;

  g_return_val_if_fail (name && name[0], FALSE);

  /* lookup the name in the table */
  p = g_hash_table_lookup (plugin->names, name);
  if (G_UNLIKELY (p == NULL))
    {
      /* add the new name */
      g_hash_table_insert (plugin->names, g_strdup (name), GUINT_TO_POINTER (0));
      g_object_notify (G_OBJECT (plugin), "names-visible");

      /* do not hide the icon */
      return FALSE;
    }
  else
    {
      return (GPOINTER_TO_UINT (p) == 1 ? TRUE : FALSE);
    }
}



static void
systray_names_clear (Systray *plugin)
{
  g_hash_table_remove_all (plugin->names);

  g_object_notify (G_OBJECT (plugin), "names-hidden");
  g_object_notify (G_OBJECT (plugin), "names-visible");

  systray_names_update (plugin);
}



static void
systray_icon_added (SystrayManager *manager,
                           GtkWidget      *icon,
                           Systray  *plugin)
{
  g_return_if_fail (IS_SYSTRAY_MANAGER (manager));
  g_return_if_fail (IS_SYSTRAY (plugin));
  g_return_if_fail (IS_SYSTRAY_SOCKET (icon));
  g_return_if_fail (plugin->manager == manager);
  g_return_if_fail (GTK_IS_WIDGET (icon));

  systray_names_update_icon (icon, plugin);
  gtk_container_add (GTK_CONTAINER (plugin->box), icon);
  gtk_widget_show (icon);

  g_debug ("added %s[%p] icon",
      systray_socket_get_name (SYSTRAY_SOCKET (icon)), icon);
}



static void
systray_icon_removed (SystrayManager *manager,
                             GtkWidget      *icon,
                             Systray  *plugin)
{
  g_return_if_fail (IS_SYSTRAY_MANAGER (manager));
  g_return_if_fail (IS_SYSTRAY (plugin));
  g_return_if_fail (plugin->manager == manager);
  g_return_if_fail (GTK_IS_WIDGET (icon));

  /* remove the icon from the box */
  gtk_container_remove (GTK_CONTAINER (plugin->box), icon);

  g_debug ("removed %s[%p] icon",
      systray_socket_get_name (SYSTRAY_SOCKET (icon)), icon);
}



static void
systray_lost_selection (SystrayManager *manager,
                               Systray  *plugin)
{
  GError error;

  g_return_if_fail (IS_SYSTRAY_MANAGER (manager));
  g_return_if_fail (IS_SYSTRAY (plugin));
  g_return_if_fail (plugin->manager == manager);

  /* create fake error and show it */
  g_error("Most likely another widget took over the function "
                    "of a notification area. This area will be unused.");
}



static gchar *
systray_dialog_camel_case (const gchar *text)
{
  const gchar *t;
  gboolean     upper = TRUE;
  gunichar     c;
  GString     *result;

  g_return_val_if_fail (text && text[0], NULL);

  /* allocate a new string for the result */
  result = g_string_sized_new (32);

  /* convert the input text */
  for (t = text; *t != '\0'; t = g_utf8_next_char (t))
    {
      /* check the next char */
      c = g_utf8_get_char (t);
      if (g_unichar_isspace (c))
        {
          upper = TRUE;
        }
      else if (upper)
        {
          c = g_unichar_toupper (c);
          upper = FALSE;
        }
      else
        {
          c = g_unichar_tolower (c);
        }

      /* append the char to the result */
      g_string_append_unichar (result, c);
    }

  return g_string_free (result, FALSE);
}



static void
systray_dialog_add_application_names (gpointer key,
                                             gpointer value,
                                             gpointer user_data)
{
  GtkListStore *store = GTK_LIST_STORE (user_data);
  const gchar  *name = key;
  gboolean      hidden = GPOINTER_TO_UINT (value);
  const gchar  *title = NULL;
  gchar        *camelcase = NULL;
  const gchar  *icon_name = name;
  GdkPixbuf    *pixbuf;
  guint         i;
  GtkTreeIter   iter;

  g_return_if_fail (GTK_IS_LIST_STORE (store));
  g_return_if_fail (name == NULL || g_utf8_validate (name, -1, NULL));

  /* skip invalid names */
  g_return_if_fail(name && name[0]);

  /* check if we have a better name for the application */
  for (i = 0; i < G_N_ELEMENTS (known_applications); i++)
    {
      if (strcmp (name, known_applications[i][0]) == 0)
        {
          icon_name = known_applications[i][1];
          title = known_applications[i][2];
          break;
        }
    }

  /* create fallback title if the application was not found */
  if (title == NULL)
    {
      camelcase = systray_dialog_camel_case (name);
      title = camelcase;
    }

  /* try to load the icon name
  if (G_LIKELY (icon_name != NULL))
    pixbuf = xfce_panel_pixbuf_from_source (icon_name, NULL, ICON_SIZE);
  else*/
    pixbuf = NULL;

  /* insert in the store */
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
                      COLUMN_PIXBUF, pixbuf,
                      COLUMN_TITLE, title,
                      COLUMN_HIDDEN, hidden,
                      COLUMN_INTERNAL_NAME, name,
                      -1);

  g_free (camelcase);
  if (pixbuf != NULL)
    g_object_unref (G_OBJECT (pixbuf));
}



static void
systray_dialog_hidden_toggled (GtkCellRendererToggle *renderer,
                                      const gchar           *path_string,
                                      Systray         *plugin)
{
  GtkTreeIter   iter;
  gboolean      hidden;
  GtkTreeModel *model;
  gchar        *name;

  g_return_if_fail (IS_SYSTRAY (plugin));
  g_return_if_fail (IS_SYSTRAY_BOX (plugin->box));

  model = g_object_get_data (G_OBJECT (plugin), "applications-store");
  g_return_if_fail (GTK_IS_LIST_STORE (model));
  if (gtk_tree_model_get_iter_from_string (model, &iter, path_string))
    {
      gtk_tree_model_get (model, &iter,
                          COLUMN_HIDDEN, &hidden,
                          COLUMN_INTERNAL_NAME, &name, -1);

      /* insert value (we need to update it) */
      hidden = !hidden;

      /* update box and store with new state */
      systray_names_set_hidden (plugin, name, hidden);
      gtk_list_store_set (GTK_LIST_STORE (model), &iter, 2, hidden, -1);

      g_free (name);
    }
}



static void
systray_dialog_clear_clicked (GtkWidget     *button,
                                     Systray *plugin)
{
    /*
  GtkListStore *store;

  g_return_if_fail (IS_SYSTRAY (plugin));
  g_return_if_fail (IS_SYSTRAY_BOX (plugin->box));

  if (xfce_dialog_confirm (GTK_WINDOW (gtk_widget_get_toplevel (button)),
                           GTK_STOCK_CLEAR, NULL, NULL,
                           _("Are you sure you want to clear the list of "
                             "known applications?")))
    {
      store = g_object_get_data (G_OBJECT (plugin), "applications-store");
      g_return_if_fail (GTK_IS_LIST_STORE (store));
      gtk_list_store_clear (store);

      systray_names_clear (plugin);
    }
    */
}
