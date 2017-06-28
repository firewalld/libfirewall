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

#ifndef __FW_CONFIG_ICMPTYPE_H__
#define __FW_CONFIG_ICMPTYPE_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"
#include "fw_port_list.h"
#include "fw_icmptype.h"

#define FW_CONFIG_ICMPTYPE_TYPE            (fw_config_icmptype_get_type())
#define FW_CONFIG_ICMPTYPE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_CONFIG_ICMPTYPE_TYPE, FWConfigFWConfigIcmpType))
#define FW_CONFIG_ICMPTYPE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_CONFIG_ICMPTYPE_TYPE, FWConfigIcmpTypeClass))
#define FW_IS_CONFIG_ICMPTYPE(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_CONFIG_ICMPTYPE_TYPE, FWConfigIcmpTypeClass))
#define FW_CONFIG_ICMPTYPE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_CONFIG_ICMPTYPE_TYPE, FWConfigIcmpTypeClass))

typedef struct {
    GObject parent;
} FWConfigIcmpType;

typedef struct {
    GObjectClass parent;
} FWConfigIcmpTypeClass;

GType fw_config_icmptype_get_type(void);
FWConfigIcmpType *fw_config_icmptype_new(gchar *path);

void fw_config_icmptype_print_str(FWConfigIcmpType *obj);

FWIcmpType *fw_config_icmptype_getSettings(FWConfigIcmpType *obj);
void fw_config_icmptype_update(FWConfigIcmpType *obj, FWIcmpType *settings);
void fw_config_icmptype_loadDefaults(FWConfigIcmpType *obj);
void fw_config_icmptype_remove(FWConfigIcmpType *obj);
void fw_config_icmptype_rename(FWConfigIcmpType *obj, gchar *name);

gchar *fw_config_icmptype_getVersion(FWConfigIcmpType *obj);
void fw_config_icmptype_setVersion(FWConfigIcmpType *obj, gchar *version);

gchar *fw_config_icmptype_getShort(FWConfigIcmpType *obj);
void fw_config_icmptype_setShort(FWConfigIcmpType *obj, gchar *short_description);

gchar *fw_config_icmptype_getDescription(FWConfigIcmpType *obj);
void fw_config_icmptype_setDescription(FWConfigIcmpType *obj, gchar *description);

GList *fw_config_icmptype_getDestinations(FWConfigIcmpType *obj);
void fw_config_icmptype_setDestinations(FWConfigIcmpType *obj, GList *destinations);
void fw_config_icmptype_addDestination(FWConfigIcmpType *obj, gchar *destination);
void fw_config_icmptype_removeDestination(FWConfigIcmpType *obj, gchar *destination);
gboolean fw_config_icmptype_queryDestination(FWConfigIcmpType *obj, gchar *destination);

#endif /* __FW_CONFIG_ICMPTYPE_H__ */
