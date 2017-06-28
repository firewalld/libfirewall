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
#include "fw_config_zone.h"
#include "fw_functions.h"

G_DEFINE_TYPE(FWConfigZone, fw_config_zone, G_TYPE_OBJECT);

#define FW_CONFIG_ZONE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_CONFIG_ZONE_TYPE, FWConfigZonePrivate))

typedef struct {
    GDBusConnection *connection;
    GDBusProxy *proxy;
    GError *error;

    gchar *path;
} FWConfigZonePrivate;

enum _FWConfigZoneProperties {
    PROP_0,
    PROP_PATH,
    PROP_BUILTIN,
    PROP_DEFAULT,
    PROP_FILENAME,
    PROP_NAME,
    N_PROPERTIES,
};

static void _fw_config_zone_dbus_connect(FWConfigZonePrivate *fw);

FWConfigZone *
fw_config_zone_new(gchar *path)
{
    FWConfigZone *obj = g_object_new(FW_CONFIG_ZONE_TYPE, NULL);
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    priv->path = g_strdup(path);

    _fw_config_zone_dbus_connect(priv);

    return obj;
    /*
      return g_object_new(FW_CONFIG_ZONE_TYPE,
			"path", path,
			NULL);
    */
}

static void
fw_config_zone_init(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    /* init vars */
    priv->connection = NULL;
    priv->proxy = NULL;
    priv->error = NULL;

    priv->connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &priv->error);
    if (priv->error != NULL) {
        g_print("ERROR: Failed to connect to system bus: %s",
		priv->error->message);
	return;
    }

#ifdef FW_DEBUG
    gchar *conn_name;
    g_object_get(priv->connection, "unique-name", &conn_name, NULL);
    g_free(conn_name);
#endif
}

static void
_fw_config_zone_reset_error(FWConfigZonePrivate *priv)
{
    if (priv->error != NULL) {
	g_error_free(priv->error);
	priv->error = NULL;
    }
}

static void
_fw_config_zone_signal_receiver(GDBusProxy *proxy,
				   gchar *sender_name,
				   gchar *signal_name,
				   GVariant *parameters,
				   gpointer user_data)
{
    /* print received signals */
    gchar *str;
    str = g_variant_print(parameters, TRUE);
    g_print("received signal: %s: %s\n", signal_name, str);
    g_free(str);
}

static void
_fw_config_zone_dbus_connect(FWConfigZonePrivate *priv)
{
    _fw_config_zone_reset_error(priv);

    /* create syncroneous proxies */
    priv->proxy = g_dbus_proxy_new_sync(priv->connection,
					G_DBUS_PROXY_FLAGS_NONE,
					NULL,
					FW_DBUS_NAME,
					priv->path,
					FW_DBUS_INTERFACE_CONFIG_ZONE,
					NULL,
					&priv->error);

    /* G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START |
       G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES */

    g_assert(priv->proxy != NULL);
    g_dbus_proxy_set_default_timeout(priv->proxy, G_MAXINT);

    /* connect signal receiver */
    g_signal_connect(priv->proxy,
		     "g-signal",
		     G_CALLBACK (_fw_config_zone_signal_receiver),
		     NULL);

    /* do not terminate if connection has been closed */
    g_dbus_connection_set_exit_on_close(priv->connection, FALSE);
}

static void
fw_config_zone_finalize(GObject *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_reset_error(priv);

    /* disconnect */
    /***/

    G_OBJECT_CLASS(fw_config_zone_parent_class)->finalize(obj);
}

static void
fw_config_zone_set_property(GObject      *obj,
			       guint         property_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    switch (property_id)
    {
    case PROP_PATH:
	priv->path = g_strdup(g_value_get_string(value));
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
        break;
    }
}

static void
fw_config_zone_get_property(GObject *obj,
			       guint property_id,
			       GValue *value,
			       GParamSpec *pspec)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    switch (property_id)
    {
    case PROP_PATH:
	g_value_set_string(value, priv->path);
	break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
        break;
    }
}

static void
fw_config_zone_class_init(FWConfigZoneClass *fw_config_zone_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_config_zone_class);

#ifdef FW_DEBUG
    g_print("fw_config_zone_class_init\n");
#endif

