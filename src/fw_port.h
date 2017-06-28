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

#ifndef __FW_PORT_H__
#define __FW_PORT_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"

#define FW_PORT_TYPE            (fw_port_get_type())
#define FW_PORT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_PORT_TYPE, FWPort))
#define FW_PORT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_PORT_TYPE, FWPortClass))
#define FW_IS_PORT(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_PORT_TYPE, FWPortClass))
#define FW_PORT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_PORT_TYPE, FWPortClass))

typedef struct {
    GObject parent;
} FWPort;

typedef struct {
    GObjectClass parent;
} FWPortClass;

GType fw_port_get_type(void);
FWPort *fw_port_new_simple(void);
FWPort *fw_port_new_from_variant(GVariant *variant);
FWPort *fw_port_new(gchar *port, gchar *protocol);
GVariant *fw_port_to_variant(FWPort *obj);

gchar *fw_port_getPort(FWPort *obj);
gchar *fw_port_getProtocol(FWPort *obj);
gchar *fw_port_getStr(FWPort *obj);

void fw_port_setPort(FWPort *obj, gchar *port);
void fw_port_setProtocol(FWPort *obj, gchar *protocol);

#endif /* __FW_PORT_H__ */
