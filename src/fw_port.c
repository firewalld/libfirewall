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

#include "fw_port.h"

G_DEFINE_TYPE(FWPort, fw_port, G_TYPE_OBJECT);

#define FW_PORT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_PORT_TYPE, FWPortPrivate))

typedef struct {
    gchar *port;      /* string */
    gchar *protocol;  /* string */
} FWPortPrivate;

FWPort *
fw_port_new_simple()
{
    return g_object_new(FW_PORT_TYPE, NULL);
}

FWPort *
fw_port_new_from_variant(GVariant *variant)
{
    FWPort *obj = fw_port_new_simple();
    GVariant *item=NULL;
    gchar *port, *protocol;

    item = g_variant_get_child_value(variant, 0);
    g_variant_get(item, "s", &port);
    fw_port_setPort(obj, port);
    g_variant_unref(item);

    item = g_variant_get_child_value(variant, 1);
    g_variant_get(item, "s", &protocol);
    fw_port_setProtocol(obj, protocol);
    g_variant_unref(item);

    return obj;
}

FWPort *
fw_port_new(gchar *port,
	    gchar *protocol)
{
    FWPort *obj = fw_port_new_simple();

    fw_port_setPort(obj, port);
    fw_port_setProtocol(obj, protocol);

    return obj;
}

GVariant *
fw_port_to_variant(FWPort *obj)
{
    FWPortPrivate *priv = FW_PORT_GET_PRIVATE(obj);

    return g_variant_new("a(ss)", priv->port, priv->protocol);
}

static void
fw_port_init(FWPort *obj)
{
    FWPortPrivate *priv = FW_PORT_GET_PRIVATE(obj);

    /* init vars */
    priv->port = g_strdup("");
    priv->protocol = g_strdup("");
}

static void
fw_port_finalize(GObject *obj)
{
    FWPortPrivate *priv = FW_PORT_GET_PRIVATE(obj);

    if (priv->port != NULL)
	g_free(priv->port);
    if (priv->protocol != NULL)
	g_free(priv->protocol);

    G_OBJECT_CLASS(fw_port_parent_class)->finalize(obj);
}

static void
fw_port_class_init(FWPortClass *fw_port_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_port_class);

    obj_class->finalize = fw_port_finalize;

    g_type_class_add_private(obj_class, sizeof(FWPortPrivate));
}

/* methods */

/**
 * fw_port_getPort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_port_getPort(FWPort *obj)
{
    FWPortPrivate *priv = FW_PORT_GET_PRIVATE(obj);

    return priv->port;
}

void
fw_port_setPort(FWPort *obj, gchar *port)
{
    FWPortPrivate *priv = FW_PORT_GET_PRIVATE(obj);

    if (priv->port != NULL)
	g_free(priv->port);
    priv->port = g_strdup(port);
}

/**
 * fw_port_getProtocol:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_port_getProtocol(FWPort *obj)
{
    FWPortPrivate *priv = FW_PORT_GET_PRIVATE(obj);

    return priv->protocol;
}

void
fw_port_setProtocol(FWPort *obj, gchar *protocol)
{
    FWPortPrivate *priv = FW_PORT_GET_PRIVATE(obj);

    if (priv->protocol != NULL)
	g_free(priv->protocol);
    priv->protocol = g_strdup(protocol);
}

/**
 * fw_port_getStr:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_port_getStr(FWPort *obj)
{
    FWPortPrivate *priv = FW_PORT_GET_PRIVATE(obj);

    return g_strdup_printf("%s/%s", priv->port, priv->protocol);
}
