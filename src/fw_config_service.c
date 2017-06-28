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
#include "fw_config_service.h"
#include "fw_functions.h"

G_DEFINE_TYPE(FWConfigService, fw_config_service, G_TYPE_OBJECT);

#define FW_CONFIG_SERVICE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_CONFIG_SERVICE_TYPE, FWConfigServicePrivate))

typedef struct {
    GDBusConnection *connection;
    GDBusProxy *proxy;
    GError *error;

    gchar *path;
} FWConfigServicePrivate;

enum _FWConfigServiceProperties {
    PROP_0,
    PROP_PATH,
    PROP_BUILTIN,
    PROP_DEFAULT,
    PROP_FILENAME,
    PROP_NAME,
    N_PROPERTIES,
};

static void _fw_config_service_dbus_connect(FWConfigServicePrivate *fw);

FWConfigService *
fw_config_service_new(gchar *path)
{
    FWConfigService *obj = g_object_new(FW_CONFIG_SERVICE_TYPE, NULL);
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);
    priv->path = g_strdup(path);

    _fw_config_service_dbus_connect(priv);

    return obj;
    /*
      return g_object_new(FW_CONFIG_SERVICE_TYPE,
			"path", path,
			NULL);
    */
}

static void
fw_config_service_init(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

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
_fw_config_service_reset_error(FWConfigServicePrivate *priv)
{
    if (priv->error != NULL) {
	g_error_free(priv->error);
	priv->error = NULL;
    }
}

static void
_fw_config_service_signal_receiver(GDBusProxy *proxy,
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
_fw_config_service_dbus_connect(FWConfigServicePrivate *priv)
{
    _fw_config_service_reset_error(priv);

    /* create syncroneous proxies */
    priv->proxy = g_dbus_proxy_new_sync(priv->connection,
					G_DBUS_PROXY_FLAGS_NONE,
					NULL,
					FW_DBUS_NAME,
					priv->path,
					FW_DBUS_INTERFACE_CONFIG_SERVICE,
					NULL,
					&priv->error);

    /* G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START |
       G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES */

    g_assert(priv->proxy != NULL);
    g_dbus_proxy_set_default_timeout(priv->proxy, G_MAXINT);

    /* connect signal receiver */
    g_signal_connect(priv->proxy,
		     "g-signal",
		     G_CALLBACK (_fw_config_service_signal_receiver),
		     NULL);

    /* do not terminate if connection has been closed */
    g_dbus_connection_set_exit_on_close(priv->connection, FALSE);
}

static void
fw_config_service_finalize(GObject *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_reset_error(priv);

    /* disconnect */
    /***/

    G_OBJECT_CLASS(fw_config_service_parent_class)->finalize(obj);
}

static void
fw_config_service_set_property(GObject      *obj,
			       guint         property_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

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
fw_config_service_get_property(GObject *obj,
			       guint property_id,
			       GValue *value,
			       GParamSpec *pspec)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

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
fw_config_service_class_init(FWConfigServiceClass *fw_config_service_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_config_service_class);

#ifdef FW_DEBUG
    g_print("fw_config_service_class_init\n");
#endif
    
/*
    GParamSpec *param_spec = NULL;

    obj_class->get_property = fw_config_service_get_property;
    obj_class->set_property = fw_config_service_set_property;

    param_spec = g_param_spec_string("path",
				     "dbus path",
				     "Set dbus path",
				     "",
				     G_PARAM_CONSTRUCT_ONLY |
				     G_PARAM_READWRITE);
    g_object_class_install_property(obj_class, PROP_PATH, param_spec);
*/

    obj_class->finalize = fw_config_service_finalize;

    g_type_class_add_private(obj_class, sizeof(FWConfigServicePrivate));
}

GVariant *
_fw_config_service_proxy_call_sync(FWConfigServicePrivate *priv,
				   GDBusProxy *proxy,
				   const gchar *method_name,
				   GVariant *parameters)
{
    GVariant *result;

    _fw_config_service_reset_error(priv);

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
_fw_config_service_proxy_call_sync_get_str(FWConfigServicePrivate *priv,
					   GDBusProxy *proxy,
					   const gchar *method_name,
					   GVariant *parameters)
{
    GVariant *variant;
    gchar *value_str;

    variant = _fw_config_service_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return NULL;
    }

    g_variant_get(variant, "(s)", &value_str);
    g_variant_unref(variant);

    return value_str;
}

gboolean
_fw_config_service_proxy_call_sync_get_bool(FWConfigServicePrivate *priv,
					    GDBusProxy *proxy,
					    const gchar *method_name,
					    GVariant *parameters)
{
    GVariant *variant;
    gboolean value;

    variant = _fw_config_service_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return FALSE;
    }

    g_variant_get(variant, "(b)", &value);
    g_variant_unref(variant);

    return value;
}

/* methods */

/**
 * fw_config_service_getSettings:
 *
 * Returns: (transfer none) (allow-none) (type FWService*)
 */
FWService *
fw_config_service_getSettings(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);
    GVariant *variant;
    FWService *hlpr = NULL;

    variant = _fw_config_service_proxy_call_sync(priv, priv->proxy,
						 "getSettings", NULL);

    if (priv->error != NULL)
       return hlpr;

    hlpr = fw_service_new_from_variant(variant);
    g_variant_unref(variant);

    return hlpr;
}

