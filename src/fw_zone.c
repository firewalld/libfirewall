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

#include <string.h>
#include "fw_zone.h"
#include "fw_functions.h"

G_DEFINE_TYPE(FWZone, fw_zone, G_TYPE_OBJECT);

#define FW_ZONE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_ZONE_TYPE, FWZonePrivate))

typedef struct {
    gchar *version;           /* string */
    gchar *short_description; /* string */
    gchar *description;       /* string */
    gchar *target;            /* string */
    GList *services;          /* list of string */
    FWPortList *ports;        /* list of FWPort */
    GList *icmp_blocks;       /* string list */
    gboolean masquerade;      /* boolean */
    FWForwardPortList *forward_ports;     /* list of FWForwardPort */
    GList *interfaces;        /* list of string */
    GList *sources;           /* list of string */
    GList *rich_rules;        /* list of string */
    GList *protocols;         /* list of string */
    FWPortList *source_ports; /* list of FWPort */
    gboolean icmp_block_inversion; /* boolean */
} FWZonePrivate;

FWZone *
fw_zone_new()
{
    return g_object_new(FW_ZONE_TYPE, NULL);
}

FWZone *
fw_zone_new_from_variant(GVariant *variant)
{
    FWZone *obj = NULL;
    GVariant *item;
    gchar *str;
    gboolean bool;

    if (variant == NULL)
	return obj;

    if (strncmp(g_variant_get_type_string(variant),
		"((sssbsasa(ss)asba(ssss)asasasasa(ss)b))", 40) != 0)
    {
       return obj;
    }

    variant = g_variant_get_child_value(variant, 0);

    obj = fw_zone_new();

    /* 0: version */
    item = g_variant_get_child_value(variant, 0);
    g_variant_get(item, "s", &str);
    fw_zone_setVersion(obj, str);
    g_variant_unref(item);

    /* 1: short */
    item = g_variant_get_child_value(variant, 1);
    g_variant_get(item, "s", &str);
    fw_zone_setShort(obj, str);
    g_variant_unref(item);

    /* 2: description */
    item = g_variant_get_child_value(variant, 2);
    g_variant_get(item, "s", &str);
    fw_zone_setDescription(obj, str);
    g_variant_unref(item);

    /* 3: unused */

    /* 4: target */
    item = g_variant_get_child_value(variant, 4);
    g_variant_get(item, "s", &str);
    fw_zone_setTarget(obj, str);
    g_variant_unref(item);

    /* 5: services */
    item = g_variant_get_child_value(variant, 5);
    fw_zone_setServices(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    /* 6: ports */
    item = g_variant_get_child_value(variant, 6);
    fw_zone_setPorts(obj, fw_port_list_new_from_variant(item));
    g_variant_unref(item);

    /* 7: icmp blocks */
    item = g_variant_get_child_value(variant, 7);
    fw_zone_setIcmpBlocks(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    /* 8: masquerade */
    item = g_variant_get_child_value(variant, 8);
    g_variant_get(item, "b", &bool);
    fw_zone_setMasquerade(obj, bool);
    g_variant_unref(item);

    /* 9: forward ports */
    item = g_variant_get_child_value(variant, 9);
    fw_zone_setForwardPorts(obj, fw_forward_port_list_new_from_variant(item));
    g_variant_unref(item);

    /* 10: interfaces */
    item = g_variant_get_child_value(variant, 10);
    fw_zone_setInterfaces(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    /* 11: sources */
    item = g_variant_get_child_value(variant, 11);
    fw_zone_setSources(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    /* 12: rich rules */
    item = g_variant_get_child_value(variant, 12);
    fw_zone_setRichRules(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    /* 13: protocols */
    item = g_variant_get_child_value(variant, 13);
    fw_zone_setProtocols(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    /* 14: source ports */
    item = g_variant_get_child_value(variant, 14);
    fw_zone_setSourcePorts(obj, fw_port_list_new_from_variant(item));
    g_variant_unref(item);

    /* 15: icmp block inversion */
    item = g_variant_get_child_value(variant, 15);
    g_variant_get(item, "b", &bool);
    fw_zone_setIcmpBlockInversion(obj, bool);
    g_variant_unref(item);

    return obj;
}

GVariant *
fw_zone_to_variant(FWZone *obj)
{
    GVariant *variant;
    GVariantBuilder *builder_services = fw_str_list_to_builder(
	fw_zone_getServices(obj));
    GVariantBuilder *builder_ports = fw_port_list_to_builder(
	fw_zone_getPorts(obj));
    GVariantBuilder *builder_icmpblocks = fw_str_list_to_builder(
	fw_zone_getIcmpBlocks(obj));
    GVariantBuilder *builder_forward_ports = fw_forward_port_list_to_builder(
	fw_zone_getForwardPorts(obj));
    GVariantBuilder *builder_interfaces = fw_str_list_to_builder(
	fw_zone_getInterfaces(obj));
    GVariantBuilder *builder_sources = fw_str_list_to_builder(
	fw_zone_getSources(obj));
    GVariantBuilder *builder_rich_rules = fw_str_list_to_builder(
	fw_zone_getRichRules(obj));
    GVariantBuilder *builder_protocols = fw_str_list_to_builder(
	fw_zone_getProtocols(obj));
    GVariantBuilder *builder_source_ports = fw_port_list_to_builder(
	fw_zone_getSourcePorts(obj));

    variant = g_variant_new("(sssbsasa(ss)asba(ssss)asasasasa(ss)b)",
			    fw_zone_getVersion(obj),
			    fw_zone_getShort(obj),
			    fw_zone_getDescription(obj),
			    FALSE,
			    fw_zone_getTarget(obj),
			    builder_services,
			    builder_ports,
			    builder_icmpblocks,
			    fw_zone_getMasquerade(obj),
			    builder_forward_ports,
			    builder_interfaces,
			    builder_sources,
			    builder_rich_rules,
			    builder_protocols,
			    builder_source_ports,
			    fw_zone_getIcmpBlockInversion(obj));

    g_variant_builder_unref(builder_services);
    g_variant_builder_unref(builder_ports);
    g_variant_builder_unref(builder_icmpblocks);
    g_variant_builder_unref(builder_forward_ports);
    g_variant_builder_unref(builder_interfaces);
    g_variant_builder_unref(builder_sources);
    g_variant_builder_unref(builder_rich_rules);
    g_variant_builder_unref(builder_protocols);
    g_variant_builder_unref(builder_source_ports);

    return variant;
}

static void
fw_zone_init(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    /* init vars */
    priv->version = g_strdup("");
    priv->short_description = g_strdup("");
    priv->description = g_strdup("");
    priv->target = g_strdup("");
    priv->services = NULL;
    priv->ports = fw_port_list_new();
    priv->icmp_blocks = NULL;
    priv->masquerade = FALSE;
    priv->forward_ports = fw_forward_port_list_new();
    priv->interfaces = NULL;
    priv->sources = NULL;
    priv->rich_rules = NULL;
    priv->protocols = NULL;
    priv->source_ports = fw_port_list_new();
    priv->icmp_block_inversion = FALSE;
}

static void
fw_zone_finalize(GObject *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->version != NULL)
	g_free(priv->version);
    if (priv->short_description != NULL)
	g_free(priv->short_description);
    if (priv->description != NULL)
	g_free(priv->description);
    if (priv->target != NULL)
	g_free(priv->target);
    if (priv->services != NULL)
	fw_str_list_free(priv->services);
    if (priv->ports != NULL)
	fw_port_list_free(priv->ports);
    if (priv->icmp_blocks != NULL)
	fw_str_list_free(priv->icmp_blocks);
    if (priv->forward_ports != NULL)
	fw_forward_port_list_free(priv->forward_ports);
    if (priv->interfaces != NULL)
	fw_str_list_free(priv->interfaces);
    if (priv->sources != NULL)
	fw_str_list_free(priv->sources);
    if (priv->rich_rules != NULL)
	fw_str_list_free(priv->rich_rules);
    if (priv->protocols != NULL)
	fw_str_list_free(priv->protocols);
    if (priv->source_ports != NULL)
	fw_port_list_free(priv->source_ports);

    G_OBJECT_CLASS(fw_zone_parent_class)->finalize(obj);
}

static void
fw_zone_class_init(FWZoneClass *fw_zone_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_zone_class);

    obj_class->finalize = fw_zone_finalize;

    g_type_class_add_private(obj_class, sizeof(FWZonePrivate));
}

/* methods */

/**
 * fw_zone_getStr:
 *
 */
void
fw_zone_print_str(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    g_print("( '%s', '%s', '%s', '%s', ",
	    priv->version, priv->short_description,
	    priv->description, priv->target);

    fw_str_list_print(priv->services);

    fw_port_list_print_simple(priv->ports);

    g_print(", ");

    fw_str_list_print(priv->icmp_blocks);

    g_print(", %s", (priv->masquerade) ? "TRUE": "FALSE");

    fw_forward_port_list_print_simple(priv->forward_ports);

    g_print(", ");

    fw_str_list_print(priv->interfaces);
    g_print(", ");

    fw_str_list_print(priv->sources);
    g_print(", ");

    fw_str_list_print(priv->rich_rules);
    g_print(" , ");

    fw_str_list_print(priv->protocols);
    g_print(" , ");

    fw_port_list_print_str(priv->source_ports);

    g_print(", ");

    g_print(", %s", (priv->icmp_block_inversion) ? "TRUE": "FALSE");

    g_print(" )\n");
}

/**
 * fw_zone_getVersion:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_zone_getVersion(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->version;
}

void
fw_zone_setVersion(FWZone *obj,
		   gchar *version)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->version != NULL)
	g_free(priv->version);
    priv->version = g_strdup(version);
}

/**
 * fw_zone_getShort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_zone_getShort(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->short_description;
}

void
fw_zone_setShort(FWZone *obj,
		 gchar *short_description)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->short_description != NULL)
	g_free(priv->short_description);
    priv->short_description = g_strdup(short_description);
}

/**
 * fw_zone_getDescription:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_zone_getDescription(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->description;
}

void
fw_zone_setDescription(FWZone *obj,
		       gchar *description)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->description != NULL)
	g_free(priv->description);
    priv->description = g_strdup(description);
}

/**
 * fw_zone_getTarget:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_zone_getTarget(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->target;
}

void
fw_zone_setTarget(FWZone *obj,
		  gchar *target)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->target != NULL)
	g_free(priv->target);
    priv->target = g_strdup(target);
}

/**
 * fw_zone_getServices:
 * @obj: (type FWZone*): a FWZone instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_zone_getServices(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->services;
}

/**
 * fw_zone_setServices:
 * @obj: (type FWZone*): a FWZone instance
 * @services: (type GList*) (element-type gchar*)
 */
void
fw_zone_setServices(FWZone *obj,
		    GList *services)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->services != NULL)
	fw_str_list_free(priv->services);
    priv->services = fw_str_list_copy(services);
}

void
fw_zone_addService(FWZone *obj,
		   gchar *service)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->services = fw_str_list_append(priv->services, service);
}

void
fw_zone_removeService(FWZone *obj,
		      gchar *service)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->services = fw_str_list_remove(priv->services, service);
}

