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
#include "fw_config_ipset.h"

G_DEFINE_TYPE(FWConfigIPSet, fw_config_ipset, G_TYPE_OBJECT);

#define FW_CONFIG_IPSET_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_CONFIG_IPSET_TYPE, FWConfigIPSetPrivate))

typedef struct {
    GDBusConnection *connection;
    GDBusProxy *proxy;
    GError *error;

    gchar *path;
} FWConfigIPSetPrivate;

enum _FWConfigIPSetProperties {
    PROP_0,
    PROP_PATH,
    PROP_BUILTIN,
    PROP_DEFAULT,
    PROP_FILENAME,
    PROP_NAME,
    N_PROPERTIES,
};

static void _fw_config_ipset_dbus_connect(FWConfigIPSetPrivate *fw);

FWConfigIPSet *
fw_config_ipset_new(gchar *path)
{
    FWConfigIPSet *obj = g_object_new(FW_CONFIG_IPSET_TYPE, NULL);
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);
    priv->path = g_strdup(path);

    _fw_config_ipset_dbus_connect(priv);

    return obj;
    /*
      return g_object_new(FW_CONFIG_IPSET_TYPE,
			"path", path,
			NULL);
    */
}

static void
fw_config_ipset_init(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

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
_fw_config_ipset_reset_error(FWConfigIPSetPrivate *priv)
{
    if (priv->error != NULL) {
	g_error_free(priv->error);
	priv->error = NULL;
    }
}

static void
_fw_config_ipset_signal_receiver(GDBusProxy *proxy,
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
_fw_config_ipset_dbus_connect(FWConfigIPSetPrivate *priv)
{
    _fw_config_ipset_reset_error(priv);

    /* create syncroneous proxies */
    priv->proxy = g_dbus_proxy_new_sync(priv->connection,
					G_DBUS_PROXY_FLAGS_NONE,
					NULL,
					FW_DBUS_NAME,
					priv->path,
					FW_DBUS_INTERFACE_CONFIG_IPSET,
					NULL,
					&priv->error);

    /* G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START |
       G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES */

    g_assert(priv->proxy != NULL);
    g_dbus_proxy_set_default_timeout(priv->proxy, G_MAXINT);

    /* connect signal receiver */
    g_signal_connect(priv->proxy,
		     "g-signal",
		     G_CALLBACK (_fw_config_ipset_signal_receiver),
		     NULL);

    /* do not terminate if connection has been closed */
    g_dbus_connection_set_exit_on_close(priv->connection, FALSE);
}

static void
fw_config_ipset_finalize(GObject *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_reset_error(priv);

    /* disconnect */
    /***/

    G_OBJECT_CLASS(fw_config_ipset_parent_class)->finalize(obj);
}

static void
fw_config_ipset_set_property(GObject      *obj,
			     guint         property_id,
			     const GValue *value,
			     GParamSpec   *pspec)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

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
fw_config_ipset_get_property(GObject *obj,
			     guint property_id,
			     GValue *value,
			     GParamSpec *pspec)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

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
fw_config_ipset_class_init(FWConfigIPSetClass *fw_config_ipset_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_config_ipset_class);

#ifdef FW_DEBUG
    g_print("fw_config_ipset_class_init\n");
#endif

/*
    GParamSpec *param_spec = NULL;

    obj_class->get_property = fw_config_ipset_get_property;
    obj_class->set_property = fw_config_ipset_set_property;

    param_spec = g_param_spec_string("path",
				     "dbus path",
				     "Set dbus path",
				     "",
				     G_PARAM_CONSTRUCT_ONLY |
				     G_PARAM_READWRITE);
    g_object_class_install_property(obj_class, PROP_PATH, param_spec);
*/

    obj_class->finalize = fw_config_ipset_finalize;

    g_type_class_add_private(obj_class, sizeof(FWConfigIPSetPrivate));
}

GVariant *
_fw_config_ipset_proxy_call_sync(FWConfigIPSetPrivate *priv,
				 GDBusProxy *proxy,
				 const gchar *method_name,
				 GVariant *parameters)
{
    GVariant *result;

    _fw_config_ipset_reset_error(priv);

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
_fw_config_ipset_proxy_call_sync_get_str(FWConfigIPSetPrivate *priv,
					 GDBusProxy *proxy,
					 const gchar *method_name,
					 GVariant *parameters)
{
    GVariant *variant;
    gchar *value_str;

    variant = _fw_config_ipset_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return NULL;
    }

    g_variant_get(variant, "(s)", &value_str);
    g_variant_unref(variant);

    return value_str;
}

gboolean
_fw_config_ipset_proxy_call_sync_get_bool(FWConfigIPSetPrivate *priv,
					  GDBusProxy *proxy,
					  const gchar *method_name,
					  GVariant *parameters)
{
    GVariant *variant;
    gboolean value;

    variant = _fw_config_ipset_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return FALSE;
    }

    g_variant_get(variant, "(b)", &value);
    g_variant_unref(variant);

    return value;
}

/* methods */

/**
 * fw_config_ipset_getSettings:
 *
 * Returns: (transfer none) (allow-none) (type FWIPSet*)
 */
