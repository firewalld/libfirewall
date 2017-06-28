/*
 * Copyright (C) 2014 Red Hat, Inc.
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

#ifndef __FW_ACTIVE_ZONE_H__
#define __FW_ACTIVE_ZONE_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"

#define FW_ACTIVE_ZONE_TYPE            (fw_active_zone_get_type())
#define FW_ACTIVE_ZONE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_ACTIVE_ZONE_TYPE, FWActiveZone))
#define FW_ACTIVE_ZONE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_ACTIVE_ZONE_TYPE, FWActiveZoneClass))
#define FW_IS_ACTIVE_ZONE(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_ACTIVE_ZONE_TYPE, FWActiveZoneClass))
#define FW_ACTIVE_ZONE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_ACTIVE_ZONE_TYPE, FWActiveZoneClass))

typedef struct {
    GObject parent;
} FWActiveZone;

typedef struct {
    GObjectClass parent;
} FWActiveZoneClass;

GType fw_active_zone_get_type(void);
FWActiveZone *fw_active_zone_new(void);

GList *fw_active_zone_getInterfaces(FWActiveZone *obj);
GList *fw_active_zone_getSources(FWActiveZone *obj);

void fw_active_zone_setInterfaces(FWActiveZone *obj, GList *list);
void fw_active_zone_setSources(FWActiveZone *obj, GList *list);

#endif /* __FW_ACTIVE_ZONE_H__ */
