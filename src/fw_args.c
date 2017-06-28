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

#include "fw_args.h"
#include "fw_functions.h"

G_DEFINE_TYPE(FWArgs, fw_args, G_TYPE_OBJECT);

#define FW_ARGS_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_ARGS_TYPE, FWArgsPrivate))

typedef struct {
    GList *args;      /* list of strings */
} FWArgsPrivate;

FWArgs *
fw_args_new()
{
    return g_object_new(FW_ARGS_TYPE, NULL);
}

static void
fw_args_init(FWArgs *obj)
{
    FWArgsPrivate *priv = FW_ARGS_GET_PRIVATE(obj);

    /* init vars */
    priv->args = NULL;
}

static void
fw_args_finalize(GObject *obj)
{
    FWArgsPrivate *priv = FW_ARGS_GET_PRIVATE(obj);

    if (priv->args != NULL)
	fw_str_list_free(priv->args);

    G_OBJECT_CLASS(fw_args_parent_class)->finalize(obj);
}

static void
fw_args_class_init(FWArgsClass *fw_args_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_args_class);

    obj_class->finalize = fw_args_finalize;

    g_type_class_add_private(obj_class, sizeof(FWArgsPrivate));
}

/* methods */

/**
 * fw_args_getArgs:
 * @obj: (type FWArgs*): a FWArgs instance
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_args_getArgs(FWArgs *obj)
{
    FWArgsPrivate *priv = FW_ARGS_GET_PRIVATE(obj);

    return priv->args;
}

/**
 * fw_args_setArgs:
 * @obj: (type FWArgs*): a FWArgs instance
 * @args: (type GList*) (element-type gchar*): list of strings
 */
void
fw_args_setArgs(FWArgs *obj, GList *args)
{
    FWArgsPrivate *priv = FW_ARGS_GET_PRIVATE(obj);

    if (priv->args != NULL)
	fw_str_list_free(priv->args);
    priv->args = fw_str_list_copy(args);
}

/**
 * fw_args_addArg:
 * @obj: (type FWArgs*): a FWArgs instance
 * @arg: (type gchar*): strings arg
 */
void
fw_args_addArg(FWArgs *obj, gchar *arg)
{
    FWArgsPrivate *priv = FW_ARGS_GET_PRIVATE(obj);

    priv->args = fw_str_list_append(priv->args, arg);
}
