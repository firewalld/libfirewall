/*
 * Copyright (C) 2014-2017 Red Hat, Inc.
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
#include "fw_client.h"
#include "fw_functions.h"
#include "fw_zone.h"
#include "fw_active_zone.h"
#include "fw_port.h"
#include "fw_forward_port.h"
#include "fw_direct_simple_rule.h"
#include "fw_direct_rule.h"
#include "fw_args.h"
#include "fw_passthrough.h"
#include "fw_types.h"
#include "fw_ipset.h"

G_DEFINE_TYPE(FWClient, fw_client, G_TYPE_OBJECT);

#define FW_CLIENT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_CLIENT_TYPE, FWClientPrivate))

typedef struct {
    /* dbus */
    GDBusConnection *connection;
    GDBusProxy *proxy;
    GDBusProxy *zone_proxy;
    GDBusProxy *ipset_proxy;
    GDBusProxy *direct_proxy;
    GDBusProxy *policies_proxy;
    GDBusProxy *properties_proxy;
    GError *error;
    
    FWConfig *config;

    /* properties */
    gboolean quiet;
    gboolean connected;
} FWClientPrivate;

enum {
    PROP_0,
    PROP_VERSION,
    PROP_INTERFACE_VERSION,
    PROP_STATE,
    PROP_IPV4,
    PROP_IPV6,
    PROP_IPV6_RPFILTER,
    PROP_BRIDGE,
    PROP_IPSET,
    PROP_IPSETTYPES,
    PROP_NF_CONNTRACK_HELPER_SETTING,
    PROP_NF_CONNTRACK_HELPERS,
    PROP_IPV4_ICMPTYPES,
    PROP_IPV6_ICMPTYPES,
    N_PROPERTIES,
};

/* static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, }; */

FWClient *
fw_client_new()
{
    return g_object_new(FW_CLIENT_TYPE, NULL);
}

static void _fw_client_dbus_connect(FWClientPrivate *priv);

static void
fw_client_init(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    /* init vars */
    priv->connection = NULL;
    priv->proxy = NULL;
    priv->zone_proxy = NULL;
    priv->ipset_proxy = NULL;
    priv->direct_proxy = NULL;
    priv->policies_proxy = NULL;
    priv->properties_proxy = NULL;
    priv->error = NULL;

    priv->config = NULL;

    priv->quiet = FALSE;
    priv->connected = FALSE;
    
    /* connect to system dbus */
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

    _fw_client_dbus_connect(priv);
}

static void
_fw_client_reset_error(FWClientPrivate *priv)
{
    if (priv->error != NULL) {
	g_error_free(priv->error);
	priv->error = NULL;
    }
}