void
fw_config_service_update(FWConfigService *obj,
			 FWService *settings)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(
	priv, priv->proxy, "update",
	g_variant_new("(v)", fw_service_to_variant(settings)));
}

void
fw_config_service_loadDefaults(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "loadDefaults",
				       NULL);
}

void
fw_config_service_remove(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "remove", NULL);
}

void
fw_config_service_rename(FWConfigService *obj, gchar *name)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "rename",
				       g_variant_new("(s)", name));
}

/**
 * fw_config_service_getVersion:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_service_getVersion(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_str(priv, priv->proxy,
						      "getVersion", NULL);
}

void
fw_config_service_setVersion(FWConfigService *obj,
			    gchar *version)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "setVersion",
				       g_variant_new("(s)", version));
}

/**
 * fw_config_service_getShort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_service_getShort(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_str(priv, priv->proxy,
						      "getShort", NULL);
}

void
fw_config_service_setShort(FWConfigService *obj,
			   gchar *short_description)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "setShort",
				       g_variant_new("(s)", short_description));
}

/**
 * fw_config_service_getDescription:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_service_getDescription(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_str(priv, priv->proxy,
						      "getDescription", NULL);
}

void
fw_config_service_setDescription(FWConfigService *obj,
				gchar *description)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "setDescription",
				       g_variant_new("(s)", description));
}

/**
 * fw_config_service_getPorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_config_service_getPorts(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_service_proxy_call_sync(priv, priv->proxy,
						 "getPorts", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_port_list_new_from_variant(variant);
}

/**
 * fw_config_service_setPorts:
 * @obj: (type FWConfigService*): a FWConfigService instance
 * @ports: (type FWPortList*)
 */
void
fw_config_service_setPorts(FWConfigService *obj,
			   FWPortList *ports)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "setPorts",
				       fw_port_list_to_variant(ports));
}

void
fw_config_service_addPort(FWConfigService *obj,
			  gchar *port,
			  gchar *protocol)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "addPort",
				       g_variant_new("(ss)", port, protocol));
}

void
fw_config_service_removePort(FWConfigService *obj,
			     gchar *port,
			     gchar *protocol)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "removePort",
				       g_variant_new("(ss)", port, protocol));
}

gboolean
fw_config_service_queryPort(FWConfigService *obj,
			    gchar *port,
			    gchar *protocol)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryPort",
	g_variant_new("(ss)", port, protocol));
}

void
fw_config_service_add_port(FWConfigService *obj,
			   FWPort *port)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "addPort",
				       fw_port_to_variant(port));
}

void
fw_config_service_remove_port(FWConfigService *obj,
			      FWPort *port)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "removePort",
				       fw_port_to_variant(port));
}

gboolean
fw_config_service_query_port(FWConfigService *obj,
			     FWPort *port)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryPort",
	fw_port_to_variant(port));
}

/**
 * fw_config_service_getProtocols:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_service_getProtocols(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_service_proxy_call_sync(priv, priv->proxy,
						 "getProtocols", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_str_list_new_from_variant(variant);
}

/**
 * fw_config_service_setProtocols:
 * @obj: (type FWConfigService*): a FWConfigService instance
 * @protocols: (type GList*) (element-type gchar*)
 */
void
fw_config_service_setProtocols(FWConfigService *obj,
			       GList *protocols)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "setProtocols",
				       fw_str_list_to_variant(protocols));
}

void
fw_config_service_addProtocol(FWConfigService *obj,
			      gchar *protocol)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "addProtocol",
				       g_variant_new("(s)", protocol));
}

void
fw_config_service_removeProtocol(FWConfigService *obj,
				 gchar *protocol)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "removeProtocol",
				       g_variant_new("(s)", protocol));
}

gboolean
fw_config_service_queryProtocol(FWConfigService *obj,
				gchar *protocol)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryProtocol",
	g_variant_new("(s)", protocol));
}

/**
 * fw_config_service_getSourcePorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_config_service_getSourcePorts(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_service_proxy_call_sync(priv, priv->proxy,
						 "getSourcePorts", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_port_list_new_from_variant(variant);
}

/**
 * fw_config_service_setSourcePorts:
 * @obj: (type FWConfigService*): a FWConfigService instance
 * @ports: (type FWPortList*)
 */
