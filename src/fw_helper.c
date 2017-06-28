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

#include "fw_helper.h"
#include <string.h>

G_DEFINE_TYPE(FWHelper, fw_helper, G_TYPE_OBJECT);

#define FW_HELPER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_HELPER_TYPE, FWHelperPrivate))

typedef struct {
    gchar *version;            /* string */
    gchar *short_description;  /* string */
    gchar *description;        /* string */
    gchar *family;             /* string */
    gchar *module;             /* string */
    FWPortList *ports;         /* list of FWPort */
} FWHelperPrivate;

FWHelper *
fw_helper_new()
{
    return g_object_new(FW_HELPER_TYPE, NULL);
}

FWHelper *
fw_helper_new_from_variant(GVariant *variant)
{
    FWHelper *obj = NULL;
    GVariant *item;
    gchar *str;

    if (variant == NULL)
	return obj;

    if (strncmp(g_variant_get_type_string(variant),
		"((sssssa(ss)))", 14) != 0)
    {
       return obj;
    }

    /* get sssssa(ss) from (sssssa(ss)) */
    variant = g_variant_get_child_value(variant, 0);

    obj = fw_helper_new();

    /* 0: version */
    item = g_variant_get_child_value(variant, 0);
    g_variant_get(item, "s", &str);
    fw_helper_setVersion(obj, str);
    g_variant_unref(item);

    /* 1: short */
    item = g_variant_get_child_value(variant, 1);
    g_variant_get(item, "s", &str);
    fw_helper_setShort(obj, str);
    g_variant_unref(item);

    /* 2: description */
    item = g_variant_get_child_value(variant, 2);
    g_variant_get(item, "s", &str);
    fw_helper_setDescription(obj, str);
    g_variant_unref(item);

    /* 3: family */
    item = g_variant_get_child_value(variant, 3);
    g_variant_get(item, "s", &str);
    fw_helper_setFamily(obj, str);
    g_variant_unref(item);

    /* 4: module */
    item = g_variant_get_child_value(variant, 4);
    g_variant_get(item, "s", &str);
    fw_helper_setModule(obj, str);
    g_variant_unref(item);

    /* 5: ports */
    item = g_variant_get_child_value(variant, 5);
    fw_helper_setPorts(obj, fw_port_list_new_from_variant(item));
    g_variant_unref(item);

    return obj;
}

GVariant *
fw_helper_to_variant(FWHelper *obj)
{
    GVariant *variant;
    GVariantBuilder *builder = fw_port_list_to_builder(fw_helper_getPorts(obj));
    
    variant = g_variant_new("(sssssa(ss))",
			    fw_helper_getVersion(obj),
			    fw_helper_getShort(obj),
			    fw_helper_getDescription(obj),
			    fw_helper_getFamily(obj),
			    fw_helper_getModule(obj),
			    builder);

    g_variant_builder_unref(builder);

    return variant;
	
}

static void
fw_helper_init(FWHelper *obj)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    /* init vars */
    priv->version = g_strdup("");
    priv->short_description = g_strdup("");
    priv->description = g_strdup("");
    priv->family = g_strdup("");
    priv->module = g_strdup("");
    priv->ports = fw_port_list_new();
}

static void
fw_helper_finalize(GObject *obj)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    if (priv->version != NULL)
	g_free(priv->version);
    if (priv->short_description != NULL)
	g_free(priv->short_description);
    if (priv->description != NULL)
	g_free(priv->description);
    if (priv->family != NULL)
	g_free(priv->family);
    if (priv->module != NULL)
	g_free(priv->module);
    if (priv->ports != NULL)
	fw_port_list_free(priv->ports);
    G_OBJECT_CLASS(fw_helper_parent_class)->finalize(obj);
}

static void
fw_helper_class_init(FWHelperClass *fw_helper_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_helper_class);

    obj_class->finalize = fw_helper_finalize;

    g_type_class_add_private(obj_class, sizeof(FWHelperPrivate));
}

/* methods */

/**
 * fw_helper_print_str:
 *
 */
void
fw_helper_print_str(FWHelper *obj)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);
    gchar *port_str = fw_port_list_getStr(priv->ports);
    
    g_print("( '%s', '%s', '%s', '%s', '%s', %s )\n",
	    priv->version, priv->short_description,
	    priv->description, priv->family, priv->module,
	    port_str);
    g_free(port_str);
}

/**
 * fw_helper_getVersion:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_helper_getVersion(FWHelper *obj)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    return priv->version;
}

void
fw_helper_setVersion(FWHelper *obj,
		   gchar *version)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    if (priv->version != NULL)
	g_free(priv->version);
    priv->version = g_strdup(version);
}

/**
 * fw_helper_getShort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_helper_getShort(FWHelper *obj)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    return priv->short_description;
}

void
fw_helper_setShort(FWHelper *obj,
		 gchar *short_description)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    if (priv->short_description != NULL)
	g_free(priv->short_description);
    priv->short_description = g_strdup(short_description);
}

/**
 * fw_helper_getDescription:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_helper_getDescription(FWHelper *obj)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    return priv->description;
}

void
fw_helper_setDescription(FWHelper *obj,
			 gchar *description)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    if (priv->description != NULL)
	g_free(priv->description);
    priv->description = g_strdup(description);
}

/**
 * fw_helper_getFamily:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_helper_getFamily(FWHelper *obj)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    return priv->family;
}

void
fw_helper_setFamily(FWHelper *obj,
		    gchar *family)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    if (priv->family != NULL)
	g_free(priv->family);
    priv->family = g_strdup(family);
}

/**
 * fw_helper_getModule:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_helper_getModule(FWHelper *obj)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    return priv->module;
}

void
fw_helper_setModule(FWHelper *obj,
		    gchar *module)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    if (priv->module != NULL)
	g_free(priv->module);
    priv->module = g_strdup(module);
}

/**
 * fw_helper_getPorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_helper_getPorts(FWHelper *obj)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    return priv->ports;
}

/**
 * fw_helper_setPorts:
 * @obj: (type FWHelper*): a FWHelper instance
 * @ports: (type FWPortList*)
 */
void
fw_helper_setPorts(FWHelper *obj,
		   FWPortList *ports)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    if (priv->ports != NULL)
	fw_port_list_free(priv->ports);
    priv->ports = ports;
}

void
fw_helper_addPort(FWHelper *obj,
		  gchar *port,
		  gchar *protocol)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    fw_port_list_addPort(priv->ports, port, protocol);
}

void
fw_helper_removePort(FWHelper *obj,
		     gchar *port,
		     gchar *protocol)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    fw_port_list_removePort(priv->ports, port, protocol);
}

gboolean
fw_helper_queryPort(FWHelper *obj,
		    gchar *port,
		    gchar *protocol)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    return fw_port_list_queryPort(priv->ports, port, protocol);
}

void
fw_helper_add_port(FWHelper *obj,
		   FWPort *port)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    fw_port_list_add(priv->ports, port);
}

void
fw_helper_remove_port(FWHelper *obj,
		      FWPort *port)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    fw_port_list_remove(priv->ports, port);
}

gboolean
fw_helper_query_port(FWHelper *obj,
		     FWPort *port)
{
    FWHelperPrivate *priv = FW_HELPER_GET_PRIVATE(obj);

    return fw_port_list_query(priv->ports, port);
}