static void
_fw_client_signal_receiver(GDBusProxy *proxy,
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
_fw_client_dbus_connect(FWClientPrivate *priv)
{
    _fw_client_reset_error(priv);

    /* create syncroneous proxies */
    priv->proxy = g_dbus_proxy_new_sync(priv->connection,
				      G_DBUS_PROXY_FLAGS_NONE,
				      NULL,
				      FW_DBUS_NAME,
				      FW_DBUS_PATH,
				      FW_DBUS_INTERFACE,
				      NULL,
				      &priv->error);

    /* G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START |
       G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES */

    g_assert(priv->proxy != NULL);
    g_dbus_proxy_set_default_timeout(priv->proxy, G_MAXINT);

    /* connect signal receiver */
    g_signal_connect(priv->proxy,
		     "g-signal",
		     G_CALLBACK (_fw_client_signal_receiver),
		     NULL);

    priv->zone_proxy = g_dbus_proxy_new_sync(priv->connection,
					   G_DBUS_PROXY_FLAGS_NONE,
					   NULL,
					   FW_DBUS_NAME,
					   FW_DBUS_PATH,
					   FW_DBUS_INTERFACE_ZONE,
					   NULL,
					   &priv->error);

    g_assert(priv->zone_proxy != NULL);
    g_dbus_proxy_set_default_timeout(priv->zone_proxy, G_MAXINT);

    priv->ipset_proxy = g_dbus_proxy_new_sync(priv->connection,
					    G_DBUS_PROXY_FLAGS_NONE,
					    NULL,
					    FW_DBUS_NAME,
					    FW_DBUS_PATH,
					    FW_DBUS_INTERFACE_IPSET,
					    NULL,
					    &priv->error);

    g_assert(priv->ipset_proxy != NULL);
    g_dbus_proxy_set_default_timeout(priv->ipset_proxy, G_MAXINT);

    priv->direct_proxy = g_dbus_proxy_new_sync(priv->connection,
					     G_DBUS_PROXY_FLAGS_NONE,
					     NULL,
					     FW_DBUS_NAME,
					     FW_DBUS_PATH,
					     FW_DBUS_INTERFACE_DIRECT,
					     NULL,
					     &priv->error);

    g_assert(priv->direct_proxy != NULL);
    g_dbus_proxy_set_default_timeout(priv->direct_proxy, G_MAXINT);

    priv->policies_proxy = g_dbus_proxy_new_sync(priv->connection,
					       G_DBUS_PROXY_FLAGS_NONE,
					       NULL,
					       FW_DBUS_NAME,
					       FW_DBUS_PATH,
					       FW_DBUS_INTERFACE_POLICIES,
					       NULL,
					       &priv->error);

    g_assert(priv->policies_proxy != NULL);
    g_dbus_proxy_set_default_timeout(priv->policies_proxy, G_MAXINT);

    priv->connected = TRUE;

    /* do not terminate if connection has been closed */
    g_dbus_connection_set_exit_on_close(priv->connection, FALSE);
}

static void
fw_client_finalize(GObject *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_reset_error(priv);

    /* disconnect */
    /***/

    G_OBJECT_CLASS(fw_client_parent_class)->finalize(obj);
}

static void
fw_client_set_property(GObject      *obj,
		       guint         property_id,
		       const GValue *value,
		       GParamSpec   *pspec)
{
    switch (property_id) {
#ifdef TODO
    case PROP_CONNECTED:
	/* error readonly */
        break;

    case PROP_ERROR:
	/* error readonly */
        break;
#endif

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
        break;
    }
}

static void
fw_client_get_property(GObject *obj,
		       guint property_id,
		       GValue *value,
		       GParamSpec *pspec)
{
#ifdef TODO
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
#endif

    switch (property_id) {

#ifdef TODO
    case PROP_CONNECTED:
        /* connected */
        break;

    case PROP_ERROR:
        /* error */
	if (priv->error != NULL)
	    g_value_set_string(value, priv->error->message);
        break;
#endif

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
        break;
    }
}

static void
fw_client_class_init(FWClientClass *fw_client_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_client_class);

/*    obj_class->constructor = fw_client_constructor; */
    obj_class->set_property = fw_client_set_property;
    obj_class->get_property = fw_client_get_property;
    obj_class->finalize = fw_client_finalize;

    /* properties */

    /**
     * FWClient:version:
     *
     * The firewalld version.
     **/
    g_object_class_install_property(
	obj_class, PROP_VERSION,
	g_param_spec_string("version", "", "", NULL,
			    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_INTERFACE_VERSION,
	g_param_spec_string("interface-version", "", "", NULL,
			    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_STATE,
	g_param_spec_string("state", "", "", NULL,
			    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_IPV4,
	g_param_spec_boolean("IPv4", "", "", FALSE,
			    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_IPV4_ICMPTYPES,
	g_param_spec_boxed("IPv4ICMPTypes", "", "", G_TYPE_PTR_ARRAY,
			   G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_IPV6,
	g_param_spec_boolean("IPv6", "", "", FALSE,
			    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_IPV6_RPFILTER,
	g_param_spec_boolean("IPv6-rpfilter", "", "", FALSE,
			     G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_IPV6_ICMPTYPES,
	g_param_spec_boxed("IPv6ICMPTypes", "", "", G_TYPE_PTR_ARRAY,
			   G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_BRIDGE,
	g_param_spec_boolean("BRIDGE", "", "", FALSE,
			    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_IPSETTYPES,
	g_param_spec_boxed("IPSetTypes", "", "", G_TYPE_PTR_ARRAY,
			   G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_NF_CONNTRACK_HELPER_SETTING,
	g_param_spec_boolean("nf-conntrack-helper-setting", "", "", FALSE,
			    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property(
	obj_class, PROP_NF_CONNTRACK_HELPERS,
	g_param_spec_boxed("nf-conntrack-helpers", "", "", G_TYPE_PTR_ARRAY,
			   G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
    /*
     * initialize properties
     */
/*
    obj_properties[PROP_CONNECTED] = 
        g_param_spec_string("connected",
			    "connected",
			    "Connected to firewalld?",
			    "World",
			    G_PARAM_READABLE |
			    G_PARAM_CONSTRUCT);

    obj_properties[PROP_ERROR] = 
        g_param_spec_string("error",
			    "error",
			    "error message",
			    "World",
			    G_PARAM_READABLE |
			    G_PARAM_CONSTRUCT);

    g_object_class_install_properties(obj_class,
				      N_PROPERTIES,
				      obj_properties);
*/
    g_type_class_add_private(obj_class, sizeof(FWClientPrivate));
}

GVariant *
_fw_client_proxy_call_sync(FWClientPrivate *priv,
			   GDBusProxy *proxy,
			   const gchar *method_name,
			   GVariant *parameters)
{
    GVariant *result;

    _fw_client_reset_error(priv);

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

const gchar *
_fw_client_proxy_call_sync_get_str(FWClientPrivate *priv,
				   GDBusProxy *proxy,
				   const gchar *method_name,
				   GVariant *parameters)
{
    GVariant *variant;
    gchar *str;

    variant = _fw_client_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return NULL;
    }

    g_variant_get(variant, "(s)", &str);
    g_variant_unref(variant);

    return str;
}

gboolean
_fw_client_proxy_call_sync_get_bool(FWClientPrivate *priv,
				    GDBusProxy *proxy,
				    const gchar *method_name,
				    GVariant *parameters)
{
    GVariant *variant;
    gboolean value;

    variant = _fw_client_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return FALSE;
    }

    g_variant_get(variant, "(b)", &value);
    g_variant_unref(variant);

    return value;
}

GList *
_fw_client_proxy_call_sync_get_str_list(FWClientPrivate *priv,
					GDBusProxy *proxy,
					const gchar *method_name,
					GVariant *parameters)
{
    GVariant *variant, *element;
    GVariantIter iter;
    GList *list = NULL;
    gchar *str;

    variant = _fw_client_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return list;
    }

    if (strncmp(g_variant_get_type_string(variant), "(as)", 4) != 0)
    {
	return list;
    }

    /* get as from (as) */
    variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    g_variant_get(element, "s", &str);
	    list = g_list_append(list, g_strdup(str));
	    g_variant_unref(element);
	}
    }
    g_variant_unref(variant);

    return list;
}

GList *
_fw_client_proxy_call_sync_get_gint32_list(FWClientPrivate *priv,
					   GDBusProxy *proxy,
					   const gchar *method_name,
					   GVariant *parameters)
{
    GVariant *variant, *element;
    GVariantIter iter;
    GList *list = NULL;
    gint32 *i;

    variant = _fw_client_proxy_call_sync(priv, proxy, method_name, parameters);

    if (priv->error != NULL) {
	return list;
    }

    if (strncmp(g_variant_get_type_string(variant), "(ai)", 4) != 0)
    {
	return list;
    }

    /* get ai from (ai) */
    variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    g_variant_get(element, "i", &i);
	    list = g_list_append(list, i);
	    g_variant_unref(element);
	}
    }
    g_variant_unref(variant);

    return list;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/* authorize all */

void
fw_client_authorizeAll(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_proxy_call_sync(priv, priv->proxy, "authorizeAll", NULL);
}

/* config */

/**
 * fw_client_config:
 *
 * Returns: (transfer none) (allow-none) (type FWConfig*)
 */
FWConfig *
fw_client_config(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    if (priv->config == NULL)
	priv->config = fw_config_new();

    return priv->config;
}

/* reload */

void
fw_client_reload(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_proxy_call_sync(priv, priv->proxy, "reload", NULL);
}

void
fw_client_completeReload(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_proxy_call_sync(priv, priv->proxy, "completeReload", NULL);
}

/* runtime to permanent */

void
fw_client_runtimeToPermanent(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_proxy_call_sync(priv, priv->proxy, "runtimeToPermanent", NULL);
}

/* properties */

/* panic mode */

void
fw_client_enablePanicMode(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_proxy_call_sync(priv, priv->proxy, "enablePanicMode", NULL);
}

void
fw_client_disablePanicMode(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_proxy_call_sync(priv, priv->proxy, "disablePanicMode", NULL);
}

gboolean
fw_client_queryPanicMode(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->proxy,
					       "queryPanicMode", NULL);
}

/* list functions */

/**
 * fw_client_getZoneSettings:
 *
 * Returns: (transfer none) (allow-none) (type FWZone*)
 */
FWZone *
fw_client_getZoneSettings(FWClient *obj,
			  const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    FWZone *zne = NULL;

    variant = _fw_client_proxy_call_sync(priv, priv->proxy,
					 "getZoneSettings",
					 g_variant_new("(s)", zone));

    if (priv->error != NULL)
	return NULL;

    zne = fw_zone_new_from_variant(variant);
    g_variant_unref(variant);

    return zne;
}

/**
 * fw_client_listServices:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_listServices(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->proxy,
						   "listServices", NULL);
}

/**
 * fw_client_getServiceSettings:
 *
 * Returns: (transfer none) (allow-none) (type FWService*)
 */
FWService *
fw_client_getServiceSettings(FWClient *obj,
			     const gchar *service)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    FWService *srvc = NULL;

    variant = _fw_client_proxy_call_sync(priv, priv->proxy,
                                        "getServiceSettings",
                                        g_variant_new("(s)", service));

    if (priv->error != NULL)
       return NULL;

    srvc = fw_service_new_from_variant(variant);
    g_variant_unref(variant);

    return srvc;
}

/**
 * fw_client_listIPSets:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_listIPSets(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->ipset_proxy,
						   "getIPSets", NULL);
}

/**
 * fw_client_getIPSetSettings:
 *
 * Returns: (transfer none) (allow-none) (type FWIPSet*)
 */
FWIPSet *
fw_client_getIPSetSettings(FWClient *obj,
			   const gchar *ipset)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    FWIPSet *ipst = NULL;

#ifdef FW_DEBUG
    g_printerr("fw_client_getIPSetSettings('%s')\n", ipset);
#endif

    variant = _fw_client_proxy_call_sync(priv, priv->ipset_proxy,
                                        "getIPSetSettings",
					 g_variant_new("(s)", ipset));

    if (priv->error != NULL)
       return NULL;

    ipst = fw_ipset_new_from_variant(variant);
    g_variant_unref(variant);

    return ipst;
}

/**
 * fw_client_listIcmpTypes:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_listIcmpTypes(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->proxy,
						   "listIcmpTypes", NULL);
}

/**
 * fw_client_getIcmpTypeSettings:
 *
 * Returns: (transfer none) (allow-none) (type FWIcmpType*)
 */
FWIcmpType *
fw_client_getIcmpTypeSettings(FWClient *obj,
			      const gchar *icmptype)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    FWIcmpType *icmp = NULL;

    variant = _fw_client_proxy_call_sync(priv, priv->proxy,
                                        "getIcmpTypeSettings",
                                        g_variant_new("(s)", icmptype));

    if (priv->error != NULL)
	return NULL;

    icmp = fw_icmptype_new_from_variant(variant);
    g_variant_unref(variant);

    return icmp;
}

/**
 * fw_client_listHelpers:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_listHelpers(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->proxy,
						   "getHelpers", NULL);
}

/**
 * fw_client_getHelperSettings:
 *
 * Returns: (transfer none) (allow-none) (type FWHelper*)
 */
FWHelper *
fw_client_getHelperSettings(FWClient *obj,
			    const gchar *helper)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    FWHelper *hlpr = NULL;

    variant = _fw_client_proxy_call_sync(priv, priv->proxy,
                                        "getHelperSettings",
                                        g_variant_new("(s)", helper));

    if (priv->error != NULL)
       return NULL;

    hlpr = fw_helper_new_from_variant(variant);
    g_variant_unref(variant);

    return hlpr;
}

/* default zone */

const gchar*
fw_client_getDefaultZone(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->proxy,
					      "getDefaultZone", NULL);
}

void
fw_client_setDefaultZone(FWClient *obj, const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_proxy_call_sync(priv, priv->proxy, "setDefaultZone",
			       g_variant_new("(s)", zone));
}

/* zone */

/**
 * fw_client_getZones:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_getZones(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->zone_proxy,
						   "getZones", NULL);
}

/**
 * fw_client_getActiveZones:
 * @obj: (type FWClient*): a FWClient instance
 *
 * Gets active zones from firewalld and creates a GHashTable for the client.
 *
 * Returns: (transfer none) (allow-none) (element-type gchar* FWActiveZone*)
 */
GHashTable *
fw_client_getActiveZones(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    GVariantIter *iter1, *iter2, *iter3;
    GHashTable *active_zones = NULL;
    const gchar *zone_key;
    GVariant *key;
    GVariant *entry;

    variant = _fw_client_proxy_call_sync(priv, priv->zone_proxy, "getActiveZones",
					 NULL);

    if (priv->error != NULL) {
	return active_zones;
    }

    /* create active_zone return hash table */
    active_zones = g_hash_table_new_full(g_str_hash, g_str_equal,
					 g_free, g_object_unref);

    /* get a{sa{sas}} from (a{sa{sas}}) */
    variant = g_variant_get_child_value(variant, 0);

    g_variant_get(variant, "a{sa{sas}}", &iter1);
    while ((entry = g_variant_iter_next_value(iter1))) {
	GVariant *value, *value2;
	GVariant *entry2;
	FWActiveZone *active_zone = fw_active_zone_new();

	key = g_variant_get_child_value(entry, 0);
	zone_key = g_variant_get_string(key, NULL);
	g_variant_unref(key);

	value = g_variant_get_child_value(entry, 1);
	g_variant_unref(entry);

	g_variant_get(value, "a{sas}", &iter2);
	while ((entry2 = g_variant_iter_next_value(iter2))) {
	    GList *list = NULL;
	    const gchar *entry_key;
	    gchar *str;

	    key = g_variant_get_child_value(entry2, 0);
	    entry_key = g_variant_get_string(key, NULL);
	    g_variant_unref(key);

	    value2 = g_variant_get_child_value(entry2, 1);
	    g_variant_unref(entry2);

	    g_variant_get(value2, "as", &iter3);
	    while (g_variant_iter_loop(iter3, "s", &str)) {
		list = g_list_append(list, g_strdup(str));
	    }

	    if (strncmp(entry_key, "interfaces", 10) == 0) {
		fw_active_zone_setInterfaces(active_zone, list);
	    }
	    if (strncmp(entry_key, "sources", 7) == 0) {
	        fw_active_zone_setSources(active_zone, list);
	    }

	    g_variant_unref(value2);
	}

	g_hash_table_insert(active_zones, (gchar *) zone_key,
			    (gpointer)active_zone);
    }

    g_variant_unref(variant);

    return active_zones;
}

/**
 * fw_client_getZoneOfInterface:
 * @obj: (type FWClient*): a FWClient instance
 * @interface: (type gchar*)
 *
 * Gets zone, the interface is bound to.
 *
 * Returns: (transfer none) (type gchar*)
 */
const gchar*
fw_client_getZoneOfInterface(FWClient *obj,
			     const gchar* interface)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "getZoneOfInterface",
					      g_variant_new("(s)", interface));
}

/**
 * fw_client_getZoneOfSource:
 * @obj: (type FWClient*): a FWClient instance
 * @source: (type gchar*)
 *
 * Gets zone, the interface is bound to.
 *
 * Returns: (transfer none) (type gchar*)
 */
const gchar*
fw_client_getZoneOfSource(FWClient *obj,
			  const gchar* source)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "getZoneOfSource",
					      g_variant_new("(s)", source));
}

