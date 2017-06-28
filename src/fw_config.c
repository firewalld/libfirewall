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
#include "fw_config.h"
#include "fw_port.h"
#include "fw_forward_port.h"
#include "fw_direct_simple_rule.h"
#include "fw_direct_rule.h"
#include "fw_args.h"
#include "fw_passthrough.h"
#include "fw_types.h"

G_DEFINE_TYPE(FWConfig, fw_config, G_TYPE_OBJECT);

#define FW_CONFIG_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), FW_CONFIG_TYPE, FWConfigPrivate))

typedef struct {
    /* dbus */
    GDBusConnection *connection;
    GDBusProxy *proxy;
    GDBusProxy *direct_proxy;
    GDBusProxy *helper_proxy;
    GDBusProxy *icmptype_proxy;
    GDBusProxy *ipset_proxy;
    GDBusProxy *policies_proxy;
    GDBusProxy *properties_proxy;
    GDBusProxy *service_proxy;
    GDBusProxy *zone_proxy;
    GError *error;

    /* properties */
    gboolean quiet;
    gboolean connected;
} FWConfigPrivate;

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
    PROP_NF_NAT_HELPERS,
    PROP_IPV4_ICMPTYPES,
    PROP_IPV6_ICMPTYPES,
    N_PROPERTIES,
};

/* static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, }; */

FWConfig *
fw_config_new()
{
    return g_object_new(FW_CONFIG_TYPE, NULL);
}

static void _fw_config_dbus_connect(FWConfigPrivate *fw);

static void
fw_config_init(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    /* init vars */
    fw->connection = NULL;
    fw->proxy = NULL;
    fw->helper_proxy = NULL;
    fw->icmptype_proxy = NULL;
    fw->ipset_proxy = NULL;
    fw->direct_proxy = NULL;
    fw->policies_proxy = NULL;
    fw->properties_proxy = NULL;
    fw->service_proxy = NULL;
    fw->zone_proxy = NULL;
    fw->error = NULL;

    fw->quiet = FALSE;
    fw->connected = FALSE;

    /* connect to system dbus */
    fw->connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &fw->error);
    if (fw->error != NULL) {
        g_print("ERROR: Failed to connect to system bus: %s",
		fw->error->message);
	return;
    }

#ifdef FW_DEBUG
    gchar *conn_name;
    g_object_get(fw->connection, "unique-name", &conn_name, NULL);
    g_free(conn_name);
#endif

    _fw_config_dbus_connect(fw);
}

static void
_fw_config_reset_error(FWConfigPrivate *fw)
{
    if (fw->error != NULL) {
	g_error_free(fw->error);
	fw->error = NULL;
    }
}



