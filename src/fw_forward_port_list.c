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
#include "fw_forward_port_list.h"

G_DEFINE_TYPE(FWForwardPortList, fw_forward_port_list, G_TYPE_OBJECT);

#define FW_FORWARD_PORT_LIST_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_FORWARD_PORT_LIST_TYPE, FWForwardPortListPrivate))

typedef struct {
    GList *forward_ports;              /* list of FWForwardPort */
} FWForwardPortListPrivate;

FWForwardPortList *
fw_forward_port_list_new()
{
    return g_object_new(FW_FORWARD_PORT_LIST_TYPE, NULL);
}

FWForwardPortList *
fw_forward_port_list_new_from_variant(GVariant *variant)
{
    FWForwardPortList *list = NULL;
    GVariantIter iter;

    if (variant == NULL)
	return list;

    if (strncmp(g_variant_get_type_string(variant), "(a(ssss))", 9) != 0) {
	if (strncmp(g_variant_get_type_string(variant), "a(ssss)", 7) != 0)
	    return list;
    }

    list = fw_forward_port_list_new();

    if (g_variant_iter_init(&iter, variant)) {
	GVariant *element;

	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    GVariant *item2=NULL;
	    gchar *port, *protocol, *toport, *toaddr;

	    item2 = g_variant_get_child_value(element, 0);
	    g_variant_get(item2, "s", &port);
	    g_variant_unref(item2);

	    item2 = g_variant_get_child_value(element, 1);
	    g_variant_get(item2, "s", &protocol);
	    g_variant_unref(item2);

	    item2 = g_variant_get_child_value(element, 2);
	    g_variant_get(item2, "s", &toport);
	    g_variant_unref(item2);

	    item2 = g_variant_get_child_value(element, 3);
	    g_variant_get(item2, "s", &toaddr);
	    g_variant_unref(item2);

	    fw_forward_port_list_addForwardPort(list, port, protocol,
						toport, toaddr);
	    g_variant_unref(element);
	}
    }

    return list;
}

GVariantBuilder *
fw_forward_port_list_to_builder(FWForwardPortList *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a(ssss)"));
    gint i;

    for (i=0; i<g_list_length(priv->forward_ports); i++) {
	FWForwardPort *forward_port = g_list_nth_data(priv->forward_ports, i);
	g_variant_builder_add(builder, "(ssss)",
			      fw_forward_port_getPort(forward_port),
			      fw_forward_port_getProtocol(forward_port),
			      fw_forward_port_getToPort(forward_port),
			      fw_forward_port_getToAddr(forward_port));
    }

    return builder;
}

GVariant *
fw_forward_port_list_to_variant(FWForwardPortList *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    GVariantBuilder builder;
    gint i;

    g_variant_builder_init(&builder, G_VARIANT_TYPE("a(ssss)"));
    for (i=0; i<g_list_length(priv->forward_ports); i++) {
	FWForwardPort *forward_port = g_list_nth_data(priv->forward_ports, i);
	g_variant_builder_add(&builder, "(ssss)",
			      fw_forward_port_getPort(forward_port),
			      fw_forward_port_getProtocol(forward_port),
			      fw_forward_port_getToPort(forward_port),
			      fw_forward_port_getToAddr(forward_port));
    }

    return g_variant_new("(a(ssss))", &builder);
}

guint
fw_forward_port_list_length(FWForwardPortList *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    return g_list_length(priv->forward_ports);
}

gpointer
fw_forward_port_list_nth_data(FWForwardPortList *obj,
		guint n)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    return g_list_nth_data(priv->forward_ports, n);
}

void
fw_forward_port_list_free(FWForwardPortList *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    if (priv->forward_ports != NULL) {
/*
        gint i;
	for (i=0; i<g_list_length(priv->forward_ports); i++)
	    g_free(g_list_nth_data(priv->forward_ports, i));
*/
	g_list_free(priv->forward_ports);
    }
}

static void
fw_forward_port_list_init(FWForwardPortList *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    /* init vars */
    priv->forward_ports = NULL;
}

static void
fw_forward_port_list_finalize(GObject *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    gint i;

    if (priv->forward_ports != NULL) {
	for (i=0; i<g_list_length(priv->forward_ports); i++)
	    g_free(g_list_nth_data(priv->forward_ports, i));
	g_list_free(priv->forward_ports);
    }

    G_OBJECT_CLASS(fw_forward_port_list_parent_class)->finalize(obj);
}

static void
fw_forward_port_list_class_init(FWForwardPortListClass *fw_forward_port_list_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_forward_port_list_class);

    obj_class->finalize = fw_forward_port_list_finalize;

    g_type_class_add_private(obj_class, sizeof(FWForwardPortListPrivate));
}

/* methods */

/**
 * fw_forward_port_list_getStr:
 *
 */
