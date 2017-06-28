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

#include "fw_icmptype.h"
#include <string.h>

G_DEFINE_TYPE(FWIcmpType, fw_icmptype, G_TYPE_OBJECT);

#define FW_ICMPTYPE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_ICMPTYPE_TYPE, FWIcmpTypePrivate))

typedef struct {
    gchar *version;            /* string */
    gchar *short_description;  /* string */
    gchar *description;        /* string */
    GList *destinations;       /* list of strings */
} FWIcmpTypePrivate;

FWIcmpType *
fw_icmptype_new()
{
    return g_object_new(FW_ICMPTYPE_TYPE, NULL);
}

FWIcmpType *
fw_icmptype_new_from_variant(GVariant *variant)
{
    FWIcmpType *obj = NULL;
    GVariant *item;
    gchar *str;

    if (variant == NULL)
	return NULL;

    if (strncmp(g_variant_get_type_string(variant), "((sssas))", 9) != 0) {
	if (strncmp(g_variant_get_type_string(variant), "(sssas)", 7) != 0)
	    return NULL;
    } else
	variant = g_variant_get_child_value(variant, 0);

    obj = fw_icmptype_new();

    /* 0: version */
    item = g_variant_get_child_value(variant, 0);
    g_variant_get(item, "s", &str);
    fw_icmptype_setVersion(obj, str);
    g_variant_unref(item);

    /* 1: short */
    item = g_variant_get_child_value(variant, 1);
    g_variant_get(item, "s", &str);
    fw_icmptype_setShort(obj, str);
    g_variant_unref(item);

    /* 2: description */
    item = g_variant_get_child_value(variant, 2);
    g_variant_get(item, "s", &str);
    fw_icmptype_setDescription(obj, str);
    g_variant_unref(item);

    /* 3: destinations */
    item = g_variant_get_child_value(variant, 3);
    fw_icmptype_setDestinations(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    return obj;
}

GVariant *
fw_icmptype_to_variant(FWIcmpType *obj)
{
    GVariant *variant;
    GVariantBuilder *builder_destinations = fw_str_list_to_builder(
	fw_icmptype_getDestinations(obj));

    variant = g_variant_new("(sssas)",
			    fw_icmptype_getVersion(obj),
			    fw_icmptype_getShort(obj),
			    fw_icmptype_getDescription(obj),
			    builder_destinations);

    g_variant_builder_unref(builder_destinations);

    return variant;
}

static void
fw_icmptype_init(FWIcmpType *obj)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    /* init vars */
    priv->version = g_strdup("");
    priv->short_description = g_strdup("");
    priv->description = g_strdup("");
    priv->destinations = NULL;
}

static void
fw_icmptype_finalize(GObject *obj)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    if (priv->version != NULL)
	g_free(priv->version);
    if (priv->short_description != NULL)
	g_free(priv->short_description);
    if (priv->description != NULL)
	g_free(priv->description);
    if (priv->destinations != NULL)
	fw_str_list_free(priv->destinations);

    G_OBJECT_CLASS(fw_icmptype_parent_class)->finalize(obj);
}

static void
fw_icmptype_class_init(FWIcmpTypeClass *fw_icmptype_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_icmptype_class);

    obj_class->finalize = fw_icmptype_finalize;

    g_type_class_add_private(obj_class, sizeof(FWIcmpTypePrivate));
}

/* methods */

/**
 * fw_icmptype_getStr:
 *
 */
void
fw_icmptype_print_str(FWIcmpType *obj)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    g_print("( '%s', '%s', '%s', ",
	    priv->version, priv->short_description, priv->description);

    fw_str_list_print(priv->destinations);

    g_print(" )\n");
}

/**
 * fw_icmptype_getVersion:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_icmptype_getVersion(FWIcmpType *obj)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    return priv->version;
}

void
fw_icmptype_setVersion(FWIcmpType *obj,
		       gchar *version)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    if (priv->version != NULL)
	g_free(priv->version);
    priv->version = g_strdup(version);
}

/**
 * fw_icmptype_getShort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_icmptype_getShort(FWIcmpType *obj)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    return priv->short_description;
}

void
fw_icmptype_setShort(FWIcmpType *obj,
		     gchar *short_description)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    if (priv->short_description != NULL)
	g_free(priv->short_description);
    priv->short_description = g_strdup(short_description);
}

/**
 * fw_icmptype_getDescription:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_icmptype_getDescription(FWIcmpType *obj)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    return priv->description;
}

void
fw_icmptype_setDescription(FWIcmpType *obj,
			   gchar *description)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    if (priv->description != NULL)
	g_free(priv->description);
    priv->description = g_strdup(description);
}

/**
 * fw_icmptype_getDestinations:
 * @obj: (type FWIcmpType*): a FWIcmpType instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_icmptype_getDestinations(FWIcmpType *obj)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    return priv->destinations;
}

/**
 * fw_icmptype_setDestinations:
 * @obj: (type FWIcmpType*): a FWIcmpType instance
 * @destinations: (type GList*) (element-type gchar*)
 */
void
fw_icmptype_setDestinations(FWIcmpType *obj,
			    GList *destinations)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    if (priv->destinations != NULL)
	fw_str_list_free(priv->destinations);
    priv->destinations = fw_str_list_copy(destinations);
}

void
fw_icmptype_addDestination(FWIcmpType *obj,
			   gchar *destination)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    priv->destinations = fw_str_list_append(priv->destinations, destination);
}

void
fw_icmptype_removeDestination(FWIcmpType *obj,
			      gchar *destination)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    priv->destinations = fw_str_list_remove(priv->destinations, destination);
}

gboolean
fw_icmptype_queryDestination(FWIcmpType *obj,
			     gchar *destination)
{
    FWIcmpTypePrivate *priv = FW_ICMPTYPE_GET_PRIVATE(obj);

    return fw_str_list_contains(priv->destinations, destination);
}
