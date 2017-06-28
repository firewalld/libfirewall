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

#ifndef __FW_CONFIG_SERVICE_H__
#define __FW_CONFIG_SERVICE_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"
#include "fw_service.h"

#define FW_CONFIG_SERVICE_TYPE            (fw_config_service_get_type())
#define FW_CONFIG_SERVICE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_CONFIG_SERVICE_TYPE, FWConfigFWConfigService))
#define FW_CONFIG_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_CONFIG_SERVICE_TYPE, FWConfigServiceClass))
#define FW_IS_CONFIG_SERVICE(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_CONFIG_SERVICE_TYPE, FWConfigServiceClass))
#define FW_CONFIG_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_CONFIG_SERVICE_TYPE, FWConfigServiceClass))

typedef struct {
    GObject parent;
} FWConfigService;

typedef struct {
    GObjectClass parent;
} FWConfigServiceClass;

GType fw_config_service_get_type(void);
FWConfigService *fw_config_service_new(gchar *path);

void fw_config_service_print_str(FWConfigService *obj);

FWService *fw_config_service_getSettings(FWConfigService *obj);
void fw_config_service_update(FWConfigService *obj, FWService *settings);
void fw_config_service_loadDefaults(FWConfigService *obj);
void fw_config_service_remove(FWConfigService *obj);
void fw_config_service_rename(FWConfigService *obj, gchar *name);


gchar *fw_config_service_getVersion(FWConfigService *obj);
void fw_config_service_setVersion(FWConfigService *obj, gchar *version);

gchar *fw_config_service_getShort(FWConfigService *obj);
void fw_config_service_setShort(FWConfigService *obj, gchar *short_description);

gchar *fw_config_service_getDescription(FWConfigService *obj);
void fw_config_service_setDescription(FWConfigService *obj, gchar *description);

FWPortList *fw_config_service_getPorts(FWConfigService *obj);
void fw_config_service_setPorts(FWConfigService *obj, FWPortList *ports);

void fw_config_service_addPort(FWConfigService *obj, gchar *port, gchar *protocol);
void fw_config_service_removePort(FWConfigService *obj, gchar *port, gchar *protocol);
gboolean fw_config_service_queryPort(FWConfigService *obj, gchar *port, gchar *protocol);

void fw_config_service_add_port(FWConfigService *obj, FWPort *port);
void fw_config_service_remove_port(FWConfigService *obj, FWPort *port);
gboolean fw_config_service_query_port(FWConfigService *obj, FWPort *port);

GList *fw_config_service_getProtocols(FWConfigService *obj);
void fw_config_service_setProtocols(FWConfigService *obj, GList *protocols);
void fw_config_service_addProtocol(FWConfigService *obj, gchar *protocol);
void fw_config_service_removeProtocol(FWConfigService *obj, gchar *protocol);
gboolean fw_config_service_queryProtocol(FWConfigService *obj, gchar *protocol);

FWPortList *fw_config_service_getSourcePorts(FWConfigService *obj);
void fw_config_service_setSourcePorts(FWConfigService *obj, FWPortList *ports);

void fw_config_service_addSourcePort(FWConfigService *obj, gchar *port, gchar *protocol);
void fw_config_service_removeSourcePort(FWConfigService *obj, gchar *port, gchar *protocol);
gboolean fw_config_service_querySourcePort(FWConfigService *obj, gchar *port, gchar *protocol);

void fw_config_service_add_source_port(FWConfigService *obj, FWPort *port);
void fw_config_service_remove_source_port(FWConfigService *obj, FWPort *port);
gboolean fw_config_service_query_source_port(FWConfigService *obj, FWPort *port);

GList *fw_config_service_getModules(FWConfigService *obj);
void fw_config_service_setModules(FWConfigService *obj, GList *modules);
void fw_config_service_addModule(FWConfigService *obj, gchar *module);
void fw_config_service_removeModule(FWConfigService *obj, gchar *module);
gboolean fw_config_service_queryModule(FWConfigService *obj, gchar *module);

GHashTable *fw_config_service_getDestinations(FWConfigService *obj);
void fw_config_service_setDestinations(FWConfigService *obj, GHashTable *destinations);
void fw_config_service_setDestination(FWConfigService *obj, gchar *ipv, gchar *address);
void fw_config_service_removeDestination(FWConfigService *obj, gchar *ipv);
gboolean fw_config_service_queryDestination(FWConfigService *obj, gchar *ipv, gchar *address);

#endif /* __FW_CONFIG_SERVICE_H__ */