/*
    GParamSpec *param_spec = NULL;

    obj_class->get_property = fw_config_zone_get_property;
    obj_class->set_property = fw_config_zone_set_property;

    param_spec = g_param_spec_string("path",
				     "dbus path",
				     "Set dbus path",
				     "",
				     G_PARAM_CONSTRUCT_ONLY |
				     G_PARAM_READWRITE);
    g_object_class_install_property(obj_class, PROP_PATH, param_spec);
*/

    obj_class->finalize = fw_config_zone_finalize;

    g_type_class_add_private(obj_class, sizeof(FWConfigZonePrivate));
}

GVariant *
_fw_config_zone_proxy_call_sync(FWConfigZonePrivate *priv,
				   GDBusProxy *proxy,
				   const gchar *method_name,
				   GVariant *parameters)
{
    GVariant *result;

    _fw_config_zone_reset_error(priv);

    result = g_dbus_proxy_call_sync(proxy,
				    method_name,
				    parameters,
				    G_DBUS_CALL_FLAGS_NONE,
				    -1,
				    NULL,
				    &priv->error);
    if (priv->error != NULL) {
        g_print(_("ERROR: %s failed: %s\n"), method_name, priv->error->message);
    }

    return result;
}

gchar *
_fw_config_zone_proxy_call_sync_get_str(FWConfigZonePrivate *priv,
					   GDBusProxy *proxy,
					   const gchar *method_name,
					   GVariant *parameters)
{
    GVariant *variant;
    gchar *value_str;

    variant = _fw_config_zone_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return NULL;
    }

    g_variant_get(variant, "(s)", &value_str);
    g_variant_unref(variant);

    return value_str;
}

gboolean
_fw_config_zone_proxy_call_sync_get_bool(FWConfigZonePrivate *priv,
					    GDBusProxy *proxy,
					    const gchar *method_name,
					    GVariant *parameters)
{
    GVariant *variant;
    gboolean value;

    variant = _fw_config_zone_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return FALSE;
    }

    g_variant_get(variant, "(b)", &value);
    g_variant_unref(variant);

    return value;
}

/* methods */

/**
 * fw_config_zone_getSettings:
 *
 * Returns: (transfer none) (allow-none) (type FWZone*)
 */
FWZone *
fw_config_zone_getSettings(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;
    FWZone *hlpr = NULL;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
						 "getSettings", NULL);

    if (priv->error != NULL)
       return hlpr;

    hlpr = fw_zone_new_from_variant(variant);
    g_variant_unref(variant);

    return hlpr;
}

void
fw_config_zone_update(FWConfigZone *obj,
			 FWZone *settings)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(
	priv, priv->proxy, "update",
	g_variant_new("(v)", fw_zone_to_variant(settings)));
}

void
fw_config_zone_loadDefaults(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "loadDefaults",
				       NULL);
}

void
fw_config_zone_remove(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "remove", NULL);
}

void
fw_config_zone_rename(FWConfigZone *obj, gchar *name)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "rename",
				       g_variant_new("(s)", name));
}

/**
 * fw_config_zone_getVersion:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_zone_getVersion(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_str(priv, priv->proxy,
						      "getVersion", NULL);
}

void
fw_config_zone_setVersion(FWConfigZone *obj,
			    gchar *version)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setVersion",
				       g_variant_new("(s)", version));
}

/**
 * fw_config_zone_getShort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_zone_getShort(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_str(priv, priv->proxy,
						      "getShort", NULL);
}

void
fw_config_zone_setShort(FWConfigZone *obj,
			   gchar *short_description)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setShort",
				       g_variant_new("(s)", short_description));
}

/**
 * fw_config_zone_getDescription:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_zone_getDescription(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_str(priv, priv->proxy,
						      "getDescription", NULL);
}

void
fw_config_zone_setDescription(FWConfigZone *obj,
				gchar *description)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setDescription",
				       g_variant_new("(s)", description));
}

/**
 * fw_config_zone_getTarget:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_zone_getTarget(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_str(priv, priv->proxy,
						   "getTarget", NULL);
}

void
fw_config_zone_setTarget(FWConfigZone *obj,
			 gchar *target)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setTarget",
				    g_variant_new("(s)", target));
}

/**
 * fw_config_zone_getServices:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_zone_getServices(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
					      "getServices", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_str_list_new_from_variant(variant);
}

/**
 * fw_config_zone_setServices:
 * @obj: (type FWConfigZone*): a FWConfigZone instance
 * @services: (type GList*) (element-type gchar*)
 */
