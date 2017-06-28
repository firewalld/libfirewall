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

#ifndef __FW_HELPER_H__
#define __FW_HELPER_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"
#include "fw_port_list.h"

#define FW_HELPER_TYPE            (fw_helper_get_type())
#define FW_HELPER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_HELPER_TYPE, FWHelper))
#define FW_HELPER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_HELPER_TYPE, FWHelperClass))
#define FW_IS_HELPER(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_HELPER_TYPE, FWHelperClass))
#define FW_HELPER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_HELPER_TYPE, FWHelperClass))

typedef struct {
    GObject parent;
} FWHelper;

typedef struct {
    GObjectClass parent;
} FWHelperClass;

GType fw_helper_get_type(void);
FWHelper *fw_helper_new(void);
FWHelper *fw_helper_new_from_variant(GVariant *variant);
GVariant *fw_helper_to_variant(FWHelper *obj);

void fw_helper_print_str(FWHelper *obj);

gchar *fw_helper_getVersion(FWHelper *obj);
void fw_helper_setVersion(FWHelper *obj, gchar *version);

gchar *fw_helper_getShort(FWHelper *obj);
void fw_helper_setShort(FWHelper *obj, gchar *short_description);

gchar *fw_helper_getDescription(FWHelper *obj);
void fw_helper_setDescription(FWHelper *obj, gchar *description);

gchar *fw_helper_getFamily(FWHelper *obj);
void fw_helper_setFamily(FWHelper *obj, gchar *family);

gchar *fw_helper_getModule(FWHelper *obj);
void fw_helper_setModule(FWHelper *obj, gchar *module);

FWPortList *fw_helper_getPorts(FWHelper *obj);
void fw_helper_setPorts(FWHelper *obj, FWPortList *ports);

void fw_helper_addPort(FWHelper *obj, gchar *port, gchar *protocol);
void fw_helper_removePort(FWHelper *obj, gchar *port, gchar *protocol);
gboolean fw_helper_queryPort(FWHelper *obj, gchar *port, gchar *protocol);

void fw_helper_add_port(FWHelper *obj, FWPort *port);
void fw_helper_remove_port(FWHelper *obj, FWPort *port);
gboolean fw_helper_query_port(FWHelper *obj, FWPort *port);

#endif /* __FW_HELPER_H__ */