static void
_fw_config_signal_receiver(GDBusProxy *proxy,
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
_fw_config_dbus_connect(FWConfigPrivate *fw)
{
    _fw_config_reset_error(fw);

    /* create syncroneous proxies */
    fw->proxy = g_dbus_proxy_new_sync(fw->connection,
				      G_DBUS_PROXY_FLAGS_NONE,
				      NULL,
				      FW_DBUS_NAME,
				      FW_DBUS_PATH_CONFIG,
				      FW_DBUS_INTERFACE_CONFIG,
				      NULL,
				      &fw->error);

    /* G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START |
       G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES */

    g_assert(fw->proxy != NULL);
    g_dbus_proxy_set_default_timeout(fw->proxy, G_MAXINT);

    /* connect signal receiver */
    g_signal_connect(fw->proxy,
		     "g-signal",
		     G_CALLBACK (_fw_config_signal_receiver),
		     NULL);

    fw->direct_proxy = g_dbus_proxy_new_sync(fw->connection,
					     G_DBUS_PROXY_FLAGS_NONE,
					     NULL,
					     FW_DBUS_NAME,
					     FW_DBUS_PATH,
					     FW_DBUS_INTERFACE_CONFIG_DIRECT,
					     NULL,
					     &fw->error);
    g_assert(fw->direct_proxy != NULL);
    g_dbus_proxy_set_default_timeout(fw->direct_proxy, G_MAXINT);

    fw->icmptype_proxy = g_dbus_proxy_new_sync(fw->connection,
					       G_DBUS_PROXY_FLAGS_NONE,
					       NULL,
					       FW_DBUS_NAME,
					       FW_DBUS_PATH,
					       FW_DBUS_INTERFACE_CONFIG_ICMPTYPE,
					       NULL,
					       &fw->error);
    g_assert(fw->icmptype_proxy != NULL);
    g_dbus_proxy_set_default_timeout(fw->icmptype_proxy, G_MAXINT);

    fw->ipset_proxy = g_dbus_proxy_new_sync(fw->connection,
					    G_DBUS_PROXY_FLAGS_NONE,
					    NULL,
					    FW_DBUS_NAME,
					    FW_DBUS_PATH,
					    FW_DBUS_INTERFACE_CONFIG_IPSET,
					    NULL,
					    &fw->error);
    g_assert(fw->ipset_proxy != NULL);
    g_dbus_proxy_set_default_timeout(fw->ipset_proxy, G_MAXINT);

    fw->policies_proxy = g_dbus_proxy_new_sync(fw->connection,
					       G_DBUS_PROXY_FLAGS_NONE,
					       NULL,
					       FW_DBUS_NAME,
					       FW_DBUS_PATH,
					       FW_DBUS_INTERFACE_CONFIG_POLICIES,
					       NULL,
					       &fw->error);
    g_assert(fw->policies_proxy != NULL);
    g_dbus_proxy_set_default_timeout(fw->policies_proxy, G_MAXINT);

    fw->service_proxy = g_dbus_proxy_new_sync(fw->connection,
					      G_DBUS_PROXY_FLAGS_NONE,
					      NULL,
					      FW_DBUS_NAME,
					      FW_DBUS_PATH,
					      FW_DBUS_INTERFACE_CONFIG_SERVICE,
					      NULL,
					      &fw->error);
    g_assert(fw->service_proxy != NULL);
    g_dbus_proxy_set_default_timeout(fw->service_proxy, G_MAXINT);

    fw->zone_proxy = g_dbus_proxy_new_sync(fw->connection,
					   G_DBUS_PROXY_FLAGS_NONE,
					   NULL,
					   FW_DBUS_NAME,
					   FW_DBUS_PATH,
					   FW_DBUS_INTERFACE_CONFIG_ZONE,
					   NULL,
					   &fw->error);
    g_assert(fw->zone_proxy != NULL);
    g_dbus_proxy_set_default_timeout(fw->zone_proxy, G_MAXINT);

    fw->connected = TRUE;

    /* do not terminate if connection has been closed */
    g_dbus_connection_set_exit_on_close(fw->connection, FALSE);
}

static void
fw_config_finalize(GObject *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    _fw_config_reset_error(fw);

    /* disconnect */
    /***/

    G_OBJECT_CLASS(fw_config_parent_class)->finalize(obj);
}

static void
fw_config_set_property(GObject      *obj,
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
fw_config_get_property(GObject *obj,
		       guint property_id,
		       GValue *value,
		       GParamSpec *pspec)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    switch (property_id) {

#ifdef TODO
    case PROP_CONNECTED:
        /* connected */
        break;

    case PROP_ERROR:
        /* error */
	if (fw->error != NULL)
	    g_value_set_string(value, fw->error->message);
        break;
#endif

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
        break;
    }
}

static void
fw_config_class_init(FWConfigClass *fw_config_class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(fw_config_class);

/*    obj_class->constructor = fw_config_constructor; */
    obj_class->set_property = fw_config_set_property;
    obj_class->get_property = fw_config_get_property;
    obj_class->finalize = fw_config_finalize;

    /* properties */

    /**
     * FWConfig:version:
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
    g_type_class_add_private(obj_class, sizeof(FWConfigPrivate));
}

GVariant *
_fw_config_proxy_call_sync(FWConfigPrivate *fw,
			   GDBusProxy *proxy,
			   const gchar *method_name,
			   GVariant *parameters)
{
    GVariant *result;

    _fw_config_reset_error(fw);

    result = g_dbus_proxy_call_sync(proxy,
				    method_name,
				    parameters,
				    G_DBUS_CALL_FLAGS_NONE,
				    -1,
				    NULL,
				    &fw->error);
    if (fw->error != NULL) {
        g_print(_("ERROR: %s failed: %s\n"), method_name, fw->error->message);
    }

    return result;
}

const gchar *
_fw_config_proxy_call_sync_get_str(FWConfigPrivate *fw,
				   GDBusProxy *proxy,
				   const gchar *method_name,
				   GVariant *parameters)
{
    GVariant *variant;
    gchar *value_str;

    variant = _fw_config_proxy_call_sync(fw, proxy, method_name, parameters);

    if (fw->error != NULL) {
	return NULL;
    }

    g_variant_get(variant, "(s)", &value_str);
    g_variant_unref(variant);

    return value_str;
}

const gchar *
_fw_config_proxy_call_sync_get_obj(FWConfigPrivate *fw,
				   GDBusProxy *proxy,
				   const gchar *method_name,
				   GVariant *parameters)
{
    GVariant *variant;
    gchar *value_str;

    variant = _fw_config_proxy_call_sync(fw, proxy, method_name, parameters);

    if (fw->error != NULL) {
	return NULL;
    }

    g_variant_get(variant, "(o)", &value_str);
    g_variant_unref(variant);

    return value_str;
}

gboolean
_fw_config_proxy_call_sync_get_bool(FWConfigPrivate *fw,
				    GDBusProxy *proxy,
				    const gchar *method_name,
				    GVariant *parameters)
{
    GVariant *variant;
    gboolean value;

    variant = _fw_config_proxy_call_sync(fw, proxy, method_name, parameters);

    if (fw->error != NULL) {
	return FALSE;
    }

    g_variant_get(variant, "(b)", &value);
    g_variant_unref(variant);

    return value;
}

GList *
_fw_config_proxy_call_sync_get_str_list(FWConfigPrivate *fw,
					GDBusProxy *proxy,
					const gchar *method_name,
					GVariant *parameters)
{
    GVariant *variant, *element;
    GVariantIter iter;
    GList *list = NULL;
    gchar *str;

    variant = _fw_config_proxy_call_sync(fw, proxy, method_name, parameters);

    if (fw->error != NULL) {
	return list;
    }

    if (strncmp(g_variant_get_type_string(variant), "(as)", 4) != 0)
    {
	g_print("!(as): %s", g_variant_get_type_string(variant));
	return list;
    }

    /* get as from (as) */
    variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    g_variant_get(element, "s", &str);
	    list = fw_str_list_append(list, str);
	    g_variant_unref(element);
	}
    }
    g_variant_unref(variant);

    return list;
}