/* interfaces */

/**
 * fw_client_addInterface:
 * @obj: (type FWClient*): a FWClient instance
 * @zone: (type gchar*)
 * @interface: (type gchar*)
 *
 * Bind interface to zone.
 *
 * Returns: (transfer none) (type gchar*)
 */
const gchar *
fw_client_addInterface(FWClient *obj,
		       const gchar *zone,
		       const gchar *interface)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addInterface",
					      g_variant_new("(ss)", zone,
							    interface));
}

const gchar *
fw_client_changeZoneOfInterface(FWClient *obj,
				const gchar *zone,
				const gchar *interface)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "changeZoneOfInterface",
					      g_variant_new("(ss)", zone,
							    interface));
}

gboolean
fw_client_queryInterface(FWClient *obj,
			 const gchar *zone,
			 const gchar *interface)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "queryInterface",
					       g_variant_new("(ss)", zone,
							     interface));
}

/**
 * fw_client_removeInterface:
 * @obj: (type FWClient*): a FWClient instance
 * @zone: (type gchar*)
 * @interface: (type gchar*)
 *
 * Remove binding of interface from zone.
 *
 * Returns: (transfer none) (type gchar*)
 */
const gchar *
fw_client_removeInterface(FWClient *obj,
			  const gchar *zone,
			  const gchar *interface)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removeInterface",
					      g_variant_new("(ss)", zone,
							    interface));
}

