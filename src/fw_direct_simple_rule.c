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

#include "fw_direct_simple_rule.h"
#include "fw_functions.h"

G_DEFINE_TYPE(FWDirectSimpleRule, fw_direct_simple_rule, G_TYPE_OBJECT);

#define FW_DIRECT_SIMPLE_RULE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_DIRECT_SIMPLE_RULE_TYPE, FWDirectSimpleRulePrivate))

typedef struct {
    gint32 priority;  /* int32 */
    GList *args;      /* list of strings */
} FWDirectSimpleRulePrivate;

FWDirectSimpleRule *
fw_direct_simple_rule_new()
{
    return g_object_new(FW_DIRECT_SIMPLE_RULE_TYPE, NULL);
}

static void
fw_direct_simple_rule_init(FWDirectSimpleRule *obj)
{
    FWDirectSimpleRulePrivate *rule = FW_DIRECT_SIMPLE_RULE_GET_PRIVATE(obj);

    /* init vars */
    rule->priority = 0;
    rule->args = NULL;
}

static void
fw_direct_simple_rule_finalize(GObject *obj)
{
    FWDirectSimpleRulePrivate *rule = FW_DIRECT_SIMPLE_RULE_GET_PRIVATE(obj);

    if (rule->args != NULL)
	fw_str_list_free(rule->args);

    G_OBJECT_CLASS(fw_direct_simple_rule_parent_class)->finalize(obj);
}

static void
fw_direct_simple_rule_class_init(FWDirectSimpleRuleClass *fw_direct_simple_rule_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_direct_simple_rule_class);

    obj_class->finalize = fw_direct_simple_rule_finalize;

    g_type_class_add_private(obj_class, sizeof(FWDirectSimpleRulePrivate));
}

/* methods */

/**
 * fw_direct_simple_rule_getPriority:
 * @obj: (type FWDirectSimpleRule*): a FWDirectSimpleRule instance
 *
 * Returns: (type gint32)
 */
gint32
fw_direct_simple_rule_getPriority(FWDirectSimpleRule *obj)
{
    FWDirectSimpleRulePrivate *rule = FW_DIRECT_SIMPLE_RULE_GET_PRIVATE(obj);

    return rule->priority;
}

/**
 * fw_direct_simple_rule_getArgs:
 * @obj: (type FWDirectSimpleRule*): a FWDirectSimpleRule instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_direct_simple_rule_getArgs(FWDirectSimpleRule *obj)
{
    FWDirectSimpleRulePrivate *rule = FW_DIRECT_SIMPLE_RULE_GET_PRIVATE(obj);

    return rule->args;
}

/**
 * fw_direct_simple_rule_setPriority:
 * @obj: (type FWDirectSimpleRule*): a FWDirectSimpleRule instance
 * @priority: (type gint32)
 */
void
fw_direct_simple_rule_setPriority(FWDirectSimpleRule *obj, gint32 priority)
{
    FWDirectSimpleRulePrivate *rule = FW_DIRECT_SIMPLE_RULE_GET_PRIVATE(obj);

    rule->priority = priority;
}

/**
 * fw_direct_simple_rule_setArgs:
 * @obj: (type FWDirectSimpleRule*): a FWDirectSimpleRule instance
 * @args: (type GList*) (element-type gchar*)
 */
void
fw_direct_simple_rule_setArgs(FWDirectSimpleRule *obj, GList *args)
{
    FWDirectSimpleRulePrivate *rule = FW_DIRECT_SIMPLE_RULE_GET_PRIVATE(obj);

    if (rule->args != NULL)
	fw_str_list_free(rule->args);
    rule->args = fw_str_list_copy(args);
}
