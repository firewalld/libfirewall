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

#include "fw_service.h"
#include <string.h>

G_DEFINE_TYPE(FWService, fw_service, G_TYPE_OBJECT);

#define FW_SERVICE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_SERVICE_TYPE, FWServicePrivate))

typedef struct {
    gchar *version;            /* string */
    gchar *short_description;  /* string */
    gchar *description;        /* string */
    FWPortList *ports;         /* list of FWPort */
    GList *modules;            /* list of string */
    GHashTable *destinations;  /* hash table of string: string */
    GList *protocols;          /* list of string */
    FWPortList *source_ports;  /* list of FWPort */
} FWServicePrivate;

FWService *
fw_service_new()
{
    return g_object_new(FW_SERVICE_TYPE, NULL);
}

/**
 * fw_service_new_from_variant:
 * @variant: (type GVariant*)
 * Returns: (transfer none) (allow-none) (type FWService*)
 */
FWService *
fw_service_new_from_variant(GVariant *variant)
{
    FWService *obj = NULL;
    GVariant *item;
    GVariantIter iter;
    gchar *str;

    if (variant == NULL)
	return NULL;

    if (strncmp(g_variant_get_type_string(variant),
		"((sssa(ss)asa{ss}asa(ss)))", 26) != 0)
	return NULL;

    /* get sssa(ss)asa{ss}asa(ss) from (sssa(ss)asa{ss}asa(ss)) */
    variant = g_variant_get_child_value(variant, 0);

    obj = fw_service_new();

    /* 0: version */
    item = g_variant_get_child_value(variant, 0);
    g_variant_get(item, "s", &str);
    fw_service_setVersion(obj, str);
    g_variant_unref(item);

    /* 1: short */
    item = g_variant_get_child_value(variant, 1);
    g_variant_get(item, "s", &str);
    fw_service_setShort(obj, str);
    g_variant_unref(item);

    /* 2: description */
    item = g_variant_get_child_value(variant, 2);
    g_variant_get(item, "s", &str);
    fw_service_setDescription(obj, str);
    g_variant_unref(item);

    /* 3: ports */
    item = g_variant_get_child_value(variant, 3);
    fw_service_setPorts(obj, fw_port_list_new_from_variant(item));
    g_variant_unref(item);

    /* 4: modules */
    item = g_variant_get_child_value(variant, 4);
    fw_service_setModules(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    /* 5: destinations */
    item = g_variant_get_child_value(variant, 5);
    if (g_variant_iter_init(&iter, item)) {
       GVariant *element;

       while ((element = g_variant_iter_next_value(&iter)) != NULL) {
           GVariant *item2=NULL;
           gchar *key, *value;

           item2 = g_variant_get_child_value(element, 0);
           g_variant_get(item2, "s", &key);
           g_variant_unref(item2);

           item2 = g_variant_get_child_value(element, 1);
           g_variant_get(item2, "s", &value);
           g_variant_unref(item2);

           fw_service_setDestination(obj, g_strdup(key), g_strdup(value));
           g_variant_unref(element);
       }
    }
    g_variant_unref(item);

    /* 6: protocols */
    item = g_variant_get_child_value(variant, 6);
    fw_service_setProtocols(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    /* 7: source ports */
    item = g_variant_get_child_value(variant, 7);
    fw_service_setSourcePorts(obj, fw_port_list_new_from_variant(item));
    g_variant_unref(item);

    return obj;
}

GVariant *
fw_service_to_variant(FWService *obj)
{
    GVariant *variant;
    GVariantBuilder *builder_ports = fw_port_list_to_builder(
	fw_service_getPorts(obj));
    GVariantBuilder *builder_modules = fw_str_list_to_builder(
	fw_service_getModules(obj));
    GVariantBuilder *builder_protocols = fw_str_list_to_builder(
	fw_service_getProtocols(obj));
    GVariantBuilder *builder_source_ports = fw_port_list_to_builder(
	fw_service_getSourcePorts(obj));

    GVariantBuilder builder_destinations;
    GHashTable *hash;
    GHashTableIter iter;
    gpointer key, value;
    g_variant_builder_init(&builder_destinations, G_VARIANT_TYPE("a{ss}"));
    hash = fw_service_getDestinations(obj);
    g_hash_table_iter_init(&iter, hash);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
	g_variant_builder_add(&builder_destinations, "{ss}", key, value);
    }

    variant = g_variant_new("(sssa(ss)asa{ss}asa(ss))",
			    fw_service_getVersion(obj),
			    fw_service_getShort(obj),
			    fw_service_getDescription(obj),
			    builder_ports,
			    builder_modules,
			    &builder_destinations,
			    builder_protocols,
			    builder_source_ports);

    g_variant_builder_unref(builder_ports);
    g_variant_builder_unref(builder_modules);
    /* g_variant_builder_unref(builder_destinations); */
    g_variant_builder_unref(builder_protocols);
    g_variant_builder_unref(builder_source_ports);

    return variant;
}

static void
fw_service_init(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    /* init vars */
    priv->version = g_strdup("");
    priv->short_description = g_strdup("");
    priv->description = g_strdup("");
    priv->ports = fw_port_list_new();
    priv->modules = NULL;
    priv->destinations = g_hash_table_new(g_str_hash, g_str_equal);
    priv->protocols = NULL;
    priv->source_ports = fw_port_list_new();
}

static void
fw_service_finalize(GObject *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    
    if (priv->version != NULL)
	g_free(priv->version);
    if (priv->short_description != NULL)
	g_free(priv->short_description);
    if (priv->description != NULL)
	g_free(priv->description);
    if (priv->ports != NULL)
	fw_port_list_free(priv->ports);
    if (priv->modules != NULL)
	fw_str_list_free(priv->modules);
    if (priv->destinations != NULL)
	fw_str_hash_table_free(priv->destinations);
    if (priv->protocols != NULL)
	fw_str_list_free(priv->protocols);
    if (priv->source_ports != NULL)
	fw_port_list_free(priv->source_ports);
    G_OBJECT_CLASS(fw_service_parent_class)->finalize(obj);
}

static void
fw_service_class_init(FWServiceClass *fw_service_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_service_class);

    obj_class->finalize = fw_service_finalize;

    g_type_class_add_private(obj_class, sizeof(FWServicePrivate));
}