GList *
_fw_config_proxy_call_sync_get_obj_list(FWConfigPrivate *fw,
					GDBusProxy *proxy,
					const gchar *method_name,
					GVariant *parameters)
{
    GVariant *variant, *element;
    GVariantIter iter;
    GList *list = NULL;
    gchar *str;

    variant = _fw_config_proxy_call_sync(fw, proxy, method_name, parameters);

    if (fw->error != NULL) {
	return list;
    }

    if (strncmp(g_variant_get_type_string(variant), "(ao)", 4) != 0)
    {
	g_print("!(as): %s", g_variant_get_type_string(variant));
	return list;
    }

    /* get as from (as) */
    variant = g_variant_get_child_value(variant, 0);

    if (g_variant_iter_init(&iter, variant)) {
	while ((element = g_variant_iter_next_value(&iter)) != NULL) {
	    g_variant_get(element, "o", &str);
	    list = fw_str_list_append(list, str);
	    g_variant_unref(element);
	}
    }
    g_variant_unref(variant);

    return list;
}

GList *
_fw_config_proxy_call_sync_get_gint32_list(FWConfigPrivate *fw,
					   GDBusProxy *proxy,
					   const gchar *method_name,
					   GVariant *parameters)
{
    GVariant *variant, *element;
    GVariantIter iter;
    GList *list = NULL;
    gint32 *i;

    variant = _fw_config_proxy_call_sync(fw, proxy, method_name, parameters);

    if (fw->error != NULL) {
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

/**
 * fw_config_addHelper:
 * @obj: (type FWConfig*): a FWConfig instance
 * @helper: (type utf8)
 * @settings: (type FWHelper*)
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
const gchar *
fw_config_addHelper(FWConfig *obj,
		    const gchar *helper,
		    FWHelper *settings)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "addHelper",
	g_variant_new("(sv)", helper, fw_helper_to_variant(settings)));
}

/**
 * fw_config_listHelpers:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_config_listHelpers(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_obj_list(fw, fw->proxy,
						   "listHelpers", NULL);
}

/**
 * fw_config_getHelperNames:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_config_getHelperNames(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_str_list(fw, fw->proxy,
						   "getHelperNames", NULL);
}

/**
 * fw_config_getHelperByName:
 *
 * Returns: (transfer none) (allow-none) (type FWConfigHelper*)
 */
FWConfigHelper *
fw_config_getHelperByName(FWConfig *obj,
			  const gchar *helper)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    gchar *path = (gchar *) _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "getHelperByName",
	g_variant_new("(s)", helper));

    return fw_config_helper_new(path);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/**
 * fw_config_addIcmpType:
 * @obj: (type FWConfig*): a FWConfig instance
 * @icmptype: (type utf8)
 * @settings: (type FWIcmpType*)
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
const gchar *
fw_config_addIcmpType(FWConfig *obj,
		      const gchar *icmptype,
		      FWIcmpType *settings)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);
    GVariantBuilder builder;
    GList *list = fw_icmptype_getDestinations(settings);
    gint i;

    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    for (i=0; i<g_list_length(list); i++) {
	g_variant_builder_add(&builder, "s",
			      (gchar *)g_list_nth_data(list, i));
    }

    return _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "addIcmpType",
	g_variant_new("(s(sssas))",
		      icmptype,
		      fw_icmptype_getVersion(settings),
		      fw_icmptype_getShort(settings),
		      fw_icmptype_getDescription(settings),
		      &builder));
}

/**
 * fw_config_listIcmpTypes:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_config_listIcmpTypes(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_obj_list(fw, fw->proxy,
						   "listIcmpTypes", NULL);
}

/**
 * fw_config_getIcmpTypeNames:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_config_getIcmpTypeNames(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_str_list(fw, fw->proxy,
						   "getIcmpTypeNames", NULL);
}

/**
 * fw_config_getIcmpTypeByName:
 *
 * Returns: (transfer none) (allow-none) (type FWConfigIcmpType*)
 */
FWConfigIcmpType *
fw_config_getIcmpTypeByName(FWConfig *obj,
			    const gchar *icmptype)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    gchar *path = (gchar *) _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "getIcmpTypeByName",
	g_variant_new("(s)", icmptype));

    return fw_config_icmptype_new(path);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/**
 * fw_config_addIPSet:
 * @obj: (type FWConfig*): a FWConfig instance
 * @ipset: (type utf8)
 * @settings: (type FWIPSet*)
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
const gchar *
fw_config_addIPSet(FWConfig *obj,
		   const gchar *ipset,
		   FWIPSet *settings)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);
    GVariantBuilder builder_options;
    GVariantBuilder builder_entries;
    GHashTable *hash;
    GList *list;
    GHashTableIter iter;
    gpointer key, value;
    gint i;
    
    g_variant_builder_init(&builder_options, G_VARIANT_TYPE("a{ss}"));
    hash = fw_ipset_getOptions(settings);
    g_hash_table_iter_init(&iter, hash);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
	g_variant_builder_add(&builder_options, "{ss}", key, value);
    }
	
    g_variant_builder_init(&builder_entries, G_VARIANT_TYPE("as"));
    list = fw_ipset_getEntries(settings);
    for (i=0; i<g_list_length(list); i++) {
	g_variant_builder_add(&builder_entries, "s",
			      g_list_nth_data(list, i));
    }

    return _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "addIPSet",
	g_variant_new("(s(ssssa{ss}as))",
		      ipset,
		      fw_ipset_getVersion(settings),
		      fw_ipset_getShort(settings),
		      fw_ipset_getDescription(settings),
		      fw_ipset_getType(settings),
		      &builder_options,
		      &builder_entries));
}

/**
 * fw_config_listIPSets:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_config_listIPSets(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_obj_list(fw, fw->proxy,
						   "listIPSets", NULL);
}

/**
 * fw_config_getIPSetNames:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_config_getIPSetNames(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_str_list(fw, fw->proxy,
						   "getIPSetNames", NULL);
}

/**
 * fw_config_getIPSetByName:
 *
 * Returns: (transfer none) (allow-none) (type FWConfigIPSet*)
 */