FWIPSet *
fw_config_ipset_getSettings(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);
    GVariant *variant;
    FWIPSet *hlpr = NULL;

    variant = _fw_config_ipset_proxy_call_sync(priv, priv->proxy,
					       "getSettings", NULL);

    if (priv->error != NULL)
       return hlpr;

    hlpr = fw_ipset_new_from_variant(variant);
    g_variant_unref(variant);

    return hlpr;
}

void
fw_config_ipset_update(FWConfigIPSet *obj,
		       FWIPSet *settings)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(
	priv, priv->proxy, "update",
	g_variant_new("(v)", fw_ipset_to_variant(settings)));
}

void
fw_config_ipset_loadDefaults(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "loadDefaults",
				     NULL);
}

void
fw_config_ipset_remove(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "remove", NULL);
}

void
fw_config_ipset_rename(FWConfigIPSet *obj, gchar *name)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "rename",
				     g_variant_new("(s)", name));
}

/**
 * fw_config_ipset_getVersion:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_ipset_getVersion(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    return _fw_config_ipset_proxy_call_sync_get_str(priv, priv->proxy,
						    "getVersion", NULL);
}

void
fw_config_ipset_setVersion(FWConfigIPSet *obj,
			   gchar *version)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "setVersion",
				     g_variant_new("(s)", version));
}

/**
 * fw_config_ipset_getShort:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_ipset_getShort(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    return _fw_config_ipset_proxy_call_sync_get_str(priv, priv->proxy,
						    "getShort", NULL);
}

void
fw_config_ipset_setShort(FWConfigIPSet *obj,
			 gchar *short_description)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "setShort",
				     g_variant_new("(s)", short_description));
}

/**
 * fw_config_ipset_getDescription:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_ipset_getDescription(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    return _fw_config_ipset_proxy_call_sync_get_str(priv, priv->proxy,
						    "getDescription", NULL);
}

void
fw_config_ipset_setDescription(FWConfigIPSet *obj,
			       gchar *description)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "setDescription",
				     g_variant_new("(s)", description));
}

/**
 * fw_config_ipset_getType:
 *
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
gchar *
fw_config_ipset_getType(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    return _fw_config_ipset_proxy_call_sync_get_str(priv, priv->proxy,
						    "getType", NULL);
}

void
fw_config_ipset_setType(FWConfigIPSet *obj,
			gchar *type)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "setType",
				     g_variant_new("(s)", type));
}

/**
 * fw_config_ipset_getOptions:
 *
 * Returns: (transfer none) (allow-none) (type GHashTable*) (element-type gchar* gchar*)
 */
GHashTable *
fw_config_ipset_getOptions(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);
    GVariant *variant;
    GVariantIter iter;

    variant = _fw_config_ipset_proxy_call_sync(priv, priv->proxy,
					       "getOptions", NULL);

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
fw_config_ipset_setOptions(FWConfigIPSet *obj,
			   GHashTable *options)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);
    GVariantBuilder builder;
    GHashTableIter iter;
    gpointer key, value;

    g_variant_builder_init(&builder, G_VARIANT_TYPE("a{ss}"));
    g_hash_table_iter_init(&iter, options);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
	g_variant_builder_add(&builder, "{ss}", key, value);
    }

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "setOptions",
				     g_variant_new("(a{ss})", &builder));
}

void
fw_config_ipset_setOption(FWConfigIPSet *obj,
			  gchar *ipv,
			  gchar *address)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "setOption",
				     g_variant_new("(ss)", ipv, address));
}

void
fw_config_ipset_removeOption(FWConfigIPSet *obj,
			     gchar *ipv)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "removeOption",
				     g_variant_new("(s)", ipv));
}

gboolean
fw_config_ipset_queryOption(FWConfigIPSet *obj,
			    gchar *ipv,
			    gchar *address)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    return _fw_config_ipset_proxy_call_sync_get_bool(
        priv, priv->proxy, "queryOption",
        g_variant_new("(ss)", ipv, address));
}

/**
 * fw_config_ipset_getEntries:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_ipset_getEntries(FWConfigIPSet *obj)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);
    GVariant *variant;

    variant = _fw_config_ipset_proxy_call_sync(priv, priv->proxy,
					       "getEntries", NULL);

    if (priv->error != NULL)
	return NULL;

    return fw_str_list_new_from_variant(variant);
}

/**
 * fw_config_ipset_setEntries:
 * @obj: (type FWConfigIPSet*): a FWConfigIPSet instance
 * @entries: (type GList*) (element-type gchar*)
 */
void
fw_config_ipset_setEntries(FWConfigIPSet *obj,
			       GList *entries)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "setEntries",
				     fw_str_list_to_variant(entries));
}

void
fw_config_ipset_addEntry(FWConfigIPSet *obj,
			 gchar *entry)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "addEntry",
				     g_variant_new("(s)", entry));
}

void
fw_config_ipset_removeEntry(FWConfigIPSet *obj,
			    gchar *entry)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    _fw_config_ipset_proxy_call_sync(priv, priv->proxy, "removeEntry",
				     g_variant_new("(s)", entry));
}

gboolean
fw_config_ipset_queryEntry(FWConfigIPSet *obj,
			   gchar *entry)
{
    FWConfigIPSetPrivate *priv = FW_CONFIG_IPSET_GET_PRIVATE(obj);

    return _fw_config_ipset_proxy_call_sync_get_bool(
	priv, priv->proxy, "queryEntry",
	g_variant_new("(s)", entry));
}