void
fw_config_service_setSourcePorts(FWConfigService *obj,
				 FWPortList *ports)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "setSourcePorts",
				       fw_port_list_to_variant(ports));
}

void
fw_config_service_addSourcePort(FWConfigService *obj,
				gchar *port,
				gchar *protocol)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "addSourcePort",
				       g_variant_new("(ss)", port, protocol));
}

void
fw_config_service_removeSourcePort(FWConfigService *obj,
				   gchar *port,
				   gchar *protocol)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "removeSourcePort",
				       g_variant_new("(ss)", port, protocol));
}

gboolean
fw_config_service_querySourcePort(FWConfigService *obj,
				  gchar *port,
				  gchar *protocol)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_bool(
	priv, priv->proxy, "querySourcePort",
	g_variant_new("(ss)", port, protocol));
}

void
fw_config_service_add_source_port(FWConfigService *obj,
				  FWPort *port)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "addSourcePort",
				       fw_port_to_variant(port));
}

void
fw_config_service_remove_source_port(FWConfigService *obj,
				     FWPort *port)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "removeSourcePort",
				       fw_port_to_variant(port));
}

gboolean
fw_config_service_query_source_port(FWConfigService *obj,
				    FWPort *port)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_bool(
	priv, priv->proxy, "querySourcePort",
	fw_port_to_variant(port));
}

/**
 * fw_config_service_getModules:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_service_getModules(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_service_proxy_call_sync(priv, priv->proxy,
						 "getModules", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_str_list_new_from_variant(variant);
}

/**
 * fw_config_service_setModules:
 * @obj: (type FWConfigService*): a FWConfigService instance
 * @modules: (type GList*) (element-type gchar*)
 */
void
fw_config_service_setModules(FWConfigService *obj,
			     GList *modules)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "setModules",
				       fw_str_list_to_variant(modules));
}

void
fw_config_service_addModule(FWConfigService *obj,
			      gchar *module)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "addModule",
				       g_variant_new("(s)", module));
}

void
fw_config_service_removeModule(FWConfigService *obj,
			       gchar *module)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "removeModule",
				       g_variant_new("(s)", module));
}

gboolean
fw_config_service_queryModule(FWConfigService *obj,
			      gchar *module)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryModule",
	g_variant_new("(s)", module));
}

/**
 * fw_config_service_getDestinations:
 *
 * Returns: (transfer none) (allow-none) (type GHashTable*) (element-type gchar* gchar*)
 */
GHashTable *
fw_config_service_getDestinations(FWConfigService *obj)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);
    GVariant *variant;
    GVariantIter iter;

    variant = _fw_config_service_proxy_call_sync(priv, priv->proxy,
						 "getDestinations", NULL);

    if (priv->error != NULL)
	return NULL;

    if (strncmp(g_variant_get_type_string(variant), "(a{ss})", 7) != 0)
	return NULL;

    GHashTable *hash = g_hash_table_new_full(g_str_hash, g_str_equal,
					     g_free, g_object_unref);
    if (g_variant_iter_init(&iter, variant)) {
	GVariant *item;

	while ((item = g_variant_iter_next_value(&iter)) != NULL) {
	    GVariant *item2=NULL;
	    gchar *key, *value;

	    item2 = g_variant_get_child_value(item, 0);
	    g_variant_get(item2, "s", &key);
	    g_variant_unref(item2);

	    item2 = g_variant_get_child_value(item, 1);
	    g_variant_get(item2, "s", &value);
	    g_variant_unref(item2);

	    g_hash_table_insert(hash, key, value);

	    g_variant_unref(item);
	}
    }

    return hash;
}

void
fw_config_service_setDestinations(FWConfigService *obj,
				  GHashTable *destinations)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);
    GVariantBuilder builder;
    GHashTableIter iter;
    gpointer key, value;

    g_variant_builder_init(&builder, G_VARIANT_TYPE("a{ss}"));
    g_hash_table_iter_init(&iter, destinations);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
	g_variant_builder_add(&builder, "{ss}", key, value);
    }

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "setDestinations",
				       g_variant_new("(a{ss})", &builder));
}

void
fw_config_service_setDestination(FWConfigService *obj,
				 gchar *ipv,
				 gchar *address)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "setDestination",
				       g_variant_new("(ss)", ipv, address));
}

void
fw_config_service_removeDestination(FWConfigService *obj,
				    gchar *ipv)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    _fw_config_service_proxy_call_sync(priv, priv->proxy, "removeDestination",
				       g_variant_new("(s)", ipv));
}

gboolean
fw_config_service_queryDestination(FWConfigService *obj,
				   gchar *ipv,
				   gchar *address)
{
    FWConfigServicePrivate *priv = FW_CONFIG_SERVICE_GET_PRIVATE(obj);

    return _fw_config_service_proxy_call_sync_get_bool(
        priv, priv->proxy, "queryDestination",
        g_variant_new("(ss)", ipv, address));
}
