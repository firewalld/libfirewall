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

#ifndef __FW_FUNCTIONS_H__
#define __FW_FUNCTIONS_H__

#include <glib.h>

/*
gint g_list_str_equal(gconstpointer a, gconstpointer b);
gboolean g_list_contains_str(GList *list, const gchar *str);
void g_list_print_str_simple(GList *list);
void g_list_print_str(GList *list);
GList *g_list_append_str(GList *list, const gchar *str);
GList *g_list_copy_str(GList *list);
GList *g_list_remove_str(GList *list, const gchar *str);
void g_list_free_str(GList *list);

GHashTable *g_hash_table_copy(GHashTable *hash);
void g_hash_table_free(GHashTable *hash);
*/

gint fw_str_equal(gconstpointer a, gconstpointer b);
gboolean fw_str_list_contains(GList *list, const gchar *str);
void fw_str_list_print_simple(GList *list);
void fw_str_list_print(GList *list);
GList *fw_str_list_append(GList *list, const gchar *str);
GList *fw_str_list_copy(GList *list);
GList *fw_str_list_remove(GList *list, const gchar *str);
void fw_str_list_free(GList *list);

GHashTable *fw_str_hash_table_copy(GHashTable *hash);
void fw_str_hash_table_free(GHashTable *hash);

GVariant *fw_str_list_to_variant(GList *list);
GVariantBuilder *fw_str_list_to_builder(GList *list);
GList *fw_str_list_new_from_variant(GVariant *variant);

#endif /* __FW_FUNCTIONS_H__ */
