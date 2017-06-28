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

#include "fw_ipset.h"
#include <string.h>

G_DEFINE_TYPE(FWIPSet, fw_ipset, G_TYPE_OBJECT);

#define FW_IPSET_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_IPSET_TYPE, FWIPSetPrivate))

typedef struct {
    gchar *version;            /* string */
    gchar *short_description;  /* string */
    gchar *description;        /* string */
    gchar *type;               /* string */
    GHashTable *options;       /* hash table of string: string */
    GList *entries;            /* list of string */
} FWIPSetPrivate;

FWIPSet *
fw_ipset_new()
{
    return g_object_new(FW_IPSET_TYPE, NULL);
}

FWIPSet *
fw_ipset_new_from_variant(GVariant *variant)
{
    FWIPSet *obj = NULL;
    GVariant *item;
    GVariantIter iter;
    gchar *str;

    if (variant == NULL)
	return NULL;

    if (strncmp(g_variant_get_type_string(variant),
		"((ssssa{ss}as))", 11) != 0)
	return NULL;

    variant = g_variant_get_child_value(variant, 0);

    obj = fw_ipset_new();

    /* 0: version */
    item = g_variant_get_child_value(variant, 0);
    g_variant_get(item, "s", &str);
    fw_ipset_setVersion(obj, str);
    g_variant_unref(item);

    /* 1: short */
    item = g_variant_get_child_value(variant, 1);
    g_variant_get(item, "s", &str);
    fw_ipset_setShort(obj, str);
    g_variant_unref(item);

    /* 2: description */
    item = g_variant_get_child_value(variant, 2);
    g_variant_get(item, "s", &str);
    fw_ipset_setDescription(obj, str);
    g_variant_unref(item);

    /* 3: type */
    item = g_variant_get_child_value(variant, 3);
    g_variant_get(item, "s", &str);
    fw_ipset_setType(obj, str);
    g_variant_unref(item);

    /* 4: options */
    item = g_variant_get_child_value(variant, 4);
    if (g_variant_iter_init(&iter, item)) {
       GVariant *element;

       while ((element = g_variant_iter_next_value(&iter)) != NULL) {
           gchar *key, *value;
           GVariant *item2=NULL;

           item2 = g_variant_get_child_value(element, 0);
           g_variant_get(item2, "s", &key);
           g_variant_unref(item2);

           item2 = g_variant_get_child_value(element, 1);
           g_variant_get(item2, "s", &value);
           g_variant_unref(item2);

           fw_ipset_setOption(obj, g_strdup(key), g_strdup(value));
           g_variant_unref(element);
       }
    }

    g_variant_unref(item);

    /* 5: entries */
    item = g_variant_get_child_value(variant, 5);
    fw_ipset_setEntries(obj, fw_str_list_new_from_variant(item));
    g_variant_unref(item);

    return obj;
}

GVariant *
fw_ipset_to_variant(FWIPSet *obj)
{
    GVariant *variant;
    GVariantBuilder *builder_entries = fw_str_list_to_builder(
	fw_ipset_getEntries(obj));

    GVariantBuilder builder_options;
    GHashTable *hash;
    GHashTableIter iter;
    gpointer key, value;

    g_variant_builder_init(&builder_options, G_VARIANT_TYPE("a{ss}"));
    hash = fw_ipset_getOptions(obj);
    g_hash_table_iter_init(&iter, hash);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
	g_variant_builder_add(&builder_options, "{ss}", key, value);
    }

    variant = g_variant_new("((ssssa{ss}as))",
			    fw_ipset_getVersion(obj),
			    fw_ipset_getShort(obj),
			    fw_ipset_getDescription(obj),
			    fw_ipset_getType(obj),
			    &builder_options,
			    builder_entries);

    g_variant_builder_unref(builder_entries);

    return variant;
}

static void
fw_ipset_init(FWIPSet *obj)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    /* init vars */
    priv->version = g_strdup("");
    priv->short_description = g_strdup("");
    priv->description = g_strdup("");
    priv->type = g_strdup("");
    priv->options = g_hash_table_new(g_str_hash, g_str_equal);
    priv->entries = NULL;
}

static void
fw_ipset_finalize(GObject *obj)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    if (priv->version != NULL)
	g_free(priv->version);
    if (priv->short_description != NULL)
	g_free(priv->short_description);
    if (priv->description != NULL)
	g_free(priv->description);
    if (priv->type != NULL)
	g_free(priv->type);
    if (priv->options != NULL) {
	g_hash_table_destroy(priv->options);
	/* g_free(priv->options); */
    }
    if (priv->entries != NULL)
	fw_str_list_free(priv->entries);

    G_OBJECT_CLASS(fw_ipset_parent_class)->finalize(obj);
}

