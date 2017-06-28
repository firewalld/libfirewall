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
#include "fw_config_helper.h"

G_DEFINE_TYPE(FWConfigHelper, fw_config_helper, G_TYPE_OBJECT);

#define FW_CONFIG_HELPER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_CONFIG_HELPER_TYPE, FWConfigHelperPrivate))

typedef struct {
    GDBusConnection *connection;
    GDBusProxy *proxy;
    GError *error;

    gchar *path;
} FWConfigHelperPrivate;

enum _FWConfigHelperProperties {
    PROP_0,
    PROP_PATH,
    PROP_BUILTIN,
    PROP_DEFAULT,
    PROP_FILENAME,
    PROP_NAME,
    N_PROPERTIES,
};

static void _fw_config_helper_dbus_connect(FWConfigHelperPrivate *fw);

FWConfigHelper *
fw_config_helper_new(gchar *path)
{
    FWConfigHelper *obj = g_object_new(FW_CONFIG_HELPER_TYPE, NULL);
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);
    priv->path = g_strdup(path);

    _fw_config_helper_dbus_connect(priv);

    return obj;
    /*
      return g_object_new(FW_CONFIG_HELPER_TYPE,
			"path", path,
			NULL);
    */
}

static void
fw_config_helper_init(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

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
_fw_config_helper_reset_error(FWConfigHelperPrivate *priv)
{
    if (priv->error != NULL) {
	g_error_free(priv->error);
	priv->error = NULL;
    }
}

static void
_fw_config_helper_signal_receiver(GDBusProxy *proxy,
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
_fw_config_helper_dbus_connect(FWConfigHelperPrivate *priv)
{
    _fw_config_helper_reset_error(priv);

    /* create syncroneous proxies */
    priv->proxy = g_dbus_proxy_new_sync(priv->connection,
					G_DBUS_PROXY_FLAGS_NONE,
					NULL,
					FW_DBUS_NAME,
					priv->path,
					FW_DBUS_INTERFACE_CONFIG_HELPER,
					NULL,
					&priv->error);

    /* G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START |
       G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES */

    g_assert(priv->proxy != NULL);
    g_dbus_proxy_set_default_timeout(priv->proxy, G_MAXINT);

    /* connect signal receiver */
    g_signal_connect(priv->proxy,
		     "g-signal",
		     G_CALLBACK (_fw_config_helper_signal_receiver),
		     NULL);

    /* do not terminate if connection has been closed */
    g_dbus_connection_set_exit_on_close(priv->connection, FALSE);
}

static void
fw_config_helper_finalize(GObject *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_reset_error(priv);

    /* disconnect */
    /***/

    G_OBJECT_CLASS(fw_config_helper_parent_class)->finalize(obj);
}

static void
fw_config_helper_set_property(GObject      *obj,
			      guint         property_id,
			      const GValue *value,
			      GParamSpec   *pspec)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

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
fw_config_helper_get_property(GObject *obj,
			      guint property_id,
			      GValue *value,
			      GParamSpec *pspec)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

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
fw_config_helper_class_init(FWConfigHelperClass *fw_config_helper_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_config_helper_class);

#ifdef FW_DEBUG
    g_print("fw_config_helper_class_init\n");
#endif

/*
    GParamSpec *param_spec = NULL;

    obj_class->get_property = fw_config_helper_get_property;
    obj_class->set_property = fw_config_helper_set_property;

    param_spec = g_param_spec_string("path",
				     "dbus path",
				     "Set dbus path",
				     "",
				     G_PARAM_CONSTRUCT_ONLY |
				     G_PARAM_READWRITE);
    g_object_class_install_property(obj_class, PROP_PATH, param_spec);
*/

    obj_class->finalize = fw_config_helper_finalize;

    g_type_class_add_private(obj_class, sizeof(FWConfigHelperPrivate));
}

GVariant *
_fw_config_helper_proxy_call_sync(FWConfigHelperPrivate *priv,
				  GDBusProxy *proxy,
				  const gchar *method_name,
				  GVariant *parameters)
{
    GVariant *result;

    _fw_config_helper_reset_error(priv);

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
_fw_config_helper_proxy_call_sync_get_str(FWConfigHelperPrivate *priv,
					  GDBusProxy *proxy,
					  const gchar *method_name,
					  GVariant *parameters)
{
    GVariant *variant;
    gchar *value_str;

    variant = _fw_config_helper_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return NULL;
    }

    g_variant_get(variant, "(s)", &value_str);
    g_variant_unref(variant);

    return value_str;
}

gboolean
_fw_config_helper_proxy_call_sync_get_bool(FWConfigHelperPrivate *priv,
					   GDBusProxy *proxy,
					   const gchar *method_name,
					   GVariant *parameters)
{
    GVariant *variant;
    gboolean value;

    variant = _fw_config_helper_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return FALSE;
    }

    g_variant_get(variant, "(b)", &value);
    g_variant_unref(variant);

    return value;
}

