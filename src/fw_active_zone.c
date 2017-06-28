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

#include "fw_active_zone.h"

G_DEFINE_TYPE(FWActiveZone, fw_active_zone, G_TYPE_OBJECT);

#define FW_ACTIVE_ZONE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_ACTIVE_ZONE_TYPE, FWActiveZonePrivate))

typedef struct {
    GList *interfaces;    /* list of string */
    GList *sources;       /* list of string */
} FWActiveZonePrivate;

FWActiveZone *
fw_active_zone_new()
{
    return g_object_new(FW_ACTIVE_ZONE_TYPE, NULL);
}

static void
fw_active_zone_init(FWActiveZone *obj)
{
    FWActiveZonePrivate *az = FW_ACTIVE_ZONE_GET_PRIVATE(obj);

    /* init vars */
    az->interfaces = NULL;
    az->sources = NULL;
}

static void
fw_active_zone_finalize(GObject *obj)
{
    FWActiveZonePrivate *az = FW_ACTIVE_ZONE_GET_PRIVATE(obj);

    if (az->interfaces != NULL) {
	g_list_foreach(az->interfaces, (GFunc) g_free, NULL);
    }
    if (az->sources != NULL) {
	g_list_foreach(az->sources, (GFunc) g_free, NULL);
    }

    G_OBJECT_CLASS(fw_active_zone_parent_class)->finalize(obj);
}

static void
fw_active_zone_class_init(FWActiveZoneClass *fw_active_zone_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_active_zone_class);

    obj_class->finalize = fw_active_zone_finalize;

    g_type_class_add_private(obj_class, sizeof(FWActiveZonePrivate));
}

/* methods */

/**
 * fw_active_zone_getInterfaces:
 * @obj: (type FWActiveZone*): a FWActiveZone instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_active_zone_getInterfaces(FWActiveZone *obj)
{
    FWActiveZonePrivate *az = FW_ACTIVE_ZONE_GET_PRIVATE(obj);

    return az->interfaces;
}

/**
 * fw_active_zone_getSources:
 * @obj: (type FWActiveZone*): a FWActiveZone instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_active_zone_getSources(FWActiveZone *obj)
{
    FWActiveZonePrivate *az = FW_ACTIVE_ZONE_GET_PRIVATE(obj);

    return az->sources;
}

/**
 * fw_active_zone_setInterfaces:
 * @obj: (type FWActiveZone*): a FWActiveZone instance
 * @list: (type GList*) (element-type gchar*)
 */
void
fw_active_zone_setInterfaces(FWActiveZone *obj, GList *list)
{
    FWActiveZonePrivate *az = FW_ACTIVE_ZONE_GET_PRIVATE(obj);

    az->interfaces = list;
}

/**
 * fw_active_zone_setSources:
 * @obj: (type FWActiveZone*): a FWActiveZone instance
 * @list: (type GList*) (element-type gchar*)
 */
void
fw_active_zone_setSources(FWActiveZone *obj, GList *list)
{
    FWActiveZonePrivate *az = FW_ACTIVE_ZONE_GET_PRIVATE(obj);

    az->sources = list;
}
