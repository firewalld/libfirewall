/*
 * Copyright (C) 2017 Red Hat, Inc.
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
#include "fw_port_list.h"

G_DEFINE_TYPE(FWPortList, fw_port_list, G_TYPE_OBJECT);

#define FW_PORT_LIST_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_PORT_LIST_TYPE, FWPortListPrivate))

typedef struct {
    GList *ports;              /* list of FWPort */
} FWPortListPrivate;

FWPortList *
fw_port_list_new()
{
    return g_object_new(FW_PORT_LIST_TYPE, NULL);
}

FWPortList *
fw_port_list_new_from_variant(GVariant *variant)
{
    FWPortList *list = NULL;
    GVariantIter iter;

    if (variant == NULL)
	return NULL;

    if (strncmp(g_variant_get_type_string(variant), "(a(ss))", 7) != 0) {
	if (strncmp(g_variant_get_type_string(variant), "a(ss)", 5) != 0)
	    return NULL;
    }

    list = fw_port_list_new();

    if (g_variant_iter_init(&iter, variant)) {
	GVariant *element;

	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    GVariant *item2=NULL;
	    gchar *port, *protocol;

	    item2 = g_variant_get_child_value(element, 0);
	    g_variant_get(item2, "s", &port);
	    g_variant_unref(item2);

	    item2 = g_variant_get_child_value(element, 1);
	    g_variant_get(item2, "s", &protocol);
	    g_variant_unref(item2);

	    fw_port_list_addPort(list, port, protocol);
	    g_variant_unref(element);
	}
    }

    return list;
}

GVariantBuilder *
fw_port_list_to_builder(FWPortList *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a(ss)"));
    gint i;

    for (i=0; i<g_list_length(priv->ports); i++) {
	FWPort *port = g_list_nth_data(priv->ports, i);
	g_variant_builder_add(builder, "(ss)",
			      fw_port_getPort(port),
			      fw_port_getProtocol(port));
    }

    return builder;
}

GVariant *
fw_port_list_to_variant(FWPortList *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    GVariantBuilder builder;
    gint i;

    g_variant_builder_init(&builder, G_VARIANT_TYPE("a(ss)"));
    for (i=0; i<g_list_length(priv->ports); i++) {
	FWPort *port = g_list_nth_data(priv->ports, i);
	g_variant_builder_add(&builder, "(ss)",
			      fw_port_getPort(port),
			      fw_port_getProtocol(port));
    }

    return g_variant_new("(a(ss))", &builder);
}

guint
fw_port_list_length(FWPortList *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);

    return g_list_length(priv->ports);
}

gpointer
fw_port_list_nth_data(FWPortList *obj,
		guint n)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);

    return g_list_nth_data(priv->ports, n);
}

void
fw_port_list_free(FWPortList *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);

    if (priv->ports != NULL) {
/*
        gint i;
	for (i=0; i<g_list_length(priv->ports); i++)
	    g_free(g_list_nth_data(priv->ports, i));
*/
	g_list_free(priv->ports);
    }
}

static void
fw_port_list_init(FWPortList *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);

    /* init vars */
    priv->ports = NULL;
}

static void
fw_port_list_finalize(GObject *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    gint i;

    if (priv->ports != NULL) {
	for (i=0; i<g_list_length(priv->ports); i++)
	    g_free(g_list_nth_data(priv->ports, i));
	g_list_free(priv->ports);
    }

    G_OBJECT_CLASS(fw_port_list_parent_class)->finalize(obj);
}

static void
fw_port_list_class_init(FWPortListClass *fw_port_list_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_port_list_class);

    obj_class->finalize = fw_port_list_finalize;

    g_type_class_add_private(obj_class, sizeof(FWPortListPrivate));
}

/* methods */

/**
 * fw_port_list_getStr:
 *
 */
gchar *
fw_port_list_getStr(FWPortList *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    GString *gstr = g_string_new("");
    gint i;

    g_string_append(gstr, "[ ");
    for (i=0; i<g_list_length(priv->ports); i++) {
	FWPort *prt = g_list_nth_data(priv->ports, i);
	gchar *str2 = fw_port_getStr(prt);
	gchar *str3 = g_strdup_printf("%s'%s'", (i>0) ? ", " : "", str2);
	g_string_append(gstr, str3);
	g_free(str2);
	g_free(str3);
    }
    g_string_append(gstr, "] ");

    return g_string_free(gstr, FALSE);
}

/**
 * fw_port_list_print_str:
 *
 */
void
fw_port_list_print_str(FWPortList *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    gint i;

    g_print("[ ");
    for (i=0; i<g_list_length(priv->ports); i++) {
	FWPort *prt = g_list_nth_data(priv->ports, i);
	gchar *str = fw_port_getStr(prt);

	g_print("%s'%s'", (i>0) ? ", " : "", str);
	g_free(str);
    }
    g_print(" ]\n");
}

/**
 * fw_port_list_print_simple:
 *
 */
void
fw_port_list_print_simple(FWPortList *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    gint i;

    for (i=0; i<g_list_length(priv->ports); i++) {
	FWPort *prt = g_list_nth_data(priv->ports, i);
	gchar *str = fw_port_getStr(prt);

	g_print("%s%s", (i>0) ? " " : "", str);
	g_free(str);
    }
}

/**
 * fw_port_list_getPorts:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type FWPort*)
 */
GList *
fw_port_list_getPorts(FWPortList *obj)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);

    return priv->ports;
}

/**
 * fw_port_list_setPorts:
 * @obj: (type FWPortList*): a FWPortList instance
 * @ports: (type GList*) (element-type FWPort*)
 */
void
fw_port_list_setPorts(FWPortList *obj,
		   GList *ports)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);

    if (priv->ports != NULL)
	g_free(priv->ports); /* TODO* */
    priv->ports = ports; /* TODO */
}

void
fw_port_list_addPort(FWPortList *obj,
		     gchar *port,
		     gchar *protocol)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    priv->ports = g_list_append(priv->ports, item);
}

void
fw_port_list_removePort(FWPortList *obj,
			gchar *port,
			gchar *protocol)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);

    priv->ports = g_list_remove(priv->ports, item);
}

gboolean
fw_port_list_queryPort(FWPortList *obj,
		       gchar *port,
		       gchar *protocol)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    FWPort *item = fw_port_new(port, protocol);
    GList *list;

    list = g_list_find(priv->ports, item);
    if (list == NULL)
	return FALSE;
    return TRUE;
}

void
fw_port_list_add(FWPortList *obj,
		 FWPort *port)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);

    priv->ports = g_list_append(priv->ports, port);
}

void
fw_port_list_remove(FWPortList *obj,
		    FWPort *port)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);

    priv->ports = g_list_remove(priv->ports, port);
}

gboolean
fw_port_list_query(FWPortList *obj,
		   FWPort *port)
{
    FWPortListPrivate *priv = FW_PORT_LIST_GET_PRIVATE(obj);
    GList *list;

    list = g_list_find(priv->ports, port);
    if (list == NULL)
	return FALSE;
    return TRUE;
}