/* methods */

/**
 * fw_config_helper_getSettings:
 *
 * Returns: (transfer none) (allow-none) (type FWHelper*)
 */
FWHelper *
fw_config_helper_getSettings(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);
    GVariant *variant;
    FWHelper *hlpr = NULL;

    variant = _fw_config_helper_proxy_call_sync(priv, priv->proxy,
						"getSettings", NULL);

    if (priv->error != NULL)
       return hlpr;

    hlpr = fw_helper_new_from_variant(variant);
    g_variant_unref(variant);

    return hlpr;
}

void
fw_config_helper_update(FWConfigHelper *obj,
			FWHelper *settings)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(
	priv, priv->proxy, "update",
	g_variant_new("(v)", fw_helper_to_variant(settings)));
}

void
fw_config_helper_loadDefaults(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "loadDefaults",
				      NULL);
}

void
fw_config_helper_remove(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "remove", NULL);
}

void
fw_config_helper_rename(FWConfigHelper *obj, gchar *name)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "rename",
				      g_variant_new("(s)", name));
}

/**
 * fw_config_helper_getVersion:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_helper_getVersion(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    return _fw_config_helper_proxy_call_sync_get_str(priv, priv->proxy,
						     "getVersion", NULL);
}

void
fw_config_helper_setVersion(FWConfigHelper *obj,
			    gchar *version)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "setVersion",
				      g_variant_new("(s)", version));
}

/**
 * fw_config_helper_getShort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_helper_getShort(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    return _fw_config_helper_proxy_call_sync_get_str(priv, priv->proxy,
						     "getShort", NULL);
}

void
fw_config_helper_setShort(FWConfigHelper *obj,
			  gchar *short_description)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "setShort",
				      g_variant_new("(s)", short_description));
}

/**
 * fw_config_helper_getDescription:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_helper_getDescription(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    return _fw_config_helper_proxy_call_sync_get_str(priv, priv->proxy,
						     "getDescription", NULL);
}

void
fw_config_helper_setDescription(FWConfigHelper *obj,
				gchar *description)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "setDescription",
				      g_variant_new("(s)", description));
}

/**
 * fw_config_helper_getFamily:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_helper_getFamily(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    return _fw_config_helper_proxy_call_sync_get_str(priv, priv->proxy,
						     "getFamily", NULL);
}

void
fw_config_helper_setFamily(FWConfigHelper *obj,
			   gchar *family)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "setFamily",
				      g_variant_new("(s)", family));
}

/**
 * fw_config_helper_getModule:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_helper_getModule(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    return _fw_config_helper_proxy_call_sync_get_str(priv, priv->proxy,
						     "getModule", NULL);
}

void
fw_config_helper_setModule(FWConfigHelper *obj,
			   gchar *module)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "setModule",
				      g_variant_new("(s)", module));
}

/**
 * fw_config_helper_getPorts:
 *
 * Returns: (transfer none) (allow-none) (type FWPortList*)
 */
FWPortList *
fw_config_helper_getPorts(FWConfigHelper *obj)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_helper_proxy_call_sync(priv, priv->proxy,
						"getPorts", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_port_list_new_from_variant(variant);
}

/**
 * fw_config_helper_setPorts:
 * @obj: (type FWConfigHelper*): a FWConfigHelper instance
 * @ports: (type FWPortList*)
 */
void
fw_config_helper_setPorts(FWConfigHelper *obj,
			  FWPortList *ports)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "setPorts",
				      fw_port_list_to_variant(ports));
}

void
fw_config_helper_addPort(FWConfigHelper *obj,
			 gchar *port,
			 gchar *protocol)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "addPort",
				      g_variant_new("(ss)", port, protocol));
}

void
fw_config_helper_removePort(FWConfigHelper *obj,
			    gchar *port,
			    gchar *protocol)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    _fw_config_helper_proxy_call_sync(priv, priv->proxy, "removePort",
				      g_variant_new("(ss)", port, protocol));
}

gboolean
fw_config_helper_queryPort(FWConfigHelper *obj,
			   gchar *port,
			   gchar *protocol)
{
    FWConfigHelperPrivate *priv = FW_CONFIG_HELPER_GET_PRIVATE(obj);

    return _fw_config_helper_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryPort",
	g_variant_new("(ss)", port, protocol));
}