/**
 * fw_client_getInterfaces:
 * @obj: (type FWClient*): a FWClient instance
 * @zone: (type gchar*)
 *
 * Returns interfaces bound to zone
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_getInterfaces(FWClient *obj,
			const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->zone_proxy,
						   "getInterfaces",
						   g_variant_new("(s)", zone));
}

/* sources */

/**
 * fw_client_addSource:
 * @obj: (type FWClient*): a FWClient instance
 * @zone: (type gchar*)
 * @source: (type gchar*)
 *
 * Bind source to zone.
 *
 * Returns: (transfer none) (type gchar*)
 */
const gchar *
fw_client_addSource(FWClient *obj,
		    const gchar *zone,
		    const gchar *source)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addSource",
					      g_variant_new("(ss)", zone,
							    source));
}

const gchar *
fw_client_changeZoneOfSource(FWClient *obj,
			     const gchar *zone,
			     const gchar *source)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "changeZoneOfSource",
					      g_variant_new("(ss)", zone,
							    source));
}

gboolean
fw_client_querySource(FWClient *obj,
		      const gchar *zone,
		      const gchar *source)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "querySource",
					       g_variant_new("(ss)", zone,
							     source));
}

/**
 * fw_client_removeSpource:
 * @obj: (type FWClient*): a FWClient instance
 * @zone: (type gchar*)
 * @source: (type gchar*)
 *
 * Remove binding of source from zone.
 *
 * Returns: (transfer none) (type gchar*)
 */
const gchar *
fw_client_removeSource(FWClient *obj,
		       const gchar *zone,
		       const gchar *source)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removeSource",
					      g_variant_new("(ss)", zone,
							    source));
}

/**
 * fw_client_getSources:
 * @obj: (type FWClient*): a FWClient instance
 * @zone: (type gchar*)
 *
 * Returns sources bound to zone
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_getSources(FWClient *obj,
		     const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->zone_proxy,
						   "getSources",
						   g_variant_new("(s)", zone));
}

/* rich rules */

const gchar *
fw_client_addRichRule(FWClient *obj,
		      const gchar *zone,
		      const gchar *rule,
		      gint32 timeout)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addRichRule",
					      g_variant_new("(ssi)", zone,
							    rule, timeout));
}

gboolean
fw_client_queryRichRule(FWClient *obj,
			const gchar *zone,
			const gchar *rule)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "queryRichRule",
					       g_variant_new("(ss)", zone,
							     rule));
}

const gchar *
fw_client_removeRichRule(FWClient *obj,
			 const gchar *zone,
			 const gchar *rule)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removeRichRule",
					      g_variant_new("(ss)", zone,
							    rule));
}

/**
 * fw_client_getRichRules:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_getRichRules(FWClient *obj,
		       const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->zone_proxy,
						   "getRichRules",
						   g_variant_new("(s)", zone));
}

/* services */

const gchar *
fw_client_addService(FWClient *obj,
		     const gchar *zone,
		     const gchar *service,
		     gint32 timeout)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addService",
					      g_variant_new("(ssi)", zone,
							    service, timeout));
}

gboolean
fw_client_queryService(FWClient *obj,
		       const gchar *zone,
		       const gchar *service)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "queryService",
					       g_variant_new("(ss)", zone,
							     service));
}

const gchar *
fw_client_removeService(FWClient *obj,
			const gchar *zone,
			const gchar *service)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removeService",
					      g_variant_new("(ss)", zone,
							    service));
}

/**
 * fw_client_getServices:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_getServices(FWClient *obj,
		      const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->zone_proxy,
						   "getServices",
						   g_variant_new("(s)", zone));
}

/* ports */

const gchar *
fw_client_addPort(FWClient *obj,
		  const gchar *zone,
		  const gchar *port,
		  const gchar *protocol,
		  gint32 timeout)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addPort",
					      g_variant_new("(sssi)", zone,
							    port, protocol,
							    timeout));
}

gboolean
fw_client_queryPort(FWClient *obj,
		    const gchar *zone,
		    const gchar *port,
		    const gchar *protocol)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "queryPort",
					       g_variant_new("(sss)", zone,
							     port, protocol));
}

const gchar *
fw_client_removePort(FWClient *obj,
		     const gchar *zone,
		     const gchar *port,
		     const gchar *protocol)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removePort",
					      g_variant_new("(sss)", zone,
							    port, protocol));
}

/**
 * fw_client_getPorts:
 *
 * Returns: (transfer none) (allow-none) (element-type FWPort*)
 */
GList *
fw_client_getPorts(FWClient *obj,
		   const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    GVariantIter iter;
    GList *list = NULL;

    variant = _fw_client_proxy_call_sync(priv, priv->zone_proxy,
					 "getPorts",
					 g_variant_new("(s)", zone));

    if (priv->error != NULL) {
	return list;
    }

    if (strncmp(g_variant_get_type_string(variant), "(aas)", 5) != 0) {
	return list;
    }

    /* get aas from (aas) */
    variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	GVariant *element;

	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    gsize length;
	    const gchar **strv = g_variant_get_strv(element, &length);

	    if (length == 2) {
		list = g_list_append(list, fw_port_new(g_strdup(strv[0]),
						       g_strdup(strv[1])));

	    }
	    g_free(strv);
	    g_variant_unref(element);
	}
    }

    g_variant_unref(variant);

    return list;
}

/* protocols */

const gchar *
fw_client_addProtocol(FWClient *obj,
		      const gchar *zone,
		      const gchar *protocol,
		      gint32 timeout)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addProtocol",
					      g_variant_new("(ssi)", zone,
							    protocol,
							    timeout));
}

gboolean
fw_client_queryProtocol(FWClient *obj,
			const gchar *zone,
			const gchar *protocol)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "queryProtocol",
					       g_variant_new("(ss)", zone,
							     protocol));
}

const gchar *
fw_client_removeProtocol(FWClient *obj,
			 const gchar *zone,
			 const gchar *protocol)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removeProtocol",
					      g_variant_new("(ss)", zone,
							    protocol));
}