void
fw_config_zone_setServices(FWConfigZone *obj,
			   GList *services)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setServices",
				    fw_str_list_to_variant(services));
}

void
fw_config_zone_addService(FWConfigZone *obj,
			  gchar *service)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addService",
				    g_variant_new("(s)", service));
}

void
fw_config_zone_removeService(FWConfigZone *obj,
			     gchar *service)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeService",
				    g_variant_new("(s)", service));
}

gboolean
fw_config_zone_queryService(FWConfigZone *obj,
			    gchar *service)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryService",
	g_variant_new("(s)", service));
}

/**
 * fw_config_zone_getPorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_config_zone_getPorts(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
						 "getPorts", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_port_list_new_from_variant(variant);
}

/**
 * fw_config_zone_setPorts:
 * @obj: (type FWConfigZone*): a FWConfigZone instance
 * @ports: (type FWPortList*)
 */
void
fw_config_zone_setPorts(FWConfigZone *obj,
			FWPortList *ports)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setPorts",
				    fw_port_list_to_variant(ports));
}

void
fw_config_zone_addPort(FWConfigZone *obj,
		       gchar *port,
		       gchar *protocol)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addPort",
				    g_variant_new("(ss)", port, protocol));
}

void
fw_config_zone_removePort(FWConfigZone *obj,
			     gchar *port,
			     gchar *protocol)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removePort",
				    g_variant_new("(ss)", port, protocol));
}

gboolean
fw_config_zone_queryPort(FWConfigZone *obj,
			 gchar *port,
			 gchar *protocol)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryPort",
	g_variant_new("(ss)", port, protocol));
}

void
fw_config_zone_add_port(FWConfigZone *obj,
			FWPort *port)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addPort",
				       fw_port_to_variant(port));
}

void
fw_config_zone_remove_port(FWConfigZone *obj,
			   FWPort *port)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removePort",
				    fw_port_to_variant(port));
}

gboolean
fw_config_zone_query_port(FWConfigZone *obj,
			  FWPort *port)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryPort",
	fw_port_to_variant(port));
}

/**
 * fw_config_zone_getProtocols:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_zone_getProtocols(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
						 "getProtocols", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_str_list_new_from_variant(variant);
}

/**
 * fw_config_zone_setProtocols:
 * @obj: (type FWConfigZone*): a FWConfigZone instance
 * @protocols: (type GList*) (element-type gchar*)
 */
void
fw_config_zone_setProtocols(FWConfigZone *obj,
			       GList *protocols)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setProtocols",
				       fw_str_list_to_variant(protocols));
}

void
fw_config_zone_addProtocol(FWConfigZone *obj,
			      gchar *protocol)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addProtocol",
				       g_variant_new("(s)", protocol));
}

void
fw_config_zone_removeProtocol(FWConfigZone *obj,
				 gchar *protocol)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeProtocol",
				       g_variant_new("(s)", protocol));
}

gboolean
fw_config_zone_queryProtocol(FWConfigZone *obj,
				gchar *protocol)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryProtocol",
	g_variant_new("(s)", protocol));
}

/**
 * fw_config_zone_getSourcePorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_config_zone_getSourcePorts(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
						 "getSourcePorts", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_port_list_new_from_variant(variant);
}

/**
 * fw_config_zone_setSourcePorts:
 * @obj: (type FWConfigZone*): a FWConfigZone instance
 * @ports: (type FWPortList*)
 */
void
fw_config_zone_setSourcePorts(FWConfigZone *obj,
				 FWPortList *ports)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setSourcePorts",
				       fw_port_list_to_variant(ports));
}

void
fw_config_zone_addSourcePort(FWConfigZone *obj,
				gchar *port,
				gchar *protocol)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addSourcePort",
				       g_variant_new("(ss)", port, protocol));
}

void
fw_config_zone_removeSourcePort(FWConfigZone *obj,
				   gchar *port,
				   gchar *protocol)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeSourcePort",
				       g_variant_new("(ss)", port, protocol));
}

