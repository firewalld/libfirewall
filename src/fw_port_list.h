/*
 * Copyright (C) 2017 Red Hat, Inc.
 *
 * Authors:
 * Thomas Woerner <twoerner@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FW_PORT_LIST_H__
#define __FW_PORT_LIST_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"

#define FW_PORT_LIST_TYPE            (fw_port_list_get_type())
#define FW_PORT_LIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_PORT_LIST_TYPE, FWPortList))
#define FW_PORT_LIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_PORT_LIST_TYPE, FWPortListClass))
#define FW_IS_PORT_LIST(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_PORT_LIST_TYPE, FWPortListClass))
#define FW_PORT_LIST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_PORT_LIST_TYPE, FWPortListClass))

typedef struct {
    GObject parent;
} FWPortList;

typedef struct {
    GObjectClass parent;
} FWPortListClass;

GType fw_port_list_get_type(void);
FWPortList *fw_port_list_new(void);
FWPortList *fw_port_list_new_from_variant(GVariant *variant);
GVariant *fw_port_list_to_variant(FWPortList *obj);
GVariantBuilder *fw_port_list_to_builder(FWPortList *obj);

guint fw_port_list_length(FWPortList *obj);
gpointer fw_port_list_nth_data(FWPortList *obj, guint n);
void fw_port_list_free(FWPortList *obj);

gchar *fw_port_list_getStr(FWPortList *obj);
void fw_port_list_print_str(FWPortList *obj);
void fw_port_list_print_simple(FWPortList *obj);

GList *fw_port_list_getPorts(FWPortList *obj);

void fw_port_list_addPort(FWPortList *obj, gchar *port, gchar *protocol);
void fw_port_list_removePort(FWPortList *obj, gchar *port, gchar *protocol);
gboolean fw_port_list_queryPort(FWPortList *obj, gchar *port, gchar *protocol);

void fw_port_list_add(FWPortList *obj, FWPort *port);
void fw_port_list_remove(FWPortList *obj, FWPort *port);
gboolean fw_port_list_query(FWPortList *obj, FWPort *port);

#endif /* __FW_PORT_LIST_H__ */