/* methods */

/**
 * fw_service_print_str:
 *
 */
void
fw_service_print_str(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    GList *list;
    gchar *str;
    gint i;

    g_print("( '%s', '%s', '%s', ",
	    priv->version, priv->short_description,
	    priv->description);

    fw_port_list_print_str(priv->ports);

    g_print(", ");

    fw_str_list_print(priv->modules);

    g_print(", { ");

    list = g_hash_table_get_keys(priv->destinations);
    for (i=0; i<g_list_length(list); i++) {
	str = (gchar *) g_list_nth_data(list, i);
	g_print("%s'%s': '%s'", (i>0) ? ", " : "", str, 
		(gchar *)g_hash_table_lookup(priv->destinations, str));
    }
    g_list_free(list);

    g_print(" }, ");

    fw_str_list_print(priv->protocols);

    g_print(", ");

    fw_port_list_print_str(priv->source_ports);

    g_print(" )\n");
}

/**
 * fw_service_print_simple:
 *
 */
void
fw_service_print_simple(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    GList *keys;
    gint i;

    g_print("  ports: ");
    fw_port_list_print_simple(priv->ports);
    g_print("\n");

    g_print("  protocols: ");
    fw_str_list_print_simple(priv->protocols);
    g_print("\n");

    g_print("  source-ports: ");
    fw_port_list_print_simple(priv->source_ports);
    g_print("\n");

    g_print("  modules: ");
    fw_str_list_print_simple(priv->modules);
    g_print("\n");

    g_print("  destinations: ");
    keys = g_hash_table_get_keys(priv->destinations);
    for (i=0; i<g_list_length(keys); i++) {
	gchar *str = (gchar *) g_list_nth_data(keys, i);
	g_print("%s%s:%s", (i>0) ? ", " : "", str, 
		(gchar *)g_hash_table_lookup(priv->destinations, str));
    }
    g_print("\n");
}

/**
 * fw_service_getVersion:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_service_getVersion(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return priv->version;
}

void
fw_service_setVersion(FWService *obj,
		   gchar *version)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    if (priv->version != NULL)
	g_free(priv->version);
    priv->version = g_strdup(version);
}

/**
 * fw_service_getShort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_service_getShort(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return priv->short_description;
}

void
fw_service_setShort(FWService *obj,
		 gchar *short_description)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    if (priv->short_description != NULL)
	g_free(priv->short_description);
    priv->short_description = g_strdup(short_description);
}

/**
 * fw_service_getDescription:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_service_getDescription(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return priv->description;
}

void
fw_service_setDescription(FWService *obj,
		       gchar *description)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    if (priv->description != NULL)
	g_free(priv->description);
    priv->description = g_strdup(description);
}

/**
 * fw_service_getPorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_service_getPorts(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return priv->ports;
}

/**
 * fw_service_setPorts:
 * @obj: (type FWService*): a FWService instance
 * @ports: (type FWPortList*)
 */
void
fw_service_setPorts(FWService *obj,
		    FWPortList *ports)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    if (priv->ports != NULL)
	fw_port_list_free(priv->ports);
    priv->ports = ports;
}

void
fw_service_addPort(FWService *obj,
		   gchar *port,
		   gchar *protocol)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    fw_port_list_add(priv->ports, item);
}

void
fw_service_removePort(FWService *obj,
		      gchar *port,
		      gchar *protocol)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    fw_port_list_remove(priv->ports, item);
}

gboolean
fw_service_queryPort(FWService *obj,
		     gchar *port,
		     gchar *protocol)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    return fw_port_list_query(priv->ports, item);
}