gboolean
fw_config_zone_querySourcePort(FWConfigZone *obj,
				  gchar *port,
				  gchar *protocol)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "querySourcePort",
	g_variant_new("(ss)", port, protocol));
}

void
fw_config_zone_add_source_port(FWConfigZone *obj,
				  FWPort *port)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addSourcePort",
				       fw_port_to_variant(port));
}

void
fw_config_zone_remove_source_port(FWConfigZone *obj,
				     FWPort *port)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeSourcePort",
				       fw_port_to_variant(port));
}

gboolean
fw_config_zone_query_source_port(FWConfigZone *obj,
				    FWPort *port)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "querySourcePort",
	fw_port_to_variant(port));
}

/**
 * fw_config_zone_getIcmpBlocks:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_zone_getIcmpBlocks(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
					      "getIcmpBlocks", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_str_list_new_from_variant(variant);
}

/**
 * fw_config_zone_setIcmpBlocks:
 * @obj: (type FWConfigZone*): a FWConfigZone instance
 * @icmpblocks: (type GList*) (element-type gchar*)
 */
void
fw_config_zone_setIcmpBlocks(FWConfigZone *obj,
			     GList *icmp_types)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setIcmpBlocks",
				    fw_str_list_to_variant(icmp_types));
}

void
fw_config_zone_addIcmpBlock(FWConfigZone *obj,
			      gchar *icmp_type)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addIcmpBlock",
				    g_variant_new("(s)", icmp_type));
}

void
fw_config_zone_removeIcmpBlock(FWConfigZone *obj,
			       gchar *icmp_type)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeIcmpBlock",
				    g_variant_new("(s)", icmp_type));
}

gboolean
fw_config_zone_queryIcmpBlock(FWConfigZone *obj,
			      gchar *icmp_type)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryIcmpBlock",
	g_variant_new("(s)", icmp_type));
}

gboolean
fw_config_zone_getMasquerade(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "getMasquerade", NULL);
}

void
fw_config_zone_setMasquerade(FWConfigZone *obj,
			     gboolean masquerade)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setMasquerade",
				    g_variant_new("(b)", masquerade));
}

void
fw_config_zone_addMasquerade(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addMasquerade",
				    NULL);
}

void
fw_config_zone_removeMasquerade(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeMasquerade",
				    NULL);
}

gboolean
fw_config_zone_queryMasquerade(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryMasquerade", NULL);
}

/**
 * fw_config_zone_getForwardPorts:
 *
 * Returns: (transfer none) (allow-none) (type FWForwardPortList*)
 */
FWForwardPortList *
fw_config_zone_getForwardPorts(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
					      "getForwardPorts", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_forward_port_list_new_from_variant(variant);
}

/**
 * fw_config_zone_setForwardPorts:
 * @obj: (type FWConfigZone*): a FWConfigZone instance
 * @forward_ports: (type FWForwardPortList*)
 */
void
fw_config_zone_setForwardPorts(FWConfigZone *obj,
			       FWForwardPortList *forward_ports)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setForwardPorts",
				    fw_forward_port_list_to_variant(
					forward_ports));
}

void
fw_config_zone_addForward_Port(FWConfigZone *obj,
			       gchar *port,
			       gchar *protocol,
			       gchar *toport,
			       gchar *toaddr)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addForward_Port",
				    g_variant_new("(ssss)", port, protocol,
						  toport, toaddr));
}

void
fw_config_zone_removeForward_Port(FWConfigZone *obj,
				  gchar *port,
				  gchar *protocol,
				  gchar *toport,
				  gchar *toaddr)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeForward_Port",
				       g_variant_new("(ssss)", port, protocol,
						     toport, toaddr));
}

gboolean
fw_config_zone_queryForward_Port(FWConfigZone *obj,
				 gchar *port,
				 gchar *protocol,
				 gchar *toport,
				 gchar *toaddr)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryForward_Port",
	g_variant_new("(ssss)", port, protocol, toport, toaddr));
}

void
fw_config_zone_add_forward_port(FWConfigZone *obj,
			   FWForwardPort *forward_port)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addForward_Port",
				       fw_forward_port_to_variant(
					   forward_port));
}

void
fw_config_zone_remove_forward_port(FWConfigZone *obj,
			      FWForwardPort *forward_port)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeForward_Port",
				       fw_forward_port_to_variant(
					   forward_port));
}