static void
fw_ipset_class_init(FWIPSetClass *fw_ipset_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_ipset_class);

    obj_class->finalize = fw_ipset_finalize;

    g_type_class_add_private(obj_class, sizeof(FWIPSetPrivate));
}

/* methods */

/**
 * fw_ipset_getStr:
 *
 */
void
fw_ipset_print_str(FWIPSet *obj)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);
    GList *list;
    gchar *str;
    gint i;

    g_print("( '%s', '%s', '%s', '%s', ",
	    priv->version, priv->short_description,
	    priv->description, priv->type);

    g_print("{ ");

    list = g_hash_table_get_keys(priv->options);
    for (i=0; i<g_list_length(list); i++) {
	str = (gchar *) g_list_nth_data(list, i);
	g_print("%s'%s': '%s'", (i>0) ? ", " : "", str, 
		(gchar *)g_hash_table_lookup(priv->options, str));
    }

    g_print(" }, ");

    fw_str_list_print(priv->entries);

    g_print(" )\n");
}

/**
 * fw_ipset_getVersion:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_ipset_getVersion(FWIPSet *obj)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    return priv->version;
}

void
fw_ipset_setVersion(FWIPSet *obj,
		    gchar *version)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    if (priv->version != NULL)
	g_free(priv->version);
    priv->version = g_strdup(version);
}

/**
 * fw_ipset_getShort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_ipset_getShort(FWIPSet *obj)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    return priv->short_description;
}

void
fw_ipset_setShort(FWIPSet *obj,
		  gchar *short_description)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    if (priv->short_description != NULL)
	g_free(priv->short_description);
    priv->short_description = g_strdup(short_description);
}

/**
 * fw_ipset_getDescription:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_ipset_getDescription(FWIPSet *obj)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    return priv->description;
}

void
fw_ipset_setDescription(FWIPSet *obj,
			gchar *description)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    if (priv->description != NULL)
	g_free(priv->description);
    priv->description = g_strdup(description);
}

/**
 * fw_ipset_getType:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_ipset_getType(FWIPSet *obj)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    return priv->type;
}

void
fw_ipset_setType(FWIPSet *obj,
		 gchar *type)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    if (priv->type != NULL)
	g_free(priv->type);
    priv->type = g_strdup(type);
}

/**
 * fw_ipset_getOptions:
 * @obj: (type FWIPSet*): a FWIPSet instance
 *
 * Returns: (transfer none) (allow-none) (type GHashTable*) (element-type gchar* gchar*)
 */
GHashTable *
fw_ipset_getOptions(FWIPSet *obj)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    return priv->options;
}

/**
 * fw_ipset_setOptions:
 * @obj: (type FWIPSet*): a FWIPSet instance
 * @options: (type GHashTable*) (element-type gchar* gchar*)
 */
void fw_ipset_setOptions(FWIPSet *obj,
			 GHashTable *options)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    if (priv->options != NULL) {
	g_hash_table_destroy(priv->options);
	g_free(priv->options);
    }
    priv->options = options;
}

void
fw_ipset_setOption(FWIPSet *obj,
		   gchar *key,
		   gchar *value)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    g_hash_table_replace(priv->options, key, value);
}

void
fw_ipset_removeOption(FWIPSet *obj,
		      gchar *key)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    g_hash_table_remove(priv->options, key);
}

gboolean
fw_ipset_queryOption(FWIPSet *obj,
		       gchar *key,
		       gchar *value)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);
    gchar *val;

    val = g_hash_table_lookup(priv->options, key);
    return (strcmp(value, val) == 0);
}

/**
 * fw_ipset_getEntries:
 * @obj: (type FWIPSet*): a FWIPSet instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_ipset_getEntries(FWIPSet *obj)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    return priv->entries;
}

/**
 * fw_ipset_setEntries:
 * @obj: (type FWIPSet*): a FWIPSet instance
 * @entries: (type GList*) (element-type gchar*)
 */
void
fw_ipset_setEntries(FWIPSet *obj,
		    GList *entries)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    if (priv->entries != NULL)
	fw_str_list_free(priv->entries);
    priv->entries = fw_str_list_copy(entries);
}

void
fw_ipset_addEntry(FWIPSet *obj,
		  gchar *entry)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    priv->entries = fw_str_list_append(priv->entries, entry);
}

void
fw_ipset_removeEntry(FWIPSet *obj,
		     gchar *entry)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    priv->entries = fw_str_list_remove(priv->entries, entry);
}

gboolean
fw_ipset_queryEntry(FWIPSet *obj,
		    gchar *entry)
{
    FWIPSetPrivate *priv = FW_IPSET_GET_PRIVATE(obj);

    return fw_str_list_contains(priv->entries, entry);
}

