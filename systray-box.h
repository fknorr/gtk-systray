/*
 * Copyright (C) 2007-2010 Nick Schermer <nick@xfce.org>
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

#ifndef __SYSTRAY_BOX_H__
#define __SYSTRAY_BOX_H__

typedef struct _SystrayBoxClass SystrayBoxClass;
typedef struct _SystrayBox      SystrayBox;

/* keep those in sync with the glade file too! */
#define SIZE_MAX_MIN     (12)
#define SIZE_MAX_MAX     (64)
#define SIZE_MAX_DEFAULT (22)

#define TYPE_SYSTRAY_BOX            (systray_box_get_type ())
#define SYSTRAY_BOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_SYSTRAY_BOX, SystrayBox))
#define SYSTRAY_BOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_SYSTRAY_BOX, SystrayBoxClass))
#define IS_SYSTRAY_BOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_SYSTRAY_BOX))
#define IS_SYSTRAY_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_SYSTRAY_BOX))
#define SYSTRAY_BOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_SYSTRAY_BOX, SystrayBoxClass))

GType      systray_box_get_type        (void) G_GNUC_CONST;

GtkWidget *systray_box_new             (void) G_GNUC_MALLOC;

void       systray_box_set_orientation (SystrayBox          *box,
                                        GtkOrientation       orientation);

void       systray_box_set_size_max    (SystrayBox          *box,
                                        gint                 size_max);

gint       systray_box_get_size_max    (SystrayBox          *box);

void       systray_box_set_size_alloc  (SystrayBox          *box,
                                        gint                 size_alloc);

void       systray_box_set_show_hidden (SystrayBox          *box,
                                        gboolean             show_hidden);

gboolean   systray_box_get_show_hidden (SystrayBox          *box);

void       systray_box_update          (SystrayBox          *box);

#endif /* !__SYSTRAY_BOX_H__ */
