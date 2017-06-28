/*
 * Copyright (C) 2014 Red Hat, Inc.
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

#ifndef __FW_FORWARD_PORT_H__
#define __FW_FORWARD_PORT_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"

#define FW_FORWARD_PORT_TYPE            (fw_forward_port_get_type())
#define FW_FORWARD_PORT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_FORWARD_PORT_TYPE, FWForwardPort))
#define FW_FORWARD_PORT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_FORWARD_PORT_TYPE, FWForwardPortClass))
#define FW_IS_FORWARD_PORT(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_FORWARD_PORT_TYPE, FWForwardPortClass))
#define FW_FORWARD_PORT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_FORWARD_PORT_TYPE, FWForwardPortClass))

typedef struct {
    GObject parent;
} FWForwardPort;

typedef struct {
    GObjectClass parent;
} FWForwardPortClass;

GType fw_forward_port_get_type(void);
FWForwardPort *fw_forward_port_new_simple(void);
FWForwardPort *fw_forward_port_new_from_variant(GVariant *variant);
FWForwardPort *fw_forward_port_new(const gchar *port, const gchar *protocol, const gchar *toport, const gchar *toaddr);
GVariant *fw_forward_port_to_variant(FWForwardPort *obj);

gchar *fw_forward_port_getStr(FWForwardPort *obj);

gchar *fw_forward_port_getPort(FWForwardPort *obj);
void fw_forward_port_setPort(FWForwardPort *obj, const gchar *port);

gchar *fw_forward_port_getProtocol(FWForwardPort *obj);
void fw_forward_port_setProtocol(FWForwardPort *obj, const gchar *protocol);

gchar *fw_forward_port_getToPort(FWForwardPort *obj);
void fw_forward_port_setToPort(FWForwardPort *obj, const gchar *toport);

gchar *fw_forward_port_getToAddr(FWForwardPort *obj);
void fw_forward_port_setToAddr(FWForwardPort *obj, const gchar *toaddr);

#endif /* __FW_FORWARD_PORT_H__ */
