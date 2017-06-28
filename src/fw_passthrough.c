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

#include "fw_passthrough.h"
#include "fw_functions.h"

G_DEFINE_TYPE(FWPassthrough, fw_passthrough, G_TYPE_OBJECT);

#define FW_PASSTHROUGH_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_PASSTHROUGH_TYPE, FWPassthroughPrivate))

typedef struct {
    gchar *ipv;       /* string */
    GList *args;      /* list of strings */
} FWPassthroughPrivate;

FWPassthrough *
fw_passthrough_new()
{
    return g_object_new(FW_PASSTHROUGH_TYPE, NULL);
}

static void
fw_passthrough_init(FWPassthrough *obj)
{
    FWPassthroughPrivate *priv = FW_PASSTHROUGH_GET_PRIVATE(obj);

    /* init vars */
    priv->ipv = g_strdup("");
    priv->args = NULL;
}

static void
fw_passthrough_finalize(GObject *obj)
{
    FWPassthroughPrivate *priv = FW_PASSTHROUGH_GET_PRIVATE(obj);

    if (priv->ipv != NULL)
	g_free(priv->ipv);
    if (priv->args != NULL)
	fw_str_list_free(priv->args);

    G_OBJECT_CLASS(fw_passthrough_parent_class)->finalize(obj);
}

static void
fw_passthrough_class_init(FWPassthroughClass *fw_passthrough_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_passthrough_class);

    obj_class->finalize = fw_passthrough_finalize;

    g_type_class_add_private(obj_class, sizeof(FWPassthroughPrivate));
}

/* methods */

/**
 * fw_passthrough_getIpv:
 * @obj: (type FWPassthrough*): a FWPassthrough instance
 *
 * Returns: (type gchar*)
 */
gchar *
fw_passthrough_getIpv(FWPassthrough *obj)
{
    FWPassthroughPrivate *priv = FW_PASSTHROUGH_GET_PRIVATE(obj);

    return priv->ipv;
}

/**
 * fw_passthrough_getArgs:
 * @obj: (type FWPassthrough*): a FWPassthrough instance
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_passthrough_getArgs(FWPassthrough *obj)
{
    FWPassthroughPrivate *priv = FW_PASSTHROUGH_GET_PRIVATE(obj);

    return priv->args;
}

/**
 * fw_passthrough_setIpv:
 * @obj: (type FWPassthrough*): a FWPassthrough instance
 * @ipv: (type gchar*)
 */
void
fw_passthrough_setIpv(FWPassthrough *obj, gchar *ipv)
{
    FWPassthroughPrivate *priv = FW_PASSTHROUGH_GET_PRIVATE(obj);

    if (priv->ipv != NULL)
	g_free(priv->ipv);
    priv->ipv = g_strdup(ipv);
}

/**
 * fw_passthrough_setArgs:
 * @obj: (type FWPassthrough*): a FWPassthrough instance
 * @args: (type GList*) (element-type gchar*)
 */
void
fw_passthrough_setArgs(FWPassthrough *obj, GList *args)
{
    FWPassthroughPrivate *priv = FW_PASSTHROUGH_GET_PRIVATE(obj);

    if (priv->args != NULL)
	fw_str_list_free(priv->args);
    priv->args = fw_str_list_copy(args);
}
