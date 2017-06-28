/*
 * Copyright (C) 2014-2017 Red Hat, Inc.
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

#ifndef __FW_CONFIG_H__
#define __FW_CONFIG_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include "firewall.h"
#include "fw_zone.h"
#include "fw_service.h"
#include "fw_icmptype.h"
#include "fw_ipset.h"
#include "fw_helper.h"
#include "fw_config_helper.h"
#include "fw_config_icmptype.h"
#include "fw_config_ipset.h"
#include "fw_config_service.h"
#include "fw_config_zone.h"

#define FW_CONFIG_TYPE           (fw_config_get_type())
#define FW_CONFIG(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_CONFIG_TYPE, FWConfig))
#define FW_CONFIG_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass), FW_CONFIG_TYPE, FWConfigClass))
#define FW_IS_CONFIG(klass)      (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_CONFIG_TYPE, FWConfigClass))
#define FW_CONFIG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), FW_CONFIG_TYPE, FWConfigClass))

typedef struct {
    GObject parent;
} FWConfig;

typedef struct {
    GObjectClass parent;
} FWConfigClass;


GType fw_config_get_type(void);
FWConfig *fw_config_new(void);

/* config */

/* properties */

/*
void *fw_config_get_property(FWConfig *obj, gchar *prop);
void *fw_config_get_properties(FWConfig *obj);
void *fw_config_set_property(FWConfig *obj, gchar *prop, void *value);
*/

/*
addHelper(s: helper, (sssssa(ss)): settings) → o
addIPSet(s: ipset, (ssssa{ss}as): settings) → o
addIcmpType(s: icmptype, (sssas): settings) → o
addService(s: service, (sssa(ss)asa{ss}asa(ss)): settings) → o
addZone(s: zone, (sssbsasa(ss)asba(ssss)asasasasa(ss)): settings) → o

getHelperByName(s: helper) → o
getHelperNames() → as
getIPSetByName(s: ipset) → o
getIPSetNames() → as
getIcmpTypeByName(s: icmptype) → o
getIcmpTypeNames() → as
getServiceByName(s: service) → o
getServiceNames() → as
getZoneByName(s: zone) → o
getZoneNames() → as

getZoneOfInterface(s: iface) → s
getZoneOfSource(s: source) → s

listHelpers() → ao
listIPSets() → ao
listIcmpTypes() → ao
listServices() → ao
listZones() → ao
*/

const gchar *fw_config_addHelper(FWConfig *obj, const gchar *helper, FWHelper *settings);
FWConfigHelper *fw_config_getHelperByName(FWConfig *obj, const gchar *helper);
GList *fw_config_getHelperNames(FWConfig *obj);
GList *fw_config_listHelpers(FWConfig *obj);

const gchar *fw_config_addIcmpType(FWConfig *obj, const gchar *icmptype, FWIcmpType *settings);
FWConfigIcmpType *fw_config_getIcmpTypeByName(FWConfig *obj, const gchar *icmptype);
GList *fw_config_getIcmpTypeNames(FWConfig *obj);
GList *fw_config_listIcmpTypes(FWConfig *obj);

const gchar *fw_config_addIPSet(FWConfig *obj, const gchar *ipset, FWIPSet *settings);
FWConfigIPSet *fw_config_getIPSetByName(FWConfig *obj, const gchar *ipset);
GList *fw_config_getIPSetNames(FWConfig *obj);
GList *fw_config_listIPSets(FWConfig *obj);

const gchar *fw_config_addService(FWConfig *obj, const gchar *service, FWService *settings);
FWConfigService *fw_config_getServiceByName(FWConfig *obj, const gchar *service);
GList *fw_config_getServiceNames(FWConfig *obj);
GList *fw_config_listServices(FWConfig *obj);

const gchar *fw_config_addZone(FWConfig *obj, const gchar *zone, FWZone *settings);
FWConfigZone *fw_config_getZoneByName(FWConfig *obj, const gchar *zone);
GList *fw_config_getZoneNames(FWConfig *obj);
GList *fw_config_listZones(FWConfig *obj);

#endif /* __FW_CONFIG_H__ */
