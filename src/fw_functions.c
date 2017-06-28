/*
 * Copyright (C) 2013,2014 Red Hat, Inc.
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
 *
 */

#include <string.h>
#include "fw_functions.h"

/**
 * fw_str_equal:
 * @a: (type gconstpointer)
 * @b: (type gconstpointer)
 *
 */
gint
/*
g_list_str_equal(gconstpointer a,
                 gconstpointer b)
*/
fw_str_equal(gconstpointer a,
	     gconstpointer b)
{
    if (a && b)
        return !g_str_equal(a, b);
    return -1;
}

/**
 * fw_str_list_contains:
 * @list: (type GList*) (element-type gchar*)
 * @str: (type gchar*)
 *
 */
gboolean
/*
g_list_contains_str(GList *list,
		    const gchar *str)
*/
fw_str_list_contains(GList *list,
		     const gchar *str)
{
    if (list == NULL)
	return FALSE;
    return (g_list_find_custom(list, (gpointer) str, fw_str_equal) != NULL);
}

/**
 * fw_str_list_print_simple:
 * @list: (type GList*) (element-type gchar*)
 *
 */
void
/*
g_list_print_str_simple(GList *list)
*/
fw_str_list_print_simple(GList *list)
{
    gint i;

    for (i=0; i<g_list_length(list); i++) {
	g_print("%s%s", (i>0) ? " " : "", (char *)g_list_nth_data(list, i));
    }
}

/**
 * fw_str_list_print:
 * @list: (type GList*) (element-type gchar*)
 *
 */
void
/*
g_list_print_str(GList *list)
*/
fw_str_list_print(GList *list)
{
    gint i;

    g_print("[ ");
    for (i=0; i<g_list_length(list); i++) {
	g_print("%s'%s'", (i>0) ? ", " : "", (char *)g_list_nth_data(list, i));
    }
    g_print(" ]");
}

/**
 * fw_str_list_append:
 * @list: (type GList*) (element-type gchar*)
 * @str: (type gchar*)
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
/*
g_list_append_str(GList *list,
                  const gchar *str)
*/
fw_str_list_append(GList *list,
		   const gchar *str)
{
    return g_list_append(list, g_strdup(str));
}

/**
 * fw_str_list_copy:
 * @list: (type GList*) (element-type gchar*)
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
/*
g_list_copy_str(GList *list)
*/
fw_str_list_copy(GList *list)
{
    return g_list_copy_deep(list, (GCopyFunc) g_strdup, NULL);
}

/**
 * fw_str_list_remove:
 * @list: (type GList*) (element-type gchar*)
 * @str: (type gchar*)
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
/*
g_list_remove_str(GList *list,
		  const gchar *str)
*/
fw_str_list_remove(GList *list,
		   const gchar *str)
{
    GList *to_remove;

    if (list == NULL)
	return FALSE;
    to_remove = g_list_find_custom(list, (gpointer) str, fw_str_equal);
    if (to_remove != NULL)
	return g_list_remove_link(list, to_remove);
    return list;
}

/**
 * fw_str_list_free:
 * @list: (type GList*) (element-type gchar*)
 */
void
/*
g_list_free_str(GList *list)
*/
fw_str_list_free(GList *list)
{
    g_list_free_full(list, g_free);
}

/**
 * fw_str_hash_table_copy:
 * @hash: (type GHashTable*) (element-type gchar* gchar*)
 * Returns: (transfer none) (allow-none) (type GHashTable*) (element-type gchar* gchar*)
 */
GHashTable *
/*
g_hash_table_copy(GHashTable *hash)
*/
fw_str_hash_table_copy(GHashTable *hash)
{
    GHashTable *ret = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTableIter iter;
    gpointer key, value;
 
    g_hash_table_iter_init(&iter, hash);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        g_hash_table_insert(ret, g_strdup(key), g_strdup(value));
    }

    return ret;
}

/**
 * fw_str_hash_table_free:
 * @hash: (type GHashTable*) (element-type gchar* gchar*)
 */
void
/*
g_hash_table_free(GHashTable *hash)
*/
fw_str_hash_table_free(GHashTable *hash)
{
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, hash);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
	g_free(key);
	g_free(value);
    }
    g_hash_table_destroy(hash);
}

/**
 * fw_str_list_to_variant:
 * @list: (type GList*) (element-type gchar*)
 * Returns: (transfer none) (allow-none) (type GVariant*)
 */
GVariant *
fw_str_list_to_variant(GList *list)
{
    GVariantBuilder *builder = fw_str_list_to_builder(list);
    GVariant *variant = g_variant_new("(as)", builder);
    g_variant_builder_unref(builder);
    
    return variant;
}

/**
 * fw_str_list_to_builder:
 * @list: (type GList*) (element-type gchar*)
 * Returns: (transfer none) (allow-none) (type GVariantBuilder*)
 */
GVariantBuilder *
fw_str_list_to_builder(GList *list)
{
    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
    gint i;

    for (i=0; i<g_list_length(list); i++)
	g_variant_builder_add(builder, "s", g_list_nth_data(list, i));

    return builder;
}

/**
 * fw_str_list_new_from_variant:
 * @variant: (type GVariant*)
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_str_list_new_from_variant(GVariant *variant)
{
    GList *list = NULL;
    GVariantIter iter;
    gchar *str;
    GVariant *element;

    if (strncmp(g_variant_get_type_string(variant), "(as)", 4) != 0) {
	if (strncmp(g_variant_get_type_string(variant), "as", 2) != 0)
	    return NULL;
    } else
	variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    g_variant_get(element, "s", &str);
	    list = g_list_append(list, g_strdup(str));
	    g_variant_unref(element);
	}
    }
    g_variant_unref(variant);

    return list;
}