FWConfigIPSet *
fw_config_getIPSetByName(FWConfig *obj,
			 const gchar *ipset)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    gchar *path = (gchar *) _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "getIPSetByName",
	g_variant_new("(s)", ipset));

    return fw_config_ipset_new(path);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/**
 * fw_config_addService:
 * @obj: (type FWConfig*): a FWConfig instance
 * @service: (type utf8)
 * @settings: (type FWService*)
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
const gchar *
fw_config_addService(FWConfig *obj,
		     const gchar *service,
		     FWService *settings)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "addService",
	g_variant_new("(sv)", service, fw_service_to_variant(settings)));
}

/**
 * fw_config_listServices:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_listServices(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_obj_list(fw, fw->proxy,
						   "listServices", NULL);
}

/**
 * fw_config_getServiceNames:
 *
 * Returns: (transfer none) (allow-none) (type GList*) (element-type gchar*)
 */
GList *
fw_config_getServiceNames(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_str_list(fw, fw->proxy,
						   "getServiceNames", NULL);
}

/**
 * fw_config_getServiceByName:
 *
 * Returns: (transfer none) (allow-none) (type FWConfigService*)
 */
FWConfigService *
fw_config_getServiceByName(FWConfig *obj,
			   const gchar *service)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    gchar *path = (gchar *) _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "getServiceByName",
	g_variant_new("(s)", service));

    return fw_config_service_new(path);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/**
 * fw_config_addZone:
 * @obj: (type FWConfig*): a FWConfig instance
 * @zone: (type utf8)
 * @settings: (type FWZone*)
 * Returns: (transfer none) (allow-none) (type gchar*)
 */