gboolean
fw_zone_queryService(FWZone *obj,
		     gchar *service)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return fw_str_list_contains(priv->services, service);
}

/**
 * fw_zone_getPorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_zone_getPorts(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->ports;
}

/**
 * fw_zone_setPorts:
 * @obj: (type FWZone*): a FWZone instance
 * @ports: (type FWPortList*)
 */
void
fw_zone_setPorts(FWZone *obj,
		 FWPortList *ports)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->ports != NULL)
	fw_port_list_free(priv->ports);
    priv->ports = ports;
}

void
fw_zone_addPort(FWZone *obj,
		gchar *port,
		gchar *protocol)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    fw_port_list_add(priv->ports, item);
}

void
fw_zone_removePort(FWZone *obj,
		   gchar *port,
		   gchar *protocol)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    fw_port_list_remove(priv->ports, item);
}

gboolean
fw_zone_queryPort(FWZone *obj,
		  gchar *port,
		  gchar *protocol)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    return fw_port_list_query(priv->ports, item);
}

void
fw_zone_add_port(FWZone *obj,
		 FWPort *port)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    fw_port_list_add(priv->ports, port);
}

void
fw_zone_remove_port(FWZone *obj,
		    FWPort *port)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    fw_port_list_remove(priv->ports, port);
}

