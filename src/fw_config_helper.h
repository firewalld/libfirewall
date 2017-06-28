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

#ifndef __FW_CONFIG_HELPER_H__
#define __FW_CONFIG_HELPER_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"
#include "fw_port_list.h"
#include "fw_helper.h"

#define FW_CONFIG_HELPER_TYPE            (fw_config_helper_get_type())
#define FW_CONFIG_HELPER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_CONFIG_HELPER_TYPE, FWConfigFWConfigHelper))
#define FW_CONFIG_HELPER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_CONFIG_HELPER_TYPE, FWConfigHelperClass))
#define FW_IS_CONFIG_HELPER(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_CONFIG_HELPER_TYPE, FWConfigHelperClass))
#define FW_CONFIG_HELPER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_CONFIG_HELPER_TYPE, FWConfigHelperClass))

typedef struct {
    GObject parent;
} FWConfigHelper;

typedef struct {
    GObjectClass parent;
} FWConfigHelperClass;

GType fw_config_helper_get_type(void);
FWConfigHelper *fw_config_helper_new(gchar *path);

void fw_config_helper_print_str(FWConfigHelper *obj);

FWHelper *fw_config_helper_getSettings(FWConfigHelper *obj);
void fw_config_helper_update(FWConfigHelper *obj, FWHelper *settings);
void fw_config_helper_loadDefaults(FWConfigHelper *obj);
void fw_config_helper_remove(FWConfigHelper *obj);
void fw_config_helper_rename(FWConfigHelper *obj, gchar *name);

gchar *fw_config_helper_getVersion(FWConfigHelper *obj);
void fw_config_helper_setVersion(FWConfigHelper *obj, gchar *version);

gchar *fw_config_helper_getShort(FWConfigHelper *obj);
void fw_config_helper_setShort(FWConfigHelper *obj, gchar *short_description);

gchar *fw_config_helper_getDescription(FWConfigHelper *obj);
void fw_config_helper_setDescription(FWConfigHelper *obj, gchar *description);

gchar *fw_config_helper_getFamily(FWConfigHelper *obj);
void fw_config_helper_setFamily(FWConfigHelper *obj, gchar *family);

gchar *fw_config_helper_getModule(FWConfigHelper *obj);
void fw_config_helper_setModule(FWConfigHelper *obj, gchar *module);

FWPortList *fw_config_helper_getPorts(FWConfigHelper *obj);
void fw_config_helper_setPorts(FWConfigHelper *obj, FWPortList *ports);
void fw_config_helper_addPort(FWConfigHelper *obj, gchar *port, gchar *protocol);
void fw_config_helper_removePort(FWConfigHelper *obj, gchar *port, gchar *protocol);
gboolean fw_config_helper_queryPort(FWConfigHelper *obj, gchar *port, gchar *protocol);

#endif /* __FW_CONFIG_HELPER_H__ */