const gchar *
fw_config_addZone(FWConfig *obj,
		    const gchar *zone,
		    FWZone *settings)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "addZone",
	g_variant_new("(sv)", zone, fw_zone_to_variant(settings)));

#ifdef FOOBAT
    GVariantBuilder builder_services;
    GVariantBuilder builder_ports;
    GVariantBuilder builder_icmpblocks;
    GVariantBuilder builder_forward_ports;
    GVariantBuilder builder_interfaces;
    GVariantBuilder builder_sources;
    GVariantBuilder builder_rich_rules;
    GVariantBuilder builder_protocols;
    GVariantBuilder builder_source_ports;
    GList *list;
    gint i;

    g_variant_builder_init(&builder_services, G_VARIANT_TYPE("as"));
    list = fw_zone_getServices(settings);
    for (i=0; i<g_list_length(list); i++) {
	g_variant_builder_add(&builder_services, "s",
			      (gchar *)g_list_nth_data(list, i));
    }

    g_variant_builder_init(&builder_ports, G_VARIANT_TYPE("a(ss)"));
    list = fw_zone_getPorts(settings);
    for (i=0; i<g_list_length(list); i++) {
	FWPort *port = g_list_nth_data(list, i);
	g_variant_builder_add(&builder_ports, "(ss)",
			      fw_port_getPort(port),
			      fw_port_getProtocol(port));
    }

    g_variant_builder_init(&builder_icmpblocks, G_VARIANT_TYPE("as"));
    list = fw_zone_getIcmpBlocks(settings);
    for (i=0; i<g_list_length(list); i++) {
	g_variant_builder_add(&builder_icmpblocks, "s",
			      (gchar *)g_list_nth_data(list, i));
    }

    g_variant_builder_init(&builder_forward_ports, G_VARIANT_TYPE("a(ssss)"));
    list = fw_zone_getForwardPorts(settings);
    for (i=0; i<g_list_length(list); i++) {
	FWForwardPort *port = g_list_nth_data(list, i);
	g_variant_builder_add(&builder_forward_ports, "(ssss)",
			      fw_forward_port_getPort(port),
			      fw_forward_port_getProtocol(port),
			      fw_forward_port_getToPort(port),
			      fw_forward_port_getToAddr(port));

    }

    g_variant_builder_init(&builder_interfaces, G_VARIANT_TYPE("as"));
    list = fw_zone_getInterfaces(settings);
    for (i=0; i<g_list_length(list); i++) {
	g_variant_builder_add(&builder_interfaces, "s",
			      (gchar *)g_list_nth_data(list, i));
    }

    g_variant_builder_init(&builder_sources, G_VARIANT_TYPE("as"));
    list = fw_zone_getSources(settings);
    for (i=0; i<g_list_length(list); i++) {
	g_variant_builder_add(&builder_sources, "s",
			      (gchar *)g_list_nth_data(list, i));
    }

    g_variant_builder_init(&builder_rich_rules, G_VARIANT_TYPE("as"));
    list = fw_zone_getRichRules(settings);
    for (i=0; i<g_list_length(list); i++) {
	g_variant_builder_add(&builder_rich_rules, "s",
			      (gchar *)g_list_nth_data(list, i));
    }

    g_variant_builder_init(&builder_protocols, G_VARIANT_TYPE("as"));
    list = fw_zone_getProtocols(settings);
    for (i=0; i<g_list_length(list); i++) {
	g_variant_builder_add(&builder_protocols, "s",
			      (gchar *)g_list_nth_data(list, i));
    }

    g_variant_builder_init(&builder_source_ports, G_VARIANT_TYPE("a(ss)"));
    list = fw_zone_getSourcePorts(settings);
    for (i=0; i<g_list_length(list); i++) {
	FWPort *port = g_list_nth_data(list, i);
	g_variant_builder_add(&builder_source_ports, "(ss)",
			      fw_port_getPort(port),
			      fw_port_getProtocol(port));
    }

    return _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "addZone",
	g_variant_new("(s(sssbsasa(ss)asba(ssss)asasasasa(ss)b))",
		      zone,
		      fw_zone_getVersion(settings),
		      fw_zone_getShort(settings),
		      fw_zone_getDescription(settings),
		      FALSE,
		      fw_zone_getTarget(settings),
		      &builder_services,
		      &builder_ports,
		      &builder_icmpblocks,
		      fw_zone_getMasquerade(settings),
		      &builder_forward_ports,
		      &builder_interfaces,
		      &builder_sources,
		      &builder_rich_rules,
		      &builder_protocols,
		      &builder_source_ports,
		      fw_zone_getIcmpBlockInversion(settings)));
#endif
}

/**
 * fw_config_listZones:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_config_listZones(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_obj_list(fw, fw->proxy,
						   "listZones", NULL);
}

/**
 * fw_config_getZoneNames:
 *
 * Returns: (transfer none) (allow-none) (element-type gchar*)
 */
GList *
fw_config_getZoneNames(FWConfig *obj)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    return _fw_config_proxy_call_sync_get_str_list(fw, fw->proxy,
						   "getZoneNames", NULL);
}

/**
 * fw_config_getZoneByName:
 *
 * Returns: (transfer none) (allow-none) (type FWConfigZone*)
 */
FWConfigZone *
fw_config_getZoneByName(FWConfig *obj,
			const gchar *zone)
{
    FWConfigPrivate *fw = FW_CONFIG_GET_PRIVATE(obj);

    gchar *path = (gchar *) _fw_config_proxy_call_sync_get_obj(
	fw, fw->proxy, "getZoneByName",
	g_variant_new("(s)", zone));

    return fw_config_zone_new(path);
}
