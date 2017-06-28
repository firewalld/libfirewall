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

#ifndef __FW_CONFIG_IPSET_H__
#define __FW_CONFIG_IPSET_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"
#include "fw_port_list.h"
#include "fw_ipset.h"

#define FW_CONFIG_IPSET_TYPE            (fw_config_ipset_get_type())
#define FW_CONFIG_IPSET(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_CONFIG_IPSET_TYPE, FWConfigFWConfigIPSet))
#define FW_CONFIG_IPSET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_CONFIG_IPSET_TYPE, FWConfigIPSetClass))
#define FW_IS_CONFIG_IPSET(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_CONFIG_IPSET_TYPE, FWConfigIPSetClass))
#define FW_CONFIG_IPSET_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_CONFIG_IPSET_TYPE, FWConfigIPSetClass))

typedef struct {
    GObject parent;
} FWConfigIPSet;

typedef struct {
    GObjectClass parent;
} FWConfigIPSetClass;

GType fw_config_ipset_get_type(void);
FWConfigIPSet *fw_config_ipset_new(gchar *path);

void fw_config_ipset_print_str(FWConfigIPSet *obj);

FWIPSet *fw_config_ipset_getSettings(FWConfigIPSet *obj);
void fw_config_ipset_update(FWConfigIPSet *obj, FWIPSet *settings);
void fw_config_ipset_loadDefaults(FWConfigIPSet *obj);
void fw_config_ipset_remove(FWConfigIPSet *obj);
void fw_config_ipset_rename(FWConfigIPSet *obj, gchar *name);

gchar *fw_config_ipset_getVersion(FWConfigIPSet *obj);
void fw_config_ipset_setVersion(FWConfigIPSet *obj, gchar *version);

gchar *fw_config_ipset_getShort(FWConfigIPSet *obj);
void fw_config_ipset_setShort(FWConfigIPSet *obj, gchar *short_description);

gchar *fw_config_ipset_getDescription(FWConfigIPSet *obj);
void fw_config_ipset_setDescription(FWConfigIPSet *obj, gchar *description);

gchar *fw_config_ipset_getType(FWConfigIPSet *obj);
void fw_config_ipset_setType(FWConfigIPSet *obj, gchar *type);

GHashTable *fw_config_ipset_getOptions(FWConfigIPSet *obj);
void fw_config_ipset_setOptions(FWConfigIPSet *obj, GHashTable *options);
void fw_config_ipset_setOption(FWConfigIPSet *obj, gchar *key, gchar *value);
void fw_config_ipset_removeOption(FWConfigIPSet *obj, gchar *key);
gboolean fw_config_ipset_queryOption(FWConfigIPSet *obj, gchar *key, gchar *value);

GList *fw_config_ipset_getEntries(FWConfigIPSet *obj);
void fw_config_ipset_setEntries(FWConfigIPSet *obj, GList *entries);
void fw_config_ipset_addEntry(FWConfigIPSet *obj, gchar *entry);
void fw_config_ipset_removeEntry(FWConfigIPSet *obj, gchar *entry);
gboolean fw_config_ipset_queryEntry(FWConfigIPSet *obj, gchar *entry);

#endif /* __FW_CONFIG_IPSET_H__ */
