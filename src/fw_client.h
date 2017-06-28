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

#ifndef __FW_CLIENT_H__
#define __FW_CLIENT_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include "firewall.h"
#include "fw_zone.h"
#include "fw_service.h"
#include "fw_icmptype.h"
#include "fw_ipset.h"
#include "fw_helper.h"
#include "fw_config.h"

#define FW_CLIENT_TYPE           (fw_client_get_type())
#define FW_CLIENT(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_CLIENT_TYPE, FWClient))
#define FW_CLIENT_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass), FW_CLIENT_TYPE, FWClientClass))
#define FW_IS_CLIENT(klass)      (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_CLIENT_TYPE, FWClientClass))
#define FW_CLIENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), FW_CLIENT_TYPE, FWClientClass))

typedef struct {
    GObject parent;
} FWClient;

typedef struct {
    GObjectClass parent;
} FWClientClass;


GType fw_client_get_type(void);
FWClient *fw_client_new(void);

/* authorize all */

void fw_client_authorizeAll(FWClient *obj);

/* config */

FWConfig *fw_client_config(FWClient *obj);

/* reload */

void fw_client_reload(FWClient *obj);
void fw_client_completeReload(FWClient *obj);

/* runtime to permanent */

void fw_client_runtimeToPermanent(FWClient *obj);

/* properties */

/*
void *fw_client_get_property(FWClient *obj, gchar *prop);
void *fw_client_get_properties(FWClient *obj);
void *fw_client_set_property(FWClient *obj, gchar *prop, void *value);
*/

/* panic mode */

void fw_client_enablePanicMode(FWClient *obj);
void fw_client_disablePanicMode(FWClient *obj);
gboolean fw_client_queryPanicMode(FWClient *obj);

/* list functions */

FWZone *fw_client_getZoneSettings(FWClient *obj, const gchar *zone);
GList *fw_client_listServices(FWClient *obj);
FWService *fw_client_getServiceSettings(FWClient *obj, const gchar *service);
GList *fw_client_listIPSets(FWClient *obj);
FWIPSet *fw_client_getIPSetSettings(FWClient *obj, const gchar *ipset);
GList *fw_client_listIcmpTypes(FWClient *obj);
FWIcmpType *fw_client_getIcmpTypeSettings(FWClient *obj, const gchar *icmptype);
GList *fw_client_listHelpers(FWClient *obj);
FWHelper *fw_client_getHelperSettings(FWClient *obj, const gchar *helper);

/* default zone */

const gchar* fw_client_getDefaultZone(FWClient *obj);
void fw_client_setDefaultZone(FWClient *obj, const gchar *zone);

/* zone */

GList *fw_client_getZones(FWClient *obj);
GHashTable *fw_client_getActiveZones(FWClient *obj);
const gchar *fw_client_getZoneOfInterface(FWClient *obj, const gchar *interface);
const gchar *fw_client_getZoneOfSource(FWClient *obj, const gchar *source);
/* isImmutableis is deprecated since some time and is always returning FALSE */

/* interfaces */

const gchar *fw_client_addInterface(FWClient *obj, const gchar *zone, const gchar *interface);
/* changeZone is deprecated and should not be used anymore */
const gchar *fw_client_changeZoneOfInterface(FWClient *obj, const gchar *zone, const gchar *interface);
GList *fw_client_getInterfaces(FWClient *obj, const gchar *zone);
gboolean fw_client_queryInterface(FWClient *obj, const gchar *zone, const gchar *interface);
const gchar *fw_client_removeInterface(FWClient *obj, const gchar *zone, const gchar *interface);

/* sources */

const gchar *fw_client_addSource(FWClient *obj, const gchar *zone, const gchar *source);
const gchar *fw_client_changeZoneOfSource(FWClient *obj, const gchar *zone, const gchar *source);
GList *fw_client_getSources(FWClient *obj, const gchar *zone);
gboolean fw_client_querySource(FWClient *obj, const gchar *zone, const gchar *source);
const gchar *fw_client_removeSource(FWClient *obj, const gchar *zone, const gchar *source);

/* rich rules */

const gchar *fw_client_addRichRule(FWClient *obj, const gchar *zone, const gchar *rule, gint32 timeout);
GList *fw_client_getRichRules(FWClient *obj, const gchar *zone);
gboolean fw_client_queryRichRule(FWClient *obj, const gchar *zone, const gchar *rule);
const gchar *fw_client_removeRichRule(FWClient *obj, const gchar *zone, const gchar *rule);

/* services */

const gchar *fw_client_addService(FWClient *obj, const gchar *zone, const gchar *service, gint32 timeout);
GList *fw_client_getServices(FWClient *obj, const gchar *zone);
gboolean fw_client_queryService(FWClient *obj, const gchar *zone, const gchar *service);
const gchar *fw_client_removeService(FWClient *obj, const gchar *zone, const gchar *service);

/* ports */

const gchar *fw_client_addPort(FWClient *obj, const gchar *zone, const gchar *port, const gchar *protocol, gint32 timeout);
GList *fw_client_getPorts(FWClient *obj, const gchar *zone);
gboolean fw_client_queryPort(FWClient *obj, const gchar *zone, const gchar *port, const gchar *protocol);
const gchar *fw_client_removePort(FWClient *obj, const gchar *zone, const gchar *port, const gchar *protocol);

/* protocols */

const gchar *fw_client_addProtocol(FWClient *obj, const gchar *zone, const gchar *protocol, gint32 timeout);
GList *fw_client_getProtocols(FWClient *obj, const gchar *zone);
gboolean fw_client_queryProtocol(FWClient *obj, const gchar *zone, const gchar *protocol);
const gchar *fw_client_removeProtocol(FWClient *obj, const gchar *zone, const gchar *protocol);