/**
 * fw_client_getProtocols:
 *
 * Returns: (transfer none) (allow-none) (element-type GList*)
 */
GList *
fw_client_getProtocols(FWClient *obj,
		       const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->zone_proxy,
						   "getProtocols",
						   g_variant_new("(s)", zone));
}

/* sourceports */

const gchar *
fw_client_addSourcePort(FWClient *obj,
			const gchar *zone,
			const gchar *port,
			const gchar *protocol,
			gint32 timeout)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addSourcePort",
					      g_variant_new("(sssi)", zone,
							    port, protocol,
							    timeout));
}

gboolean
fw_client_querySourcePort(FWClient *obj,
			  const gchar *zone,
			  const gchar *port,
			  const gchar *protocol)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "querySourcePort",
					       g_variant_new("(sss)", zone,
							     port, protocol));
}

const gchar *
fw_client_removeSourcePort(FWClient *obj,
			   const gchar *zone,
			   const gchar *port,
			   const gchar *protocol)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removeSourcePort",
					      g_variant_new("(sss)", zone,
							    port, protocol));
}

/**
 * fw_client_getSourcePorts:
 *
 * Returns: (transfer none) (allow-none) (element-type FWPort*)
 */
GList *
fw_client_getSourcePorts(FWClient *obj,
			 const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    GVariantIter iter;
    GList *list = NULL;

    variant = _fw_client_proxy_call_sync(priv, priv->zone_proxy,
					 "getSourcePorts",
					 g_variant_new("(s)", zone));

    if (priv->error != NULL) {
	return list;
    }

    if (strncmp(g_variant_get_type_string(variant), "(aas)", 5) != 0) {
	return list;
    }

    /* get aas from (aas) */
    variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	GVariant *element;

	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    gsize length;
	    const gchar **strv = g_variant_get_strv(element, &length);

	    if (length == 2) {
		list = g_list_append(list, fw_port_new(g_strdup(strv[0]),
						       g_strdup(strv[1])));
	    }
	    g_free(strv);
	    g_variant_unref(element);
	}
    }

    g_variant_unref(variant);

    return list;
}

/* masquerade */

const gchar *
fw_client_addMasquerade(FWClient *obj,
			const gchar *zone,
			gint32 timeout)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addMasquerade",
					      g_variant_new("(si)", zone,
							    timeout));
}

gboolean
fw_client_queryMasquerade(FWClient *obj,
			  const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "queryMasquerade",
					       g_variant_new("(s)", zone));
}

const gchar *
fw_client_removeMasquerade(FWClient *obj,
			   const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removeMasquerade",
					      g_variant_new("(s)", zone));
}

/* foward ports */

const gchar *
fw_client_addForwardPort(FWClient *obj,
			 const gchar *zone,
			 const gchar *port,
			 const gchar *protocol,
			 const gchar *toport,
			 const gchar *toaddr,
			 gint32 timeout)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addForwardPort",
					      g_variant_new("(sssssi)", zone,
							    port, protocol,
							    toport, toaddr,
							    timeout));
}

gboolean
fw_client_queryForwardPort(FWClient *obj,
			   const gchar *zone,
			   const gchar *port,
			   const gchar *protocol,
			   const gchar *toport,
			   const gchar *toaddr)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "queryForwardPort",
					       g_variant_new("(sssss)", zone,
							     port, protocol,
							     toport, toaddr));
}

const gchar *
fw_client_removeForwardPort(FWClient *obj,
			    const gchar *zone,
			    const gchar *port,
			    const gchar *protocol,
			    const gchar *toport,
			    const gchar *toaddr)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removeForwardPort",
					      g_variant_new("(sssss)", zone,
							    port, protocol,
							    toport, toaddr));
}

/**
 * fw_client_getForwardPorts:
 *
 * Returns: (transfer none) (allow-none) (element-type FWForwardPort*)
 */
GList *
fw_client_getForwardPorts(FWClient *obj,
			  const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    GVariant *variant;
    GVariantIter iter;
    GList *list = NULL;

    variant = _fw_client_proxy_call_sync(priv, priv->zone_proxy,
					 "getForwardPorts",
					 g_variant_new("(s)", zone));

    if (priv->error != NULL) {
	return list;
    }

    if (strncmp(g_variant_get_type_string(variant), "(aas)", 5) != 0) {
	return list;
    }

    /* get aas from (aas) */
    variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	GVariant *element;

	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    gsize length;
	    const gchar **strv = g_variant_get_strv(element, &length);

	    if (length == 4) {
		list = g_list_append(list,
				     fw_forward_port_new(strv[0], strv[1],
							 strv[2], strv[3]));
	    }
	    g_free(strv);
	    g_variant_unref(element);
	}
    }

    g_variant_unref(variant);

    return list;
}

/* icmpblock */

const gchar *
fw_client_addIcmpBlock(FWClient *obj,
		       const gchar *zone,
		       const gchar *icmptype,
		       gint32 timeout)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "addIcmpBlock",
					      g_variant_new("(ssi)", zone,
							    icmptype, timeout));
}

gboolean
fw_client_queryIcmpBlock(FWClient *obj,
			 const gchar *zone,
			 const gchar *icmptype)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->zone_proxy,
					       "queryIcmpBlock",
					       g_variant_new("(ss)", zone,
							     icmptype));
}

const gchar *
fw_client_removeIcmpBlock(FWClient *obj,
			  const gchar *zone,
			  const gchar *icmptype)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str(priv, priv->zone_proxy,
					      "removeIcmpBlock",
					      g_variant_new("(ss)", zone,
							    icmptype));
}

/**
 * fw_client_getIcmpBlocks:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_getIcmpBlocks(FWClient *obj,
			const gchar *zone)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->zone_proxy,
						   "getIcmpBlocks",
						   g_variant_new("(s)", zone));
}

/* direct chain */

void
fw_client_addChain(FWClient *obj,
		   const gchar *ipv,
		   const gchar *table,
		   const gchar *chain)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_proxy_call_sync(priv, priv->direct_proxy,
			       "addChain",
			       g_variant_new("(sss)", ipv,
					     table, chain));
}

/**
 * fw_client_getChains:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_client_getChains(FWClient *obj,
		    const gchar *ipv,
		    const gchar *table)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->direct_proxy,
						   "getChains",
						   g_variant_new("(ss)", ipv,
								 table));
}

gboolean
fw_client_queryChain(FWClient *obj,
		     const gchar *ipv,
		     const gchar *table,
		     const gchar *chain)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_bool(priv, priv->direct_proxy,
					       "queryChain",
					       g_variant_new("(sss)", ipv,
							     table, chain));
}

void
fw_client_removeChain(FWClient *obj,
		      const gchar *ipv,
		      const gchar *table,
		      const gchar *chain)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    _fw_client_proxy_call_sync(priv, priv->direct_proxy,
			       "removeChain",
			       g_variant_new("(sss)", ipv,
					     table, chain));
}

/**
 * fw_client_getAllChains:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *priv_client_getAllChains(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

    return _fw_client_proxy_call_sync_get_str_list(priv, priv->direct_proxy,
						   "getAllChains",
						   NULL);
}

/* direct rule */