gboolean
fw_zone_query_port(FWZone *obj,
		   FWPort *port)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return fw_port_list_query(priv->ports, port);
}

/**
 * fw_zone_getProtocols:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type GList*)
 */
GList *
fw_zone_getProtocols(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->protocols;
}

/**
 * fw_zone_setProtocols:
 * @obj: (type FWZone*): a FWZone instance
 * @protocols: (type GList*) (element-type gchar*)
 */
void
fw_zone_setProtocols(FWZone *obj,
		     GList *protocols)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->protocols != NULL)
	fw_str_list_free(priv->protocols);
    priv->protocols = fw_str_list_copy(protocols);
}

void
fw_zone_addProtocol(FWZone *obj,
		    gchar *protocol)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->protocols = fw_str_list_append(priv->protocols, protocol);
}

void
fw_zone_removeProtocol(FWZone *obj,
		       gchar *protocol)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->protocols = fw_str_list_remove(priv->protocols, protocol);
}

gboolean
fw_zone_queryProtocol(FWZone *obj,
		      gchar *protocol)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return fw_str_list_contains(priv->protocols, protocol);
}

/**
 * fw_zone_getSourcePorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_zone_getSourcePorts(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->source_ports;
}

/**
 * fw_zone_setSourcePorts:
 * @obj: (type FWZone*): a FWZone instance
 * @ports: (type FWPortList*)
 */
