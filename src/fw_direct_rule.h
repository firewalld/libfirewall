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

#ifndef __FW_DIRECT_RULE_H__
#define __FW_DIRECT_RULE_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"

#define FW_DIRECT_RULE_TYPE            (fw_direct_rule_get_type())
#define FW_DIRECT_RULE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_DIRECT_RULE_TYPE, FWDirectRule))
#define FW_DIRECT_RULE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_DIRECT_RULE_TYPE, FWDirectRuleClass))
#define FW_IS_DIRECT_RULE(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_DIRECT_RULE_TYPE, FWDirectRuleClass))
#define FW_DIRECT_RULE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_DIRECT_RULE_TYPE, FWDirectRuleClass))

typedef struct {
    GObject parent;
} FWDirectRule;

typedef struct {
    GObjectClass parent;
} FWDirectRuleClass;

GType fw_direct_rule_get_type(void);
FWDirectRule *fw_direct_rule_new(void);

gchar *fw_direct_rule_getIpv(FWDirectRule *obj);
gchar *fw_direct_rule_getTable(FWDirectRule *obj);
gchar *fw_direct_rule_getChain(FWDirectRule *obj);
gint32 fw_direct_rule_getPriority(FWDirectRule *obj);
GList *fw_direct_rule_getArgs(FWDirectRule *obj);

void fw_direct_rule_setIpv(FWDirectRule *obj, char *ipv);
void fw_direct_rule_setTable(FWDirectRule *obj, char *table);
void fw_direct_rule_setChain(FWDirectRule *obj, char *chain);
void fw_direct_rule_setPriority(FWDirectRule *obj, gint32 priority);
void fw_direct_rule_setArgs(FWDirectRule *obj, GList *args);

#endif /* __FW_DIRECT_RULE_H__ */