/**
 * fw_client_addRule:
 * @obj: (type FWClient*): a FWClient instance
 * @ipv: (type utf8)
 * @table: (type utf8)
 * @chain: (type utf8)
 * @priority: (type gint32)
 * @args: (type GList*) (element-type utf8)
 */
void
fw_client_addRule(FWClient *obj,
		  const gchar *ipv,
		  const gchar *table,
		  const gchar *chain,
		  gint32 priority,
		  const GList *args)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariantBuilder builder;
    gint i;

#ifdef FW_DEBUG
    g_printerr("fw_client_addRule('%s', '%s', '%s', %d, ", ipv, table, chain, priority);
    fw_str_list_print((GList *)args);
    g_printerr(")\n");
#endif

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    for (i=0; i<g_list_length((GList *)args); i++) {
	g_variant_builder_add(&builder, "s",
			      (gchar *)g_list_nth_data((GList *)args, i));
    }

    _fw_client_proxy_call_sync(priv, priv->direct_proxy,
			       "addRule",
			       g_variant_new("(sssias)", ipv, table, chain,
					     priority, &builder));
}

/**
 * fw_client_getRules:
 * @obj: (type FWClient*): a FWClient instance
 * @ipv: (type gchar*)
 * @table: (type gchar*)
 * @chain: (type gchar*)
 *
 * Returns: (transfer none) (allow-none) (element-type FWDirectSimpleRule*)
 */
GList *
fw_client_getRules(FWClient *obj,
		   const gchar *ipv,
		   const gchar *table,
		   const char *chain)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    GVariant *child;
    GVariantIter iter;
    GList *list = NULL;

    variant = _fw_client_proxy_call_sync(priv, priv->direct_proxy,
					 "getRules",
					 g_variant_new("(sss)", ipv,
						       table, chain));

    if (priv->error != NULL) {
	return list;
    }

    child = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, child)) {
	GVariant *element;

	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    GVariant *item;
	    gint32 priority;
	    GVariantIter iter2;
	    GList *args = NULL;
	    FWDirectSimpleRule *rule = NULL;

	    item = g_variant_get_child_value(element, 0);
	    g_variant_get(item, "i", &priority);

	    item = g_variant_get_child_value(element, 1);
	    if (g_variant_iter_init(&iter2, item)) {
		GVariant *item2;
		const gchar *str;

		while ((item2 = g_variant_iter_next_value(&iter2))) {
		    str = g_variant_get_string(item2, NULL);
		    args = g_list_append(args, g_strdup(str));
		}
	    }

	    rule = fw_direct_simple_rule_new();
	    fw_direct_simple_rule_setPriority(rule, priority);
	    fw_direct_simple_rule_setArgs(rule, args);
	    list = g_list_append(list, rule);
	    g_variant_unref(element);
	}
    }

    g_variant_unref(variant);

    return list;
}

/**
 * fw_client_queryRule:
 * @obj: (type FWClient*): a FWClient instance
 * @ipv: (type gchar*)
 * @table: (type gchar*)
 * @chain: (type gchar*)
 * @priority: (type gint32)
 * @args: (type GList*) (element-type gchar*)
 *
 * Returns: (type gboolean)
 */
gboolean
fw_client_queryRule(FWClient *obj,
		    const gchar *ipv,
		    const gchar *table,
		    const gchar *chain,
		    gint32 priority,
		    const GList *args)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariantBuilder builder;
    gint i;

#ifdef FW_DEBUG
    g_printerr("fw_client_removeRule('%s', '%s', '%s', %d, ", ipv, table, chain, priority);
    fw_str_list_print((GList *)args);
    g_printerr(")\n");
#endif

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    for (i=0; i<g_list_length((GList *)args); i++) {
	g_variant_builder_add(&builder, "s",
			      (gchar *)g_list_nth_data((GList *)args, i));
    }

    return _fw_client_proxy_call_sync_get_bool(priv, priv->direct_proxy,
					       "queryRule",
					       g_variant_new("(sssias)", ipv,
							     table, chain,
							     priority,
							     &builder));
}

/**
 * fw_client_removeRule:
 * @obj: (type FWClient*): a FWClient instance
 * @ipv: (type gchar*)
 * @table: (type gchar*)
 * @chain: (type gchar*)
 * @priority: (type gint32)
 * @args: (type GList*) (element-type gchar*)
 */
void
fw_client_removeRule(FWClient *obj,
		     const gchar *ipv,
		     const gchar *table,
		     const gchar *chain,
		     gint32 priority,
		     const GList *args)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariantBuilder builder;
    gint i;

#ifdef FW_DEBUG
    g_printerr("fw_client_removeRule('%s', '%s', '%s', %d, ", ipv, table, chain, priority);
    fw_str_list_print((GList *)args);
    g_printerr(")\n");
#endif

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    for (i=0; i<g_list_length((GList *)args); i++) {
	g_variant_builder_add(&builder, "s",
			      (gchar *)g_list_nth_data((GList *)args, i));
    }

    _fw_client_proxy_call_sync(priv, priv->direct_proxy,
			       "removeRule",
			       g_variant_new("(sssias)", ipv, table, chain,
					     priority, &builder));
}

/**
 * fw_client_getAllRules:
 *
 * Returns: (transfer none) (allow-none) (element-type FWDirectRule*)
 */
GList *
fw_client_getAllRules(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    GVariant *child;
    GVariantIter iter;
    GList *list = NULL;

#ifdef FW_DEBUG
    g_printerr("fw_client_getAllRules()\n");
#endif

    variant = _fw_client_proxy_call_sync(priv, priv->direct_proxy,
                                         "getAllRules",
                                         NULL);

    if (priv->error != NULL) {
        return list;
    }

    child = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, child)) {
        GVariant *element;

        while ((element = g_variant_iter_next_value(&iter)) != NULL) {
            GVariant *item;
            gchar *ipv, *table, *chain;
            gint32 priority;
            GVariantIter iter2;
            GList *args = NULL;
            FWDirectRule *rule = NULL;

            item = g_variant_get_child_value(element, 0);
            g_variant_get(item, "s", &ipv);

            item = g_variant_get_child_value(element, 1);
            g_variant_get(item, "s", &table);

            item = g_variant_get_child_value(element, 2);
            g_variant_get(item, "s", &chain);

            item = g_variant_get_child_value(element, 3);
            g_variant_get(item, "i", &priority);

            item = g_variant_get_child_value(element, 4);
            if (g_variant_iter_init(&iter2, item)) {
                GVariant *item2;
                const gchar *str;

                while ((item2 = g_variant_iter_next_value(&iter2))) {
                    str = g_variant_get_string(item2, NULL);
                    args = g_list_append(args, g_strdup(str));
                }
            }

            rule = fw_direct_rule_new();
            fw_direct_rule_setIpv(rule, ipv);
            fw_direct_rule_setTable(rule, table);
            fw_direct_rule_setChain(rule, chain);
            fw_direct_rule_setPriority(rule, priority);
            fw_direct_rule_setArgs(rule, args);
            list = g_list_append(list, rule);
            g_variant_unref(element);
        }
    }

    g_variant_unref(variant);

    return list;
}