void
fw_zone_setSourcePorts(FWZone *obj,
		       FWPortList *ports)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->source_ports != NULL)
	fw_port_list_free(priv->source_ports);
    priv->source_ports = ports;
}

void
fw_zone_addSourcePort(FWZone *obj,
		      gchar *port,
		      gchar *protocol)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    fw_port_list_add(priv->source_ports, item);
}

void
fw_zone_removeSourcePort(FWZone *obj,
			 gchar *port,
			 gchar *protocol)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    fw_port_list_remove(priv->source_ports, item);
}

gboolean
fw_zone_querySourcePort(FWZone *obj,
			gchar *port,
			gchar *protocol)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    return fw_port_list_query(priv->source_ports, item);
}

void
fw_zone_add_source_port(FWZone *obj,
			FWPort *port)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    fw_port_list_add(priv->source_ports, port);
}

void
fw_zone_remove_source_port(FWZone *obj,
			   FWPort *port)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    fw_port_list_remove(priv->source_ports, port);
}

gboolean
fw_zone_query_source_port(FWZone *obj,
			  FWPort *port)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return fw_port_list_query(priv->source_ports, port);
}

/**
 * fw_zone_getIcmpBlocks:
 * @obj: (type FWZone*): a FWZone instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_zone_getIcmpBlocks(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->icmp_blocks;
}

/**
 * fw_zone_setIcmpBlocks:
 * @obj: (type FWZone*): a FWZone instance
 * @icmp_types: (type GList*) (element-type gchar*)
 */
void
fw_zone_setIcmpBlocks(FWZone *obj,
		      GList *icmp_types)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->icmp_blocks != NULL)
	fw_str_list_free(priv->icmp_blocks);
    priv->icmp_blocks = fw_str_list_copy(icmp_types);
}

void
fw_zone_addIcmpBlock(FWZone *obj,
		     gchar *icmp_type)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->icmp_blocks = fw_str_list_append(priv->icmp_blocks, icmp_type);
}

void
fw_zone_removeIcmpBlock(FWZone *obj,
			gchar *icmp_type)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->icmp_blocks = fw_str_list_remove(priv->icmp_blocks, icmp_type);
}

gboolean
fw_zone_queryIcmpBlock(FWZone *obj,
		       gchar *icmp_type)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    return fw_str_list_contains(priv->icmp_blocks, icmp_type);
}

gboolean
fw_zone_getMasquerade(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->masquerade;
}

void
fw_zone_setMasquerade(FWZone *obj,
		      gboolean masquerade)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->masquerade = masquerade;
}

void
fw_zone_addMasquerade(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->masquerade = TRUE;
}

void
fw_zone_removeMasquerade(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->masquerade = FALSE;
}

gboolean
fw_zone_queryMasquerade(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->masquerade;
}

/**
 * fw_zone_getForwardPorts:
 *
 * Returns: (transfer none) (allow-none) (type FWForwardPortList*)
 */
FWForwardPortList *
fw_zone_getForwardPorts(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->forward_ports;
}

/**
 * fw_zone_setForwardPorts:
 * @obj: (type FWZone*): a FWZone instance
 * @forward_ports: (type FWForwardPortList*)
 */
void
fw_zone_setForwardPorts(FWZone *obj,
			FWForwardPortList *forward_ports)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->forward_ports != NULL)
	fw_forward_port_list_free(priv->forward_ports);
    priv->forward_ports = forward_ports;
}

void
fw_zone_addForwardPort(FWZone *obj,
		       gchar *port,
		       gchar *protocol,
		       gchar *toport,
		       gchar *toaddr)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    FWForwardPort *item = fw_forward_port_new(port, protocol, toport, toaddr);

    fw_forward_port_list_add(priv->forward_ports, item);
}

void
fw_zone_removeForwardPort(FWZone *obj,
			  gchar *port,
			  gchar *protocol,
			  gchar *toport,
			  gchar *toaddr)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    FWForwardPort *item = fw_forward_port_new(port, protocol, toport, toaddr);

    fw_forward_port_list_remove(priv->forward_ports, item);
}

