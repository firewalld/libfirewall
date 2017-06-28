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

#ifndef __FW_ZONE_H__
#define __FW_ZONE_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"
#include "fw_port_list.h"
#include "fw_forward_port.h"
#include "fw_forward_port_list.h"

#define FW_ZONE_TYPE            (fw_zone_get_type())
#define FW_ZONE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_ZONE_TYPE, FWZone))
#define FW_ZONE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_ZONE_TYPE, FWZoneClass))
#define FW_IS_ZONE(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_ZONE_TYPE, FWZoneClass))
#define FW_ZONE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_ZONE_TYPE, FWZoneClass))

typedef struct {
    GObject parent;
} FWZone;

typedef struct {
    GObjectClass parent;
} FWZoneClass;

GType fw_zone_get_type(void);
FWZone *fw_zone_new(void);
FWZone *fw_zone_new_from_variant(GVariant *variant);
GVariant *fw_zone_to_variant(FWZone *obj);

void fw_zone_print_str(FWZone *obj);

gchar *fw_zone_getVersion(FWZone *obj);
void fw_zone_setVersion(FWZone *obj, gchar *version);

gchar *fw_zone_getShort(FWZone *obj);
void fw_zone_setShort(FWZone *obj, gchar *short_description);

gchar *fw_zone_getDescription(FWZone *obj);
void fw_zone_setDescription(FWZone *obj, gchar *description);

gchar *fw_zone_getTarget(FWZone *obj);
void fw_zone_setTarget(FWZone *obj, gchar *target);

GList *fw_zone_getServices(FWZone *obj);
void fw_zone_setServices(FWZone *obj, GList *services);
void fw_zone_addService(FWZone *obj, gchar *service);
void fw_zone_removeService(FWZone *obj, gchar *service);
gboolean fw_zone_queryService(FWZone *obj, gchar *service);

FWPortList *fw_zone_getPorts(FWZone *obj);
void fw_zone_setPorts(FWZone *obj, FWPortList *ports);

void fw_zone_addPort(FWZone *obj, gchar *port, gchar *protocol);
void fw_zone_removePort(FWZone *obj, gchar *port, gchar *protocol);
gboolean fw_zone_queryPort(FWZone *obj, gchar *port, gchar *protocol);

void fw_zone_add_port(FWZone *obj, FWPort *port);
void fw_zone_remove_port(FWZone *obj, FWPort *port);
gboolean fw_zone_query_port(FWZone *obj, FWPort *port);

GList *fw_zone_getProtocols(FWZone *obj);
void fw_zone_setProtocols(FWZone *obj, GList *protocols);
void fw_zone_addProtocol(FWZone *obj, gchar *protocol);
void fw_zone_removeProtocol(FWZone *obj, gchar *protocol);
gboolean fw_zone_queryProtocol(FWZone *obj, gchar *protocol);

FWPortList *fw_zone_getSourcePorts(FWZone *obj);
void fw_zone_setSourcePorts(FWZone *obj, FWPortList *ports);

void fw_zone_addSourcePort(FWZone *obj, gchar *port, gchar *protocol);
void fw_zone_removeSourcePort(FWZone *obj, gchar *port, gchar *protocol);
gboolean fw_zone_querySourcePort(FWZone *obj, gchar *port, gchar *protocol);

void fw_zone_add_source_port(FWZone *obj, FWPort *port);
void fw_zone_remove_source_port(FWZone *obj, FWPort *port);
gboolean fw_zone_query_source_port(FWZone *obj, FWPort *port);

GList *fw_zone_getIcmpBlocks(FWZone *obj);
void fw_zone_setIcmpBlocks(FWZone *obj, GList *icmp_types);
void fw_zone_addIcmpBlock(FWZone *obj, gchar *icmp_type);
void fw_zone_removeIcmpBlock(FWZone *obj, gchar *icmp_type);
gboolean fw_zone_queryIcmpBlock(FWZone *obj, gchar *icmp_type);

gboolean fw_zone_getMasquerade(FWZone *obj);
void fw_zone_setMasquerade(FWZone *obj, gboolean masquerade);
void fw_zone_addMasquerade(FWZone *obj);
void fw_zone_removeMasquerade(FWZone *obj);
gboolean fw_zone_queryMasquerade(FWZone *obj);

FWForwardPortList *fw_zone_getForwardPorts(FWZone *obj);
void fw_zone_setForwardPorts(FWZone *obj, FWForwardPortList *forward_ports);

void fw_zone_addForwardPort(FWZone *obj, gchar *port, gchar *protocol, gchar *toport, gchar *toaddr);
void fw_zone_removeForwardPort(FWZone *obj, gchar *port, gchar *protocol, gchar *toport, gchar *toaddr);
gboolean fw_zone_queryForwardPort(FWZone *obj, gchar *port, gchar *protocol, gchar *toport, gchar *toaddr);

void fw_zone_add_forward_port(FWZone *obj, FWForwardPort *forward_port);
void fw_zone_remove_forward_port(FWZone *obj, FWForwardPort *forward_port);
gboolean fw_zone_query_forward_port(FWZone *obj, FWForwardPort *forward_port);

GList *fw_zone_getInterfaces(FWZone *obj);
void fw_zone_setInterfaces(FWZone *obj, GList *interfaces);
void fw_zone_addInterface(FWZone *obj, gchar *interface);
void fw_zone_removeInterface(FWZone *obj, gchar *interface);
gboolean fw_zone_queryInterface(FWZone *obj, gchar *interface);

GList *fw_zone_getSources(FWZone *obj);
void fw_zone_setSources(FWZone *obj, GList *sources);
void fw_zone_addSource(FWZone *obj, gchar *source);
void fw_zone_removeSource(FWZone *obj, gchar *source);
gboolean fw_zone_querySource(FWZone *obj, gchar *source);

GList *fw_zone_getRichRules(FWZone *obj);
void fw_zone_setRichRules(FWZone *obj, GList *rich_rules);
void fw_zone_addRichRule(FWZone *obj, gchar *rich_rule);
void fw_zone_removeRichRule(FWZone *obj, gchar *rich_rule);
gboolean fw_zone_queryRichRule(FWZone *obj, gchar *rich_rule);

gboolean fw_zone_getIcmpBlockInversion(FWZone *obj);
void fw_zone_setIcmpBlockInversion(FWZone *obj, gboolean icmp_block_inversion);
void fw_zone_addIcmpBlockInversion(FWZone *obj);
void fw_zone_removeIcmpBlockInversion(FWZone *obj);
gboolean fw_zone_queryIcmpBlockInversion(FWZone *obj);

#endif /* __FW_ZONE_H__ */