/* direct passthrough (untracked) */

/**
 * fw_client_passthrough:
 * @obj: (type FWClient*): a FWClient instance
 * @ipv: (type utf8)
 * @args: (type GList*) (element-type utf8)
 */
const gchar *
fw_client_passthrough(FWClient *obj,
		      const gchar *ipv,
		      const GList *args)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariantBuilder builder;
    gint i;

#ifdef FW_DEBUG
    g_printerr("fw_client_passthrough('%s', ", ipv);
    fw_str_list_print((GList *)args);
    g_printerr(")\n");
#endif

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    for (i=0; i<g_list_length((GList *)args); i++) {
	g_variant_builder_add(&builder, "s",
			      (gchar *)g_list_nth_data((GList *)args, i));
    }

    return _fw_client_proxy_call_sync_get_str(priv, priv->direct_proxy,
					      "passthrough",
					      g_variant_new("(sas)", ipv,
							    &builder));
}

/* direct passthrough (tracked) */

/**
 * fw_client_addPassthrough:
 * @obj: (type FWClient*): a FWClient instance
 * @ipv: (type utf8)
 * @args: (type GList*) (element-type utf8)
 */
void
fw_client_addPassthrough(FWClient *obj,
			 const gchar *ipv,
			 const GList *args)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariantBuilder builder;
    gint i;

#ifdef FW_DEBUG
    g_printerr("fw_client_addPassthrough('%s', ", ipv);
    fw_str_list_print((GList *)args);
    g_printerr(")\n");
#endif

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    for (i=0; i<g_list_length((GList *)args); i++) {
	g_variant_builder_add(&builder, "s",
			      (gchar *)g_list_nth_data((GList *)args, i));
    }

    _fw_client_proxy_call_sync(priv, priv->direct_proxy,
			       "addPassthrough",
			       g_variant_new("(sas)", ipv, &builder));
}

/**
 * fw_client_removePassthrough:
 * @obj: (type FWClient*): a FWClient instance
 * @ipv: (type utf8)
 * @args: (type GList*) (element-type utf8)
 */
void
fw_client_removePassthrough(FWClient *obj,
			    const gchar *ipv,
			    const GList *args)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariantBuilder builder;
    gint i;

#ifdef FW_DEBUG
    g_printerr("fw_client_removePassthrough('%s', ", ipv);
    fw_str_list_print((GList *)args);
    g_printerr(")\n");
#endif

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    for (i=0; i<g_list_length((GList *)args); i++) {
	g_variant_builder_add(&builder, "s",
			      (gchar *)g_list_nth_data((GList *)args, i));
    }

    _fw_client_proxy_call_sync(priv, priv->direct_proxy,
			       "removePassthrough",
			       g_variant_new("(sas)", ipv, &builder));
}

/**
 * fw_client_queryPassthrough:
 * @obj: (type FWClient*): a FWClient instance
 * @ipv: (type utf8)
 * @args: (type GList*) (element-type utf8)
 *
 * Returns: (type gboolean)
 */
gboolean
fw_client_queryPassthrough(FWClient *obj,
			   const gchar *ipv,
			   const GList *args)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariantBuilder builder;
    gint i;

#ifdef FW_DEBUG
    g_printerr("fw_client_queryPassthrough('%s', ", ipv);
    fw_str_list_print((GList *)args);
    g_printerr(")\n");
#endif

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    for (i=0; i<g_list_length((GList *)args); i++) {
	g_variant_builder_add(&builder, "s",
			      (gchar *)g_list_nth_data((GList *)args, i));
    }

    return _fw_client_proxy_call_sync_get_bool(priv, priv->direct_proxy,
					       "queryPassthrough",
					       g_variant_new("(sas)", ipv,
							     &builder));
}

/**
 * fw_client_getPassthroughs:
 * @obj: (type FWClient*): a FWClient instance
 * @ipv: (type utf8)
 *
 * Returns: (type GList*) (transfer container) (element-type FWArgs)
 */
GList *
fw_client_getPassthroughs(FWClient *obj,
			  const gchar *ipv)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    GVariantIter iter;
    GList *list = NULL;

#ifdef FW_DEBUG
    g_printerr("fw_client_getPassthroughs('%s')\n, ", ipv);
#endif

    variant = _fw_client_proxy_call_sync(priv, priv->direct_proxy,
					 "getPassthroughs",
					 g_variant_new("(s)", ipv));

    if (priv->error != NULL) {
	return list;
    }

    if (strncmp(g_variant_get_type_string(variant), "(aas)", 5) != 0) {
	return list;
    }

    /* get aas from (aas) */
    variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	GVariant *element;

	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    FWArgs *args = fw_args_new();
	    gsize length, i;
	    const gchar **strv = g_variant_get_strv(element, &length);

	    for (i=0; i<length; i++) {
		fw_args_addArg(args, g_strdup(strv[i]));
	    }
	    g_free(strv);

	    list = g_list_append(list, args);
	    g_variant_unref(element);
	}
    }

    g_variant_unref(variant);

    return list;
}

/**
 * fw_client_getAllPassthroughs:
 * @obj: (type FWClient*): a FWClient instance
 *
 * Returns: (type GList*) (transfer container) (element-type FWPassthrough*)
 */
GList *
fw_client_getAllPassthroughs(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);
    GVariant *variant;
    GVariantIter iter;
    GList *list = NULL;

#ifdef FW_DEBUG
    g_printerr("fw_client_getPassthroughs()\n");
#endif

    variant = _fw_client_proxy_call_sync(priv, priv->direct_proxy,
					 "getAllPassthroughs",
					 NULL);

    if (priv->error != NULL) {
	return list;
    }

    if (strncmp(g_variant_get_type_string(variant), "(a(sas))", 7) != 0) {
	return list;
    }

    /* get a(sas) from (a(sas)) */
    variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	GVariant *element;

	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    GVariant *item;
	    gchar *ipv;
	    GVariantIter iter2;
	    GList *args = NULL;
	    FWPassthrough *pthru = NULL;

	    item = g_variant_get_child_value(element, 0);
	    g_variant_get(item, "s", &ipv);

	    item = g_variant_get_child_value(element, 1);
	    if (g_variant_iter_init(&iter2, item)) {
		GVariant *item2;
		const gchar *str;

		while ((item2 = g_variant_iter_next_value(&iter2))) {
		    str = g_variant_get_string(item2, NULL);
		    args = g_list_append(args, g_strdup(str));
		}
	    }

	    pthru = fw_passthrough_new();
	    fw_passthrough_setIpv(pthru, ipv);
	    fw_passthrough_setArgs(pthru, args);
	    list = g_list_append(list, pthru);
	    g_variant_unref(element);
	}
    }

    g_variant_unref(variant);

    return list;
}

