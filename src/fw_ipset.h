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

#ifndef __FW_IPSET_H__
#define __FW_IPSET_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"
#include "fw_functions.h"

#define FW_IPSET_TYPE            (fw_ipset_get_type())
#define FW_IPSET(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_IPSET_TYPE, FWIPSet))
#define FW_IPSET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_IPSET_TYPE, FWIPSetClass))
#define FW_IS_IPSET(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_IPSET_TYPE, FWIPSetClass))
#define FW_IPSET_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_IPSET_TYPE, FWIPSetClass))

typedef struct {
    GObject parent;
} FWIPSet;

typedef struct {
    GObjectClass parent;
} FWIPSetClass;

GType fw_ipset_get_type(void);
FWIPSet *fw_ipset_new(void);
FWIPSet *fw_ipset_new_from_variant(GVariant *variant);
GVariant *fw_ipset_to_variant(FWIPSet *obj);

void fw_ipset_print_str(FWIPSet *obj);

gchar *fw_ipset_getVersion(FWIPSet *obj);
void fw_ipset_setVersion(FWIPSet *obj, gchar *version);

gchar *fw_ipset_getShort(FWIPSet *obj);
void fw_ipset_setShort(FWIPSet *obj, gchar *short_description);

gchar *fw_ipset_getDescription(FWIPSet *obj);
void fw_ipset_setDescription(FWIPSet *obj, gchar *description);

gchar *fw_ipset_getType(FWIPSet *obj);
void fw_ipset_setType(FWIPSet *obj, gchar *type);

GHashTable *fw_ipset_getOptions(FWIPSet *obj);
void fw_ipset_setOptions(FWIPSet *obj, GHashTable *options);
void fw_ipset_setOption(FWIPSet *obj, gchar *key, gchar *value);
void fw_ipset_removeOption(FWIPSet *obj, gchar *key);
gboolean fw_ipset_queryOption(FWIPSet *obj, gchar *key, gchar *value);

GList *fw_ipset_getEntries(FWIPSet *obj);
void fw_ipset_setEntries(FWIPSet *obj, GList *entries);
void fw_ipset_addEntry(FWIPSet *obj, gchar *entry);
void fw_ipset_removeEntry(FWIPSet *obj, gchar *entry);
gboolean fw_ipset_queryEntry(FWIPSet *obj, gchar *entry);

#endif /* __FW_IPSET_H__ */