void
fw_service_add_port(FWService *obj,
		    FWPort *port)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    fw_port_list_add(priv->ports, port);
}

void
fw_service_remove_port(FWService *obj,
		       FWPort *port)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    fw_port_list_remove(priv->ports, port);
}

gboolean
fw_service_query_port(FWService *obj,
		      FWPort *port)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return fw_port_list_query(priv->ports, port);
}

/**
 * fw_service_getProtocols:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_service_getProtocols(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return priv->protocols;
}

/**
 * fw_service_setProtocols:
 * @obj: (type FWService*): a FWService instance
 * @protocols: (type GList*) (element-type gchar*)
 */
void
fw_service_setProtocols(FWService *obj,
			GList *protocols)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    if (priv->protocols != NULL)
	fw_str_list_free(priv->protocols);
    priv->protocols = fw_str_list_copy(protocols);
}

void
fw_service_addProtocol(FWService *obj,
		       gchar *protocol)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    priv->protocols = fw_str_list_append(priv->protocols, protocol);
}

void
fw_service_removeProtocol(FWService *obj,
			  gchar *protocol)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    priv->protocols = fw_str_list_remove(priv->protocols, protocol);
}

gboolean
fw_service_queryProtocol(FWService *obj,
			 gchar *protocol)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return fw_str_list_contains(priv->protocols, protocol);
}

/**
 * fw_service_getSourcePorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_service_getSourcePorts(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return priv->source_ports;
}

/**
 * fw_service_setSourcePorts:
 * @obj: (type FWService*): a FWService instance
 * @ports: (type FWPortList*)
 */
void
fw_service_setSourcePorts(FWService *obj,
			  FWPortList *ports)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    if (priv->source_ports != NULL)
	fw_port_list_free(priv->source_ports);
    priv->source_ports = ports;
}

void
fw_service_addSourcePort(FWService *obj,
			 gchar *port,
			 gchar *protocol)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    fw_port_list_add(priv->source_ports, item);
}

void
fw_service_removeSourcePort(FWService *obj,
			    gchar *port,
			    gchar *protocol)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    fw_port_list_remove(priv->source_ports, item);
}

gboolean
fw_service_querySourcePort(FWService *obj,
			   gchar *port,
			   gchar *protocol)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    return fw_port_list_query(priv->source_ports, item);
}

void
fw_service_add_source_port(FWService *obj,
			   FWPort *port)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    fw_port_list_add(priv->source_ports, port);
}

void
fw_service_remove_source_port(FWService *obj,
			      FWPort *port)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    fw_port_list_remove(priv->source_ports, port);
}

gboolean
fw_service_query_source_port(FWService *obj,
			     FWPort *port)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return fw_port_list_query(priv->source_ports, port);
}

/**
 * fw_service_getModules:
 * @obj: (type FWService*): a FWService instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_service_getModules(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return priv->modules;
}

/**
 * fw_service_setModules:
 * @obj: (type FWService*): a FWService instance
 * @modules: (type GList*) (element-type gchar*)
 */
void
fw_service_setModules(FWService *obj,
		      GList *modules)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    if (priv->modules != NULL)
	fw_str_list_free(priv->modules);
    priv->protocols = fw_str_list_copy(modules);
}

void
fw_service_addModule(FWService *obj,
		     gchar *module)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    priv->modules = fw_str_list_append(priv->modules, module);
}

void
fw_service_removeModule(FWService *obj,
			gchar *module)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    priv->modules = fw_str_list_remove(priv->modules, module);
}

gboolean
fw_service_queryModule(FWService *obj,
		       gchar *module)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return fw_str_list_contains(priv->modules, module);
}

/**
 * fw_service_getDestinations:
 * @obj: (type FWService*): a FWService instance
 *
 * Returns: (transfer none) (allow-none) (type GHashTable*) (element-type gchar* gchar*)
 */
GHashTable *fw_service_getDestinations(FWService *obj)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    return priv->destinations;
}

/**
 * fw_service_setDestinations:
 * @obj: (type FWService*): a FWService instance
 * @destinations: (type GHashTable*) (element-type gchar* gchar*)
 */
void fw_service_setDestinations(FWService *obj,
				GHashTable *destinations)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    priv->destinations = fw_str_hash_table_copy(destinations);
}

void
fw_service_setDestination(FWService *obj,
			  gchar *ipv,
			  gchar *address)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    g_hash_table_replace(priv->destinations, g_strdup(ipv), g_strdup(address));
}

void
fw_service_removeDestination(FWService *obj,
			     gchar *ipv)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);

    g_hash_table_remove(priv->destinations, ipv);
}

gboolean
fw_service_queryDestination(FWService *obj,
			    gchar *ipv,
			    gchar *address)
{
    FWServicePrivate *priv = FW_SERVICE_GET_PRIVATE(obj);
    gchar *value;

    value = g_hash_table_lookup(priv->destinations, ipv);
    return (strcmp(value, address) == 0);
}
