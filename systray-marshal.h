
#ifndef __systray_marshal_MARSHAL_H__
#define __systray_marshal_MARSHAL_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* VOID:OBJECT,STRING,LONG,LONG (systray-marshal.list:1) */
extern void systray_marshal_VOID__OBJECT_STRING_LONG_LONG(
    GClosure *closure, GValue *return_value, guint n_param_values,
    const GValue *param_values, gpointer invocation_hint,
    gpointer marshal_data);

/* VOID:OBJECT,LONG (systray-marshal.list:2) */
extern void systray_marshal_VOID__OBJECT_LONG(GClosure *closure,
                                              GValue *return_value,
                                              guint n_param_values,
                                              const GValue *param_values,
                                              gpointer invocation_hint,
                                              gpointer marshal_data);

G_END_DECLS

#endif /* __systray_marshal_MARSHAL_H__ */
