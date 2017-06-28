/*
 * Copyright (C) 2015 Red Hat, Inc.
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

#ifndef __FW_SERVICE_H__
#define __FW_SERVICE_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"
#include "fw_port_list.h"

#define FW_SERVICE_TYPE            (fw_service_get_type())
#define FW_SERVICE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_SERVICE_TYPE, FWService))
#define FW_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_SERVICE_TYPE, FWServiceClass))
#define FW_IS_SERVICE(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_SERVICE_TYPE, FWServiceClass))
#define FW_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_SERVICE_TYPE, FWServiceClass))

typedef struct {
    GObject parent;
} FWService;

typedef struct {
    GObjectClass parent;
} FWServiceClass;

GType fw_service_get_type(void);
FWService *fw_service_new(void);
FWService *fw_service_new_from_variant(GVariant *variant);
GVariant *fw_service_to_variant(FWService *obj);

void fw_service_print_str(FWService *obj);

gchar *fw_service_getVersion(FWService *obj);
void fw_service_setVersion(FWService *obj, gchar *version);

gchar *fw_service_getShort(FWService *obj);
void fw_service_setShort(FWService *obj, gchar *short_description);

gchar *fw_service_getDescription(FWService *obj);
void fw_service_setDescription(FWService *obj, gchar *description);

FWPortList *fw_service_getPorts(FWService *obj);
void fw_service_setPorts(FWService *obj, FWPortList *ports);

void fw_service_addPort(FWService *obj, gchar *port, gchar *protocol);
void fw_service_removePort(FWService *obj, gchar *port, gchar *protocol);
gboolean fw_service_queryPort(FWService *obj, gchar *port, gchar *protocol);

void fw_service_add_port(FWService *obj, FWPort *port);
void fw_service_remove_port(FWService *obj, FWPort *port);
gboolean fw_service_query_port(FWService *obj, FWPort *port);

GList *fw_service_getProtocols(FWService *obj);
void fw_service_setProtocols(FWService *obj, GList *protocols);
void fw_service_addProtocol(FWService *obj, gchar *protocol);
void fw_service_removeProtocol(FWService *obj, gchar *protocol);
gboolean fw_service_queryProtocol(FWService *obj, gchar *protocol);

FWPortList *fw_service_getSourcePorts(FWService *obj);
void fw_service_setSourcePorts(FWService *obj, FWPortList *ports);

void fw_service_addSourcePort(FWService *obj, gchar *port, gchar *protocol);
void fw_service_removeSourcePort(FWService *obj, gchar *port, gchar *protocol);
gboolean fw_service_querySourcePort(FWService *obj, gchar *port, gchar *protocol);

void fw_service_add_source_port(FWService *obj, FWPort *port);
void fw_service_remove_source_port(FWService *obj, FWPort *port);
gboolean fw_service_query_source_port(FWService *obj, FWPort *port);

GList *fw_service_getModules(FWService *obj);
void fw_service_setModules(FWService *obj, GList *modules);
void fw_service_addModule(FWService *obj, gchar *module);
void fw_service_removeModule(FWService *obj, gchar *module);
gboolean fw_service_queryModule(FWService *obj, gchar *module);

GHashTable *fw_service_getDestinations(FWService *obj);
void fw_service_setDestinations(FWService *obj, GHashTable *destinations);
void fw_service_setDestination(FWService *obj, gchar *ipv, gchar *address);
void fw_service_removeDestination(FWService *obj, gchar *ipv);
gboolean fw_service_queryDestination(FWService *obj, gchar *ipv, gchar *address);

#endif /* __FW_SERVICE_H__ */