gboolean
fw_zone_queryForwardPort(FWZone *obj,
			 gchar *port,
			 gchar *protocol,
			 gchar *toport,
			 gchar *toaddr)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    FWForwardPort *item = fw_forward_port_new(port, protocol, toport, toaddr);

    return fw_forward_port_list_query(priv->forward_ports, item);
}

void
fw_zone_add_forward_port(FWZone *obj,
			 FWForwardPort *forward_port)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    fw_forward_port_list_add(priv->forward_ports, forward_port);
}

void
fw_zone_remove_forward_port(FWZone *obj,
			    FWForwardPort *forward_port)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    fw_forward_port_list_remove(priv->forward_ports, forward_port);
}

gboolean
fw_zone_query_forward_port(FWZone *obj,
			   FWForwardPort *forward_port)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return fw_forward_port_list_query(priv->forward_ports, forward_port);
}

/**
 * fw_zone_getInterfaces:
 * @obj: (type FWZone*): a FWZone instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_zone_getInterfaces(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->interfaces;
}

/**
 * fw_zone_setInterfaces:
 * @obj: (type FWZone*): a FWZone instance
 * @interfaces: (type GList*) (element-type gchar*)
 */
void
fw_zone_setInterfaces(FWZone *obj,
		      GList *interfaces)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->interfaces != NULL)
	fw_str_list_free(priv->interfaces);
    priv->interfaces = fw_str_list_copy(interfaces);
}

void
fw_zone_addInterface(FWZone *obj,
		     gchar *interface)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->interfaces = fw_str_list_append(priv->interfaces, interface);
}

void
fw_zone_removeInterface(FWZone *obj,
			gchar *interface)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->interfaces = fw_str_list_remove(priv->interfaces, interface);
}

gboolean
fw_zone_queryInterface(FWZone *obj,
		       gchar *interface)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    return fw_str_list_contains(priv->interfaces, interface);
}

/**
 * fw_zone_getSources:
 * @obj: (type FWZone*): a FWZone instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_zone_getSources(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->sources;
}

/**
 * fw_zone_setSources:
 * @obj: (type FWZone*): a FWZone instance
 * @sources: (type GList*) (element-type gchar*)
 */
void
fw_zone_setSources(FWZone *obj,
		   GList *sources)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->sources != NULL)
	fw_str_list_free(priv->sources);
    priv->sources = fw_str_list_copy(sources);
}

void
fw_zone_addSource(FWZone *obj,
		  gchar *source)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->sources = fw_str_list_append(priv->sources, source);
}

void
fw_zone_removeSource(FWZone *obj,
		     gchar *source)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->sources = fw_str_list_remove(priv->sources, source);
}

gboolean
fw_zone_querySource(FWZone *obj,
		    gchar *source)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);
    return fw_str_list_contains(priv->sources, source);
}

/**
 * fw_zone_getRichRules:
 * @obj: (type FWZone*): a FWZone instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_zone_getRichRules(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->rich_rules;
}

/**
 * fw_zone_setRichRules:
 * @obj: (type FWZone*): a FWZone instance
 * @rich_rules: (type GList*) (element-type gchar*)
 */
void
fw_zone_setRichRules(FWZone *obj,
		     GList *rich_rules)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    if (priv->rich_rules != NULL)
	fw_str_list_free(priv->rich_rules);
    priv->rich_rules = fw_str_list_copy(rich_rules);
}

void
fw_zone_addRichRule(FWZone *obj,
		    gchar *rich_rule)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->rich_rules = fw_str_list_append(priv->rich_rules, rich_rule);
}

void
fw_zone_removeRichRule(FWZone *obj,
		       gchar *rich_rule)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->rich_rules = fw_str_list_remove(priv->rich_rules, rich_rule);
}

gboolean
fw_zone_queryRichRule(FWZone *obj,
		      gchar *rich_rule)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return fw_str_list_contains(priv->rich_rules, rich_rule);
}

gboolean
fw_zone_getIcmpBlockInversion(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->icmp_block_inversion;
}

void
fw_zone_setIcmpBlockInversion(FWZone *obj,
			      gboolean icmp_block_inversion)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->icmp_block_inversion = icmp_block_inversion;
}

void
fw_zone_addIcmpBlockInversion(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->icmp_block_inversion = TRUE;
}

void
fw_zone_removeIcmpBlockInversion(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    priv->icmp_block_inversion = FALSE;
}

gboolean
fw_zone_queryIcmpBlockInversion(FWZone *obj)
{
    FWZonePrivate *priv = FW_ZONE_GET_PRIVATE(obj);

    return priv->icmp_block_inversion;
}