gchar *
fw_forward_port_list_getStr(FWForwardPortList *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    GString *gstr = g_string_new("");
    gint i;

    g_string_append(gstr, "[ ");
    for (i=0; i<g_list_length(priv->forward_ports); i++) {
	FWForwardPort *prt = g_list_nth_data(priv->forward_ports, i);
	gchar *str2 = fw_forward_port_getStr(prt);
	gchar *str3 = g_strdup_printf("%s'%s'", (i>0) ? ", " : "", str2);
	g_string_append(gstr, str3);
	g_free(str2);
	g_free(str3);
    }
    g_string_append(gstr, "] ");

    return g_string_free(gstr, FALSE);
}

/**
 * fw_forward_port_list_print_str:
 *
 */
void
fw_forward_port_list_print_str(FWForwardPortList *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    gint i;

    g_print("[ ");
    for (i=0; i<g_list_length(priv->forward_ports); i++) {
	FWForwardPort *prt = g_list_nth_data(priv->forward_ports, i);
	gchar *str = fw_forward_port_getStr(prt);

	g_print("%s'%s'", (i>0) ? ", " : "", str);
	g_free(str);
    }
    g_print(" ]\n");
}

/**
 * fw_forward_port_list_print_simple:
 *
 */
void
fw_forward_port_list_print_simple(FWForwardPortList *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    gint i;

    for (i=0; i<g_list_length(priv->forward_ports); i++) {
	FWForwardPort *prt = g_list_nth_data(priv->forward_ports, i);
	gchar *str = fw_forward_port_getStr(prt);

	g_print("%s%s", (i>0) ? " " : "", str);
	g_free(str);
    }
}

/**
 * fw_forward_port_list_getForwardPorts:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type FWForwardPort*)
 */
GList *
fw_forward_port_list_getForwardPorts(FWForwardPortList *obj)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    return priv->forward_ports;
}

/**
 * fw_forward_port_list_setForwardPorts:
 * @obj: (type FWForwardPortList*): a FWForwardPortList instance
 * @forward_ports: (type GList*) (element-type FWForwardPort*)
 */
void
fw_forward_port_list_setForwardPorts(FWForwardPortList *obj,
		   GList *forward_ports)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    if (priv->forward_ports != NULL)
	g_free(priv->forward_ports); /* TODO* */
    priv->forward_ports = forward_ports; /* TODO */
}

/*
void
fw_forward_port_list_addForwardPort(FWForwardPortList *obj,
		  FWForwardPort *forward_port)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    priv->forward_ports = g_list_append(priv->forward_ports, forward_port);
}

void
fw_forward_port_list_removeForwardPort(FWForwardPortList *obj,
		     FWForwardPort *forward_port)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    priv->forward_ports = g_list_remove(priv->forward_ports, forward_port);
}

gboolean
fw_forward_port_list_queryForwardPort(FWForwardPortList *obj,
		    FWForwardPort *forward_port)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    GList *list;

    list = g_list_find(priv->forward_ports, forward_port);
    if (list == NULL)
	return FALSE;
    return TRUE;
}
*/

void
fw_forward_port_list_addForwardPort(FWForwardPortList *obj,
				    gchar *port,
				    gchar *protocol,
				    gchar *toport,
				    gchar *toaddr)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    FWForwardPort *item = fw_forward_port_new(port, protocol, toport, toaddr);

    priv->forward_ports = g_list_append(priv->forward_ports, item);
}

void
fw_forward_port_list_removeForwardPort(FWForwardPortList *obj,
				       gchar *forward_port,
				       gchar *protocol,
				       gchar *toport,
				       gchar *toaddr)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    FWForwardPort *item = fw_forward_port_new(forward_port, protocol,
					      toport, toaddr);

    priv->forward_ports = g_list_remove(priv->forward_ports, item);
}

gboolean
fw_forward_port_list_queryForwardPort(FWForwardPortList *obj,
				      gchar *forward_port,
				      gchar *protocol,
				      gchar *toport,
				      gchar *toaddr)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    FWForwardPort *item = fw_forward_port_new(forward_port, protocol,
					      toport, toaddr);
    GList *list;

    list = g_list_find(priv->forward_ports, item);
    if (list == NULL)
	return FALSE;
    return TRUE;
}

void
fw_forward_port_list_add(FWForwardPortList *obj,
			 FWForwardPort *forward_port)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    priv->forward_ports = g_list_append(priv->forward_ports, forward_port);
}

void
fw_forward_port_list_remove(FWForwardPortList *obj,
			    FWForwardPort *forward_port)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);

    priv->forward_ports = g_list_remove(priv->forward_ports, forward_port);
}

gboolean
fw_forward_port_list_query(FWForwardPortList *obj,
			   FWForwardPort *forward_port)
{
    FWForwardPortListPrivate *priv = FW_FORWARD_PORT_LIST_GET_PRIVATE(obj);
    GList *list;

    list = g_list_find(priv->forward_ports, forward_port);
    if (list == NULL)
	return FALSE;
    return TRUE;
}
