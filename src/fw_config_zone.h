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

#ifndef __FW_CONFIG_ZONE_H__
#define __FW_CONFIG_ZONE_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"
#include "fw_zone.h"

#define FW_CONFIG_ZONE_TYPE            (fw_config_zone_get_type())
#define FW_CONFIG_ZONE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_CONFIG_ZONE_TYPE, FWConfigFWConfigZone))
#define FW_CONFIG_ZONE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_CONFIG_ZONE_TYPE, FWConfigZoneClass))
#define FW_IS_CONFIG_ZONE(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_CONFIG_ZONE_TYPE, FWConfigZoneClass))
#define FW_CONFIG_ZONE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_CONFIG_ZONE_TYPE, FWConfigZoneClass))

typedef struct {
    GObject parent;
} FWConfigZone;

typedef struct {
    GObjectClass parent;
} FWConfigZoneClass;

GType fw_config_zone_get_type(void);
FWConfigZone *fw_config_zone_new(gchar *path);

void fw_config_zone_print_str(FWConfigZone *obj);

FWZone *fw_config_zone_getSettings(FWConfigZone *obj);
void fw_config_zone_update(FWConfigZone *obj, FWZone *settings);
void fw_config_zone_loadDefaults(FWConfigZone *obj);
void fw_config_zone_remove(FWConfigZone *obj);
void fw_config_zone_rename(FWConfigZone *obj, gchar *name);

gchar *fw_config_zone_getVersion(FWConfigZone *obj);
void fw_config_zone_setVersion(FWConfigZone *obj, gchar *version);

gchar *fw_config_zone_getShort(FWConfigZone *obj);
void fw_config_zone_setShort(FWConfigZone *obj, gchar *short_description);

gchar *fw_config_zone_getDescription(FWConfigZone *obj);
void fw_config_zone_setDescription(FWConfigZone *obj, gchar *description);

gchar *fw_config_zone_getTarget(FWConfigZone *obj);
void fw_config_zone_setTarget(FWConfigZone *obj, gchar *target);

GList *fw_config_zone_getServices(FWConfigZone *obj);
void fw_config_zone_setServices(FWConfigZone *obj, GList *services);
void fw_config_zone_addService(FWConfigZone *obj, gchar *service);
void fw_config_zone_removeService(FWConfigZone *obj, gchar *service);
gboolean fw_config_zone_queryService(FWConfigZone *obj, gchar *service);

FWPortList *fw_config_zone_getPorts(FWConfigZone *obj);
void fw_config_zone_setPorts(FWConfigZone *obj, FWPortList *ports);

void fw_config_zone_addPort(FWConfigZone *obj, gchar *port, gchar *protocol);
void fw_config_zone_removePort(FWConfigZone *obj, gchar *port, gchar *protocol);
gboolean fw_config_zone_queryPort(FWConfigZone *obj, gchar *port, gchar *protocol);

void fw_config_zone_add_port(FWConfigZone *obj, FWPort *port);
void fw_config_zone_remove_port(FWConfigZone *obj, FWPort *port);
gboolean fw_config_zone_query_port(FWConfigZone *obj, FWPort *port);

GList *fw_config_zone_getProtocols(FWConfigZone *obj);
void fw_config_zone_setProtocols(FWConfigZone *obj, GList *protocols);
void fw_config_zone_addProtocol(FWConfigZone *obj, gchar *protocol);
void fw_config_zone_removeProtocol(FWConfigZone *obj, gchar *protocol);
gboolean fw_config_zone_queryProtocol(FWConfigZone *obj, gchar *protocol);

FWPortList *fw_config_zone_getSourcePorts(FWConfigZone *obj);
void fw_config_zone_setSourcePorts(FWConfigZone *obj, FWPortList *ports);

void fw_config_zone_addSourcePort(FWConfigZone *obj, gchar *port, gchar *protocol);
void fw_config_zone_removeSourcePort(FWConfigZone *obj, gchar *port, gchar *protocol);
gboolean fw_config_zone_querySourcePort(FWConfigZone *obj, gchar *port, gchar *protocol);