/**
 * fw_client_removeAllPassthroughs:
 * @obj: (type FWClient*): a FWClient instance
 */
void
fw_client_removeAllPassthroughs(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_removeAllPassthroughs()\n");
#endif

    _fw_client_proxy_call_sync(priv, priv->direct_proxy,
			       "removeAllPassthroughs",
			       NULL);
}


/* lockdown */

void
fw_client_enableLockdown(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_enableLockdown()\n");
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "enableLockdown",
			       NULL);
}

void
fw_client_disableLockdown(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_disableLockdown()\n");
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "disableLockdown",
			       NULL);
}

gboolean
fw_client_queryLockdown(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_queryLockdown()\n");
#endif

    return _fw_client_proxy_call_sync_get_bool(priv, priv->policies_proxy,
					       "queryLockdown",
					       NULL);
}

/* policies */

/* lockdown whitelist commands */

void
fw_client_addLockdownWhitelistCommand(FWClient *obj,
				      const gchar *command)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_addLockdownWhitelistCommand('%s')\n", command);
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "addLockdownWhitelistCommand",
			       g_variant_new("(s)", command));
}

/**
 * fw_client_getLockdownWhitelistCommands:
 * @obj: (type FWClient*): a FWClient instance
 *
 * Returns: (type GList*) (transfer container) (element-type gchar*)
 */
GList *
fw_client_getLockdownWhitelistCommands(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_getLockdownWhitelistCommands()\n");
#endif

    return _fw_client_proxy_call_sync_get_str_list(
	priv, priv->policies_proxy,
	"getLockdownWhitelistCommands",
	NULL);
}

gboolean
fw_client_queryLockdownWhitelistCommand(FWClient *obj,
					const gchar *command)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_queryLockdownWhitelistCommand('%s')\n", command);
#endif

    return _fw_client_proxy_call_sync_get_bool(priv, priv->policies_proxy,
					       "queryLockdownWhitelistCommand",
					       g_variant_new("(s)", command));
}

void
fw_client_removeLockdownWhitelistCommand(FWClient *obj,
					 const gchar *command)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_removeLockdownWhitelistCommand('%s')\n", command);
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "removeLockdownWhitelistCommand",
			       g_variant_new("(s)", command));
}

/* lockdown whitelist contexts */

void
fw_client_addLockdownWhitelistContext(FWClient *obj,
				      const gchar *context)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_addLockdownWhitelistContext('%s')\n", context);
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "addLockdownWhitelistContext",
			       g_variant_new("(s)", context));
}

/**
 * fw_client_getLockdownWhitelistContexts:
 * @obj: (type FWClient*): a FWClient instance
 *
 * Returns: (type GList*) (transfer container) (element-type gchar*)
 */
GList *
fw_client_getLockdownWhitelistContexts(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_getLockdownWhitelistContexts()\n");
#endif

    return _fw_client_proxy_call_sync_get_str_list(
	priv, priv->policies_proxy,
	"getLockdownWhitelistContexts",
	NULL);
}

gboolean
fw_client_queryLockdownWhitelistContext(FWClient *obj,
					const gchar *context)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_queryLockdownWhitelistContext('%s')\n", context);
#endif

    return _fw_client_proxy_call_sync_get_bool(priv, priv->policies_proxy,
					       "queryLockdownWhitelistContext",
					       g_variant_new("(s)", context));
}

void
fw_client_removeLockdownWhitelistContext(FWClient *obj,
					 const gchar *context)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_removeLockdownWhitelistContext('%s')\n", context);
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "removeLockdownWhitelistContext",
			       g_variant_new("(s)", context));
}

/* lockdown whitelist uids */

void
fw_client_addLockdownWhitelistUid(FWClient *obj,
				  gint32  uid)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_addLockdownWhitelistUid(%d)\n", uid);
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "addLockdownWhitelistUid",
			       g_variant_new("(i)", uid));
}

/**
 * fw_client_getLockdownWhitelistUids:
 * @obj: (type FWClient*): a FWClient instance
 *
 * Returns: (type GList*) (transfer container) (element-type gint32)
 */
GList *
fw_client_getLockdownWhitelistUids(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_getLockdownWhitelistUids()\n");
#endif

    return _fw_client_proxy_call_sync_get_gint32_list(
	priv, priv->policies_proxy,
	"getLockdownWhitelistUids",
	NULL);
}

gboolean
fw_client_queryLockdownWhitelistUid(FWClient *obj,
				    gint32 uid)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_addLockdownWhitelistUid(%d)\n", uid);
#endif

    return _fw_client_proxy_call_sync_get_bool(priv, priv->policies_proxy,
					       "queryLockdownWhitelistUid",
					       g_variant_new("(i)", uid));
}

void
fw_client_removeLockdownWhitelistUid(FWClient *obj,
				     gint32 uid)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_addLockdownWhitelistUid(%d)\n", uid);
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "removeLockdownWhitelistUid",
			       g_variant_new("(i)", uid));
}

/* lockdown whitelist users */

void
fw_client_addLockdownWhitelistUser(FWClient *obj,
				   const gchar *user)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_addLockdownWhitelistUser('%s')\n", user);
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "addLockdownWhitelistUser",
			       g_variant_new("(s)", user));
}

/**
 * fw_client_getLockdownWhitelistUsers:
 * @obj: (type FWClient*): a FWClient instance
 *
 * Returns: (type GList*) (transfer container) (element-type gchar*)
 */
GList *
fw_client_getLockdownWhitelistUsers(FWClient *obj)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_getLockdownWhitelistUsers()\n");
#endif

    return _fw_client_proxy_call_sync_get_str_list(
	priv, priv->policies_proxy,
	"getLockdownWhitelistUsers",
	NULL);
}

gboolean
fw_client_queryLockdownWhitelistUser(FWClient *obj,
				     const gchar *user)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_queryLockdownWhitelistUser('%s')\n", user);
#endif

    return _fw_client_proxy_call_sync_get_bool(priv, priv->policies_proxy,
					       "queryLockdownWhitelistUser",
					       g_variant_new("(s)", user));
}

void
fw_client_removeLockdownWhitelistUser(FWClient *obj,
				      const gchar *user)
{
    FWClientPrivate *priv = FW_CLIENT_GET_PRIVATE(obj);

#ifdef FW_DEBUG
    g_printerr("fw_client_removeLockdownWhitelistUser('%s')\n", user);
#endif

    _fw_client_proxy_call_sync(priv, priv->policies_proxy,
			       "removeLockdownWhitelistUser",
			       g_variant_new("(s)", user));
}