gboolean
fw_config_zone_query_forward_port(FWConfigZone *obj,
			     FWForwardPort *forward_port)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryForward_Port",
	fw_forward_port_to_variant(forward_port));
}

/**
 * fw_config_zone_getInterfaces:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_zone_getInterfaces(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
					      "getInterfaces", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_str_list_new_from_variant(variant);
}

/**
 * fw_config_zone_setInterfaces:
 * @obj: (type FWConfigZone*): a FWConfigZone instance
 * @interfaces: (type GList*) (element-type gchar*)
 */
void
fw_config_zone_setInterfaces(FWConfigZone *obj,
			     GList *interfaces)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setInterfaces",
				       fw_str_list_to_variant(interfaces));
}

void
fw_config_zone_addInterface(FWConfigZone *obj,
			    gchar *interface)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addInterface",
				    g_variant_new("(s)", interface));
}

void
fw_config_zone_removeInterface(FWConfigZone *obj,
			       gchar *interface)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeInterface",
				    g_variant_new("(s)", interface));
}

gboolean
fw_config_zone_queryInterface(FWConfigZone *obj,
			      gchar *interface)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryInterface",
	g_variant_new("(s)", interface));
}

/**
 * fw_config_zone_getSources:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_zone_getSources(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
					      "getSources", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_str_list_new_from_variant(variant);
}

/**
 * fw_config_zone_setSources:
 * @obj: (type FWConfigZone*): a FWConfigZone instance
 * @sources: (type GList*) (element-type gchar*)
 */
void
fw_config_zone_setSources(FWConfigZone *obj,
			  GList *sources)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setSources",
				    fw_str_list_to_variant(sources));
}

void
fw_config_zone_addSource(FWConfigZone *obj,
			 gchar *source)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addSource",
				    g_variant_new("(s)", source));
}

void
fw_config_zone_removeSource(FWConfigZone *obj,
			    gchar *source)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeSource",
				    g_variant_new("(s)", source));
}

gboolean
fw_config_zone_querySource(FWConfigZone *obj,
			   gchar *source)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "querySource",
	g_variant_new("(s)", source));
}

/**
 * fw_config_zone_getRichRules:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_zone_getRichRules(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_zone_proxy_call_sync(priv, priv->proxy,
					      "getRichRules", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_str_list_new_from_variant(variant);
}

/**
 * fw_config_zone_setRichRules:
 * @obj: (type FWConfigZone*): a FWConfigZone instance
 * @richrules: (type GList*) (element-type gchar*)
 */
void
fw_config_zone_setRichRules(FWConfigZone *obj,
			    GList *rich_rules)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setRichRules",
				    fw_str_list_to_variant(rich_rules));
}

void
fw_config_zone_addRichRule(FWConfigZone *obj,
			   gchar *rich_rule)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addRichRule",
				    g_variant_new("(s)", rich_rule));
}

void
fw_config_zone_removeRichRule(FWConfigZone *obj,
			      gchar *rich_rule)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "removeRichRule",
				    g_variant_new("(s)", rich_rule));
}

gboolean
fw_config_zone_queryRichRule(FWConfigZone *obj,
			     gchar *rich_rule)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryRichRule",
	g_variant_new("(s)", rich_rule));
}

gboolean
fw_config_zone_getIcmpBlockInversion(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "getIcmpBlockInversion", NULL);
}

void
fw_config_zone_setIcmpBlockInversion(FWConfigZone *obj,
				     gboolean icmp_block_inversion)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "setIcmpBlockInversion",
				    g_variant_new("(b)", icmp_block_inversion));
}

void
fw_config_zone_addIcmpBlockInversion(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy, "addIcmpBlockInversion",
				    NULL);
}

void
fw_config_zone_removeIcmpBlockInversion(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    _fw_config_zone_proxy_call_sync(priv, priv->proxy,
				    "removeIcmpBlockInversion",
				    NULL);
}

gboolean
fw_config_zone_queryIcmpBlockInversion(FWConfigZone *obj)
{
    FWConfigZonePrivate *priv = FW_CONFIG_ZONE_GET_PRIVATE(obj);

    return _fw_config_zone_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryIcmpBlockInversion", NULL);
}