void fw_config_zone_add_source_port(FWConfigZone *obj, FWPort *port);
void fw_config_zone_remove_source_port(FWConfigZone *obj, FWPort *port);
gboolean fw_config_zone_query_source_port(FWConfigZone *obj, FWPort *port);

GList *fw_config_zone_getIcmpBlocks(FWConfigZone *obj);
void fw_config_zone_setIcmpBlocks(FWConfigZone *obj, GList *icmp_types);
void fw_config_zone_addIcmpBlock(FWConfigZone *obj, gchar *icmp_type);
void fw_config_zone_removeIcmpBlock(FWConfigZone *obj, gchar *icmp_type);
gboolean fw_config_zone_queryIcmpBlock(FWConfigZone *obj, gchar *icmp_type);

gboolean fw_config_zone_getMasquerade(FWConfigZone *obj);
void fw_config_zone_setMasquerade(FWConfigZone *obj, gboolean masquerade);
void fw_config_zone_addMasquerade(FWConfigZone *obj);
void fw_config_zone_removeMasquerade(FWConfigZone *obj);
gboolean fw_config_zone_queryMasquerade(FWConfigZone *obj);

FWForwardPortList *fw_config_zone_getForwardPorts(FWConfigZone *obj);
void fw_config_zone_setForwardPorts(FWConfigZone *obj, FWForwardPortList *forward_ports);

void fw_config_zone_addForwardPort(FWConfigZone *obj, gchar *port, gchar *protocol, gchar *toport, gchar *toaddr);
void fw_config_zone_removeForwardPort(FWConfigZone *obj, gchar *port, gchar *protocol, gchar *toport, gchar *toaddr);
gboolean fw_config_zone_queryForwardPort(FWConfigZone *obj, gchar *port, gchar *protocol, gchar *toport, gchar *toaddr);

void fw_config_zone_add_forward_port(FWConfigZone *obj, FWForwardPort *forward_port);
void fw_config_zone_remove_forward_port(FWConfigZone *obj, FWForwardPort *forward_port);
gboolean fw_config_zone_query_forward_port(FWConfigZone *obj, FWForwardPort *forward_port);

GList *fw_config_zone_getInterfaces(FWConfigZone *obj);
void fw_config_zone_setInterfaces(FWConfigZone *obj, GList *interfaces);
void fw_config_zone_addInterface(FWConfigZone *obj, gchar *interface);
void fw_config_zone_removeInterface(FWConfigZone *obj, gchar *interface);
gboolean fw_config_zone_queryInterface(FWConfigZone *obj, gchar *interface);

GList *fw_config_zone_getSources(FWConfigZone *obj);
void fw_config_zone_setSources(FWConfigZone *obj, GList *sources);
void fw_config_zone_addSource(FWConfigZone *obj, gchar *source);
void fw_config_zone_removeSource(FWConfigZone *obj, gchar *source);
gboolean fw_config_zone_querySource(FWConfigZone *obj, gchar *source);

GList *fw_config_zone_getRichRules(FWConfigZone *obj);
void fw_config_zone_setRichRules(FWConfigZone *obj, GList *rich_rules);
void fw_config_zone_addRichRule(FWConfigZone *obj, gchar *rich_rule);
void fw_config_zone_removeRichRule(FWConfigZone *obj, gchar *rich_rule);
gboolean fw_config_zone_queryRichRule(FWConfigZone *obj, gchar *rich_rule);

gboolean fw_config_zone_getIcmpBlockInversion(FWConfigZone *obj);
void fw_config_zone_setIcmpBlockInversion(FWConfigZone *obj, gboolean icmp_block_inversion);
void fw_config_zone_addIcmpBlockInversion(FWConfigZone *obj);
void fw_config_zone_removeIcmpBlockInversion(FWConfigZone *obj);
gboolean fw_config_zone_queryIcmpBlockInversion(FWConfigZone *obj);

#endif /* __FW_CONFIG_ZONE_H__ */
