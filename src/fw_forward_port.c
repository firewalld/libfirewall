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

#include <string.h>
#include "fw_forward_port.h"

G_DEFINE_TYPE(FWForwardPort, fw_forward_port, G_TYPE_OBJECT);

#define FW_FORWARD_PORT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_FORWARD_PORT_TYPE, FWForwardPortPrivate))

typedef struct {
    gchar *port;      /* string */
    gchar *protocol;  /* string */
    gchar *toport;    /* string */
    gchar *toaddr;    /* string */
} FWForwardPortPrivate;

FWForwardPort *
fw_forward_port_new_simple()
{
    return g_object_new(FW_FORWARD_PORT_TYPE, NULL);
}

FWForwardPort *
fw_forward_port_new_from_variant(GVariant *variant)
{
    FWForwardPort *obj = fw_forward_port_new_simple();
    GVariant *item=NULL;
    const gchar *port, *protocol, *toport, *toaddr;

    item = g_variant_get_child_value(variant, 0);
    g_variant_get(item, "s", &port);
    fw_forward_port_setPort(obj, port);
    g_variant_unref(item);

    item = g_variant_get_child_value(variant, 1);
    g_variant_get(item, "s", &protocol);
    fw_forward_port_setProtocol(obj, protocol);
    g_variant_unref(item);

    item = g_variant_get_child_value(variant, 2);
    g_variant_get(item, "s", &toport);
    fw_forward_port_setToPort(obj, toport);
    g_variant_unref(item);

    item = g_variant_get_child_value(variant, 3);
    g_variant_get(item, "s", &toaddr);
    fw_forward_port_setToAddr(obj, toaddr);
    g_variant_unref(item);

    return obj;
}

FWForwardPort *
fw_forward_port_new(const gchar *port,
		    const gchar *protocol,
		    const gchar *toport,
		    const gchar *toaddr)
{
    FWForwardPort *obj = fw_forward_port_new_simple();

    fw_forward_port_setPort(obj, port);
    fw_forward_port_setProtocol(obj, protocol);
    fw_forward_port_setToPort(obj, toport);
    fw_forward_port_setToAddr(obj, toaddr);

    return obj;
}

GVariant *
fw_forward_port_to_variant(FWForwardPort *obj)
{
    FWForwardPortPrivate *priv = FW_FORWARD_PORT_GET_PRIVATE(obj);

    return g_variant_new("a(ssss)", priv->port, priv->protocol, priv->toport,
			 priv->toaddr);
}

static void
fw_forward_port_init(FWForwardPort *obj)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    /* init vars */
    prt->port = g_strdup("");
    prt->protocol = g_strdup("");
    prt->toport = g_strdup("");
    prt->toaddr = g_strdup("");
}

static void
fw_forward_port_finalize(GObject *obj)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    if (prt->port != NULL)
	g_free(prt->port);
    if (prt->protocol != NULL)
	g_free(prt->protocol);
    if (prt->toport != NULL)
	g_free(prt->toport);
    if (prt->toaddr != NULL)
	g_free(prt->toaddr);

    G_OBJECT_CLASS(fw_forward_port_parent_class)->finalize(obj);
}

static void
fw_forward_port_class_init(FWForwardPortClass *fw_forward_port_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_forward_port_class);

    obj_class->finalize = fw_forward_port_finalize;

    g_type_class_add_private(obj_class, sizeof(FWForwardPortPrivate));
}

/* methods */

/**
 * fw_forward_port_getPort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_forward_port_getPort(FWForwardPort *obj)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    return prt->port;
}

/**
 * fw_forward_port_getProtocol:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_forward_port_getProtocol(FWForwardPort *obj)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    return prt->protocol;
}

/**
 * fw_forward_port_getToPort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_forward_port_getToPort(FWForwardPort *obj)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    return prt->toport;
}

/**
 * fw_forward_port_getToAddr:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_forward_port_getToAddr(FWForwardPort *obj)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    return prt->toaddr;
}

/**
 * fw_forward_port_getStr:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_forward_port_getStr(FWForwardPort *obj)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    return g_strdup_printf("%s/%s%s%s%s%s", prt->port, prt->protocol,
			   (strlen(prt->toport) > 0) ? " >" : "",
			   prt->toport,
			   (strlen(prt->toaddr) > 0) ? " @" : "",
			   prt->toaddr);
}


void
fw_forward_port_setPort(FWForwardPort *obj, const gchar *port)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    if (prt->port != NULL)
	g_free(prt->port);
    prt->port = g_strdup(port);
}

void
fw_forward_port_setProtocol(FWForwardPort *obj, const gchar *protocol)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    if (prt->protocol != NULL)
	g_free(prt->protocol);
    prt->protocol = g_strdup(protocol);
}

void
fw_forward_port_setToPort(FWForwardPort *obj, const gchar *toport)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    if (prt->toport != NULL)
	g_free(prt->toport);
    prt->toport = g_strdup(toport);
}

void
fw_forward_port_setToAddr(FWForwardPort *obj, const gchar *toaddr)
{
    FWForwardPortPrivate *prt = FW_FORWARD_PORT_GET_PRIVATE(obj);

    if (prt->toaddr != NULL)
	g_free(prt->toaddr);
    prt->toaddr = g_strdup(toaddr);
}
