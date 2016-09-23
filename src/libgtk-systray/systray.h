/*
 * Copyright (c) 2008-2010 Nick Schermer <nick@xfce.org>
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

#ifndef __SYSTRAY_H__
#define __SYSTRAY_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _SystrayClass SystrayClass;
typedef struct _Systray Systray;
typedef struct _SystrayChild SystrayChild;
typedef enum _SystrayChildState SystrayChildState;

#define TYPE_SYSTRAY (systray_get_type())
#define SYSTRAY(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_SYSTRAY, Systray))
#define SYSTRAY_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_SYSTRAY, SystrayClass))
#define IS_SYSTRAY(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYPE_SYSTRAY))
#define IS_SYSTRAY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_SYSTRAY))
#define SYSTRAY_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_SYSTRAY, SystrayClass))

GType systray_get_type(void) G_GNUC_CONST;

GtkWidget *systray_new(void);

G_END_DECLS

#endif /* !__SYSTRAY_H__ */
