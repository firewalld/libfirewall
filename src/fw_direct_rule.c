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

#include "fw_direct_rule.h"
#include "fw_functions.h"

G_DEFINE_TYPE(FWDirectRule, fw_direct_rule, G_TYPE_OBJECT);

#define FW_DIRECT_RULE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_DIRECT_RULE_TYPE, FWDirectRulePrivate))

typedef struct {
    gchar *ipv;       /* string */
    gchar *table;     /* string */
    gchar *chain;     /* string */
    gint32 priority;  /* int32 */
    GList *args;      /* list of strings */
} FWDirectRulePrivate;

FWDirectRule *
fw_direct_rule_new()
{
    return g_object_new(FW_DIRECT_RULE_TYPE, NULL);
}

static void
fw_direct_rule_init(FWDirectRule *obj)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    /* init vars */
    rule->ipv = g_strdup("");
    rule->table = g_strdup("");
    rule->chain = g_strdup("");
    rule->priority = 0;
    rule->args = NULL;
}

static void
fw_direct_rule_finalize(GObject *obj)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    if (rule->ipv != NULL)
	g_free(rule->ipv);
    if (rule->table != NULL)
	g_free(rule->table);
    if (rule->chain != NULL)
	g_free(rule->chain);
    if (rule->args != NULL)
	fw_str_list_free(rule->args);

    G_OBJECT_CLASS(fw_direct_rule_parent_class)->finalize(obj);
}

static void
fw_direct_rule_class_init(FWDirectRuleClass *fw_direct_rule_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_direct_rule_class);

    obj_class->finalize = fw_direct_rule_finalize;

    g_type_class_add_private(obj_class, sizeof(FWDirectRulePrivate));
}

/* methods */

/**
 * fw_direct_rule_getIpv:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 *
 * Returns: (type gchar*)
 */
gchar *
fw_direct_rule_getIpv(FWDirectRule *obj)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    return rule->ipv;
}

/**
 * fw_direct_rule_getTable:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 *
 * Returns: (type gchar*)
 */
gchar *
fw_direct_rule_getTable(FWDirectRule *obj)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    return rule->table;
}

/**
 * fw_direct_rule_getChain:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 *
 * Returns: (type gchar*)
 */
gchar *
fw_direct_rule_getChain(FWDirectRule *obj)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    return rule->chain;
}

/**
 * fw_direct_rule_getPriority:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 *
 * Returns: (type gint32)
 */
gint32
fw_direct_rule_getPriority(FWDirectRule *obj)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    return rule->priority;
}

/**
 * fw_direct_rule_getArgs:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_direct_rule_getArgs(FWDirectRule *obj)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    return rule->args;
}

/**
 * fw_direct_rule_setIpv:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 * @ipv: (type gchar*)
 */
void
fw_direct_rule_setIpv(FWDirectRule *obj, gchar *ipv)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    if (rule->ipv != NULL)
	g_free(rule->ipv);
    rule->ipv = g_strdup(ipv);
}

/**
 * fw_direct_rule_setTable:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 * @table: (type gchar*)
 */
void
fw_direct_rule_setTable(FWDirectRule *obj, gchar *table)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    if (rule->table != NULL)
	g_free(rule->table);
    rule->table = g_strdup(table);
}

/**
 * fw_direct_rule_setChain:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 * @chain: (type gchar*)
 */
void
fw_direct_rule_setChain(FWDirectRule *obj, gchar *chain)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    if (rule->chain != NULL)
	g_free(rule->chain);
    rule->chain = g_strdup(chain);
}

/**
 * fw_direct_rule_setPriority:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 * @priority: (type gint32)
 */
void
fw_direct_rule_setPriority(FWDirectRule *obj, gint32 priority)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    rule->priority = priority;
}

/**
 * fw_direct_rule_setArgs:
 * @obj: (type FWDirectRule*): a FWDirectRule instance
 * @args: (type GList*) (element-type gchar*)
 */
void
fw_direct_rule_setArgs(FWDirectRule *obj, GList *args)
{
    FWDirectRulePrivate *rule = FW_DIRECT_RULE_GET_PRIVATE(obj);

    if (rule->args != NULL)
	fw_str_list_free(rule->args);
    rule->args = fw_str_list_copy(args);
}
