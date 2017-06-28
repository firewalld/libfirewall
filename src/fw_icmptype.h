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

#ifndef __FW_ICMPTYPE_H__
#define __FW_ICMPTYPE_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"
#include "fw_functions.h"
#include "fw_port.h"

#define FW_ICMPTYPE_TYPE            (fw_icmptype_get_type())
#define FW_ICMPTYPE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_ICMPTYPE_TYPE, FWIcmpType))
#define FW_ICMPTYPE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_ICMPTYPE_TYPE, FWIcmpTypeClass))
#define FW_IS_ICMPTYPE(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_ICMPTYPE_TYPE, FWIcmpTypeClass))
#define FW_ICMPTYPE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_ICMPTYPE_TYPE, FWIcmpTypeClass))

typedef struct {
    GObject parent;
} FWIcmpType;

typedef struct {
    GObjectClass parent;
} FWIcmpTypeClass;

GType fw_icmptype_get_type(void);
FWIcmpType *fw_icmptype_new(void);
FWIcmpType *fw_icmptype_new_from_variant(GVariant *variant);
GVariant *fw_icmptype_to_variant(FWIcmpType *obj);

void fw_icmptype_print_str(FWIcmpType *obj);

gchar *fw_icmptype_getVersion(FWIcmpType *obj);
void fw_icmptype_setVersion(FWIcmpType *obj, gchar *version);

gchar *fw_icmptype_getShort(FWIcmpType *obj);
void fw_icmptype_setShort(FWIcmpType *obj, gchar *short_description);

gchar *fw_icmptype_getDescription(FWIcmpType *obj);
void fw_icmptype_setDescription(FWIcmpType *obj, gchar *description);

GList *fw_icmptype_getDestinations(FWIcmpType *obj);
void fw_icmptype_setDestinations(FWIcmpType *obj, GList *destinations);
void fw_icmptype_addDestination(FWIcmpType *obj, gchar *destination);
void fw_icmptype_removeDestination(FWIcmpType *obj, gchar *destination);
gboolean fw_icmptype_queryDestination(FWIcmpType *obj, gchar *destination);

#endif /* __FW_ICMPTYPE_H__ */
