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

#ifndef __FW_DIRECT_SIMPLE_RULE_H__
#define __FW_DIRECT_SIMPLE_RULE_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"

#define FW_DIRECT_SIMPLE_RULE_TYPE            (fw_direct_simple_rule_get_type())
#define FW_DIRECT_SIMPLE_RULE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_DIRECT_SIMPLE_RULE_TYPE, FWDirectSimpleRule))
#define FW_DIRECT_SIMPLE_RULE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_DIRECT_SIMPLE_RULE_TYPE, FWDirectSimpleRuleClass))
#define FW_IS_SIMPLERULE(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_DIRECT_SIMPLE_RULE_TYPE, FWDirectSimpleRuleClass))
#define FW_DIRECT_SIMPLE_RULE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_DIRECT_SIMPLE_RULE_TYPE, FWDirectSimpleRuleClass))

typedef struct {
    GObject parent;
} FWDirectSimpleRule;

typedef struct {
    GObjectClass parent;
} FWDirectSimpleRuleClass;

GType fw_direct_simple_rule_get_type(void);
FWDirectSimpleRule *fw_direct_simple_rule_new(void);

gint32 fw_direct_simple_rule_getPriority(FWDirectSimpleRule *obj);
GList *fw_direct_simple_rule_getArgs(FWDirectSimpleRule *obj);

void fw_direct_simple_rule_setPriority(FWDirectSimpleRule *obj, gint32 priority);
void fw_direct_simple_rule_setArgs(FWDirectSimpleRule *obj, GList *args);

#endif /* __FW_DIRECT_SIMPLE_RULE_H__ */