/* source-ports */

const gchar *fw_client_addSourcePort(FWClient *obj, const gchar *zone, const gchar *port, const gchar *protocol, gint32 timeout);
GList *fw_client_getSourcePorts(FWClient *obj, const gchar *zone);
gboolean fw_client_querySourcePort(FWClient *obj, const gchar *zone, const gchar *port, const gchar *protocol);
const gchar *fw_client_removeSourcePort(FWClient *obj, const gchar *zone, const gchar *port, const gchar *protocol);

/* masquerade */

const gchar *fw_client_addMasquerade(FWClient *obj, const gchar *zone, gint32 timeout);
gboolean fw_client_queryMasquerade(FWClient *obj, const gchar *zone);
const gchar *fw_client_removeMasquerade(FWClient *obj, const gchar *zone);

/* foward ports */

const gchar *fw_client_addForwardPort(FWClient *obj, const gchar *zone, const gchar *port, const gchar *protocol, const gchar *toport, const gchar *toaddr, gint32 timeout);
GList *fw_client_getForwardPorts(FWClient *obj, const gchar *zone);
gboolean fw_client_queryForwardPort(FWClient *obj, const gchar *zone, const gchar *port, const gchar *protocol, const gchar *toport, const gchar *toaddr);
const gchar *fw_client_removeForwardPort(FWClient *obj, const gchar *zone, const gchar *port, const gchar *protocol, const gchar *toport, const gchar *toaddr);

/* icmpblock */

const gchar *fw_client_addIcmpBlock(FWClient *obj,  const gchar *zone,  const gchar *icmptype, gint32 timeout);
GList *fw_client_getIcmpBlocks(FWClient *obj,  const gchar *zone);
gboolean fw_client_queryIcmpBlock(FWClient *obj,  const gchar *zone,  const gchar *icmptype);
const gchar *fw_client_removeIcmpBlock(FWClient *obj,  const gchar *zone,  const gchar *icmptype);

/* direct chain */

void fw_client_addChain(FWClient *obj, const gchar *ipv, const gchar *table, const gchar *chain);
GList *fw_client_getChains(FWClient *obj, const gchar *ipv, const gchar *table);
gboolean fw_client_queryChain(FWClient *obj, const gchar *ipv, const gchar *table, const gchar *chain);
void fw_client_removeChain(FWClient *obj, const gchar *ipv, const gchar *table, const gchar *chain);
GList *fw_client_getAllChains(FWClient *obj);

/* direct rule */

void fw_client_addRule(FWClient *obj, const gchar *ipv, const gchar *table, const gchar *chain, gint32 priority, const GList *args);

GList *fw_client_getRules(FWClient *obj, const gchar *ipv, const gchar *table, const char *chain);

gboolean fw_client_queryRule(FWClient *obj, const gchar *ipv, const gchar *table, const gchar *chain, gint32 priority, const GList *args);

void fw_client_removeRule(FWClient *obj, const gchar *ipv, const gchar *table, const gchar *chain, gint32 priority, const GList *args);

void fw_client_removeRules(FWClient *obj, const gchar *ipv, const gchar *table, const gchar *chain);

GList *fw_client_getAllRules(FWClient *obj);

/* direct passthrough (untracked) */

const gchar *fw_client_passthrough(FWClient *obj, const gchar *ipv, const GList *args);

/* direct passthrough (tracked) */

void fw_client_addPassthrough(FWClient *obj, const gchar *ipv, const GList *args);
void fw_client_removePassthrough(FWClient *obj, const gchar *ipv, const GList *args);
gboolean fw_client_queryPassthrough(FWClient *obj, const gchar *ipv, const GList *args);
GList *fw_client_getPassthroughs(FWClient *obj, const gchar *ipv);
GList *fw_client_getAllPassthroughs(FWClient *obj);
void fw_client_removeAllPassthroughs(FWClient *obj);

/* lockdown */

void fw_client_enableLockdown(FWClient *obj);
void fw_client_disableLockdown(FWClient *obj);
gboolean fw_client_queryLockdown(FWClient *obj);

/* policies */

/* lockdown whitelist commands */

void fw_client_addLockdownWhitelistCommand(FWClient *obj, const gchar *command);
GList *fw_client_getLockdownWhitelistCommands(FWClient *obj);
gboolean fw_client_queryLockdownWhitelistCommand(FWClient *obj, const gchar *command);
void fw_client_removeLockdownWhitelistCommand(FWClient *obj, const gchar *command);

/* lockdown whitelist contexts */

void fw_client_addLockdownWhitelistContext(FWClient *obj, const gchar *context);
GList *fw_client_getLockdownWhitelistContexts(FWClient *obj);
gboolean fw_client_queryLockdownWhitelistContext(FWClient *obj, const gchar *context);
void fw_client_removeLockdownWhitelistContext(FWClient *obj, const gchar *context);

/* lockdown whitelist uids */

void fw_client_addLockdownWhitelistUid(FWClient *obj, gint32  uid);
GList *fw_client_getLockdownWhitelistUids(FWClient *obj);
gboolean fw_client_queryLockdownWhitelistUid(FWClient *obj, gint32 uid);
void fw_client_removeLockdownWhitelistUid(FWClient *obj, gint32 uid);

/* lockdown whitelist users */

void fw_client_addLockdownWhitelistUser(FWClient *obj, const gchar *user);
GList *fw_client_getLockdownWhitelistUsers(FWClient *obj);
gboolean fw_client_queryLockdownWhitelistUser(FWClient *obj, const gchar *user);
void fw_client_removeLockdownWhitelistUser(FWClient *obj, const gchar *user);

#endif /* __FW_CLIENT_H__ */
