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

#ifndef __FW_FORWARD_PORT_LIST_H__
#define __FW_FORWARD_PORT_LIST_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_forward_port.h"

#define FW_FORWARD_PORT_LIST_TYPE            (fw_forward_port_list_get_type())
#define FW_FORWARD_PORT_LIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_FORWARD_PORT_LIST_TYPE, FWForwardPortList))
#define FW_FORWARD_PORT_LIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_FORWARD_PORT_LIST_TYPE, FWForwardPortListClass))
#define FW_IS_FORWARD_PORT_LIST(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_FORWARD_PORT_LIST_TYPE, FWForwardPortListClass))
#define FW_FORWARD_PORT_LIST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_FORWARD_PORT_LIST_TYPE, FWForwardPortListClass))

typedef struct {
    GObject parent;
} FWForwardPortList;

typedef struct {
    GObjectClass parent;
} FWForwardPortListClass;

GType fw_forward_port_list_get_type(void);
FWForwardPortList *fw_forward_port_list_new(void);
FWForwardPortList *fw_forward_port_list_new_from_variant(GVariant *variant);
GVariant *fw_forward_port_list_to_variant(FWForwardPortList *obj);
GVariantBuilder *fw_forward_port_list_to_builder(FWForwardPortList *obj);

guint fw_forward_port_list_length(FWForwardPortList *obj);
gpointer fw_forward_port_list_nth_data(FWForwardPortList *obj, guint n);
void fw_forward_port_list_free(FWForwardPortList *obj);

gchar *fw_forward_port_list_getStr(FWForwardPortList *obj);
void fw_forward_port_list_print_str(FWForwardPortList *obj);
void fw_forward_port_list_print_simple(FWForwardPortList *obj);

GList *fw_forward_port_list_getForwardPorts(FWForwardPortList *obj);
/*
void fw_forward_port_list_addForward_Port(FWForwardPortList *obj, FWForwardPort *forward_port);
void fw_forward_port_list_removeForward_Port(FWForwardPortList *obj, FWForwardPort *forward_port);
gboolean fw_forward_port_list_queryForward_Port(FWForwardPortList *obj, FWForwardPort *forward_port);
*/
void fw_forward_port_list_addForwardPort(FWForwardPortList *obj, gchar *port, gchar *protocol, gchar *toport, gchar *toaddr);
void fw_forward_port_list_removeForwardPort(FWForwardPortList *obj, gchar *port, gchar *protocol, gchar *toport, gchar *toaddr);
gboolean fw_forward_port_list_queryForwardPort(FWForwardPortList *obj, gchar *port, gchar *protocol, gchar *toport, gchar *toaddr);

void fw_forward_port_list_add(FWForwardPortList *obj, FWForwardPort *forward_port);
void fw_forward_port_list_remove(FWForwardPortList *obj, FWForwardPort *forward_port);
gboolean fw_forward_port_list_query(FWForwardPortList *obj, FWForwardPort *forward_port);

#endif /* __FW_FORWARD_PORT_LIST_H__ */
