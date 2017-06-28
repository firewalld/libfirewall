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

#include <glib.h>
#include <string.h>
#include "fw_client.h"
#include "fw_active_zone.h"
#include "fw_port.h"
#include "fw_port_list.h"
#include "fw_functions.h"

#include "fw_zone.h"

int
main(int argc, char **argv) {
    FWClient *fw;
    const gchar *default_zone;
    GList *list;
    gint i, j;
    GHashTable *hash;
    const char *name;
    /* const gchar *item, *item2, *zone; */

    fw = fw_client_new();

    default_zone = fw_client_getDefaultZone(fw);

    g_print("-------------------------------------------------------------------------------\n");

    g_print("default zone: '%s'\n", default_zone);

    g_print("-------------------------------------------------------------------------------\n");

    g_print("zones:\n\n");
    list = fw_client_getZones(fw);
    for (i=0; i<g_list_length(list); i++) {
	FWZone *zone;
	GList *interfaces, *sources, *rules;
	name = g_list_nth_data(list, i);
	zone = fw_client_getZoneSettings(fw, name);
	interfaces = fw_zone_getInterfaces(zone);
	sources = fw_zone_getSources(zone);

	g_print(name);
	if (strcmp(name, default_zone) == 0) {
	    g_print(" (default)");
	}
	if (g_list_length(interfaces) > 0 || g_list_length(sources) > 0) {
	    g_print(" (active)");
	}
	g_print("\n");

	g_print("  target: %s\n", fw_zone_getTarget(zone));

	g_print("  icmp-block-inversion: %s\n",
		fw_zone_getIcmpBlockInversion(zone) ? "yes": "no");

	g_print("  interfaces: ");
	fw_str_list_print_simple(interfaces);
	g_print("\n");

	g_print("  sources: ");
	fw_str_list_print_simple(sources);
	g_print("\n");

	g_print("  services: ");
	fw_str_list_print_simple(fw_zone_getServices(zone));
	g_print("\n");

	g_print("  ports: ");
	fw_port_list_print_simple(fw_zone_getPorts(zone));
	g_print("\n");

	g_print("  protocols: ");
	fw_str_list_print_simple(fw_zone_getProtocols(zone));
	g_print("\n");

	g_print("  masquerade: %s\n",
		fw_zone_getMasquerade(zone) ? "yes": "no");

	g_print("  forward-ports:");
	fw_forward_port_list_print_simple(fw_zone_getForwardPorts(zone));
	g_print("\n");

	g_print("  source-ports: ");
	fw_port_list_print_simple(fw_zone_getSourcePorts(zone));
	g_print("\n");

	g_print("  icmp-blocks: ");
	fw_str_list_print_simple(fw_zone_getIcmpBlocks(zone));
	g_print("\n");

	g_print("  rich-rules:\n");
	rules = fw_zone_getRichRules(zone);
	for (j=0; j<g_list_length(rules); j++) {
	    char *str = g_list_nth_data(rules, j);
	    g_print("\t%s\n", str);
	}

    	g_print("\n");
    }
    g_print("\n");

    g_print("-------------------------------------------------------------------------------\n");

    g_print("services:\n\n");
    list = fw_client_listServices(fw);
    for (i=0; i<g_list_length(list); i++) {
	FWService *service;
	GList *keys;
	GHashTable *destinations;
	name = g_list_nth_data(list, i);
	service = fw_client_getServiceSettings(fw, name);

	g_print("%s\n", name);

	g_print("  ports: ");
	fw_port_list_print_simple(fw_service_getPorts(service));
	g_print("\n");

	g_print("  protocols: ");
	fw_str_list_print_simple(fw_service_getProtocols(service));
	g_print("\n");

	g_print("  source-ports: ");
	fw_port_list_print_simple(fw_service_getSourcePorts(service));
	g_print("\n");

	g_print("  modules: ");
	fw_str_list_print_simple(fw_service_getModules(service));
	g_print("\n");

	g_print("  destinations: ");
	destinations = fw_service_getDestinations(service);
	keys = g_hash_table_get_keys(destinations);
	for (j=0; j<g_list_length(keys); j++) {
	    gchar *str = (gchar *) g_list_nth_data(keys, j);
	    g_print("%s%s:%s", (j>0) ? ", " : "", str, 
		    (gchar *)g_hash_table_lookup(destinations, str));
	}
	g_print("\n");

	g_print("\n");
    }
    g_print("\n");

    g_print("-------------------------------------------------------------------------------\n");

    g_print("icmptypes:\n\n");
    list = fw_client_listIcmpTypes(fw);
    for (i=0; i<g_list_length(list); i++) {
	FWIcmpType *icmp;
	name = g_list_nth_data(list, i);
	icmp = fw_client_getIcmpTypeSettings(fw, name);

	g_print("%s\n", name);

	g_print("  destinations: ");
	fw_str_list_print_simple(fw_icmptype_getDestinations(icmp));
	g_print("\n");

	g_print("\n");
    }
    g_print("\n");

    g_print("-------------------------------------------------------------------------------\n");

    g_print("ipsets:\n\n");
    list = fw_client_listIPSets(fw);
    for (i=0; i<g_list_length(list); i++) {
	FWIPSet *ipset;
	GList *keys;
	GHashTable *options;
	name = g_list_nth_data(list, i);
	ipset = fw_client_getIPSetSettings(fw, name);

	g_print("%s\n", name);

	g_print("  type: %s\n", fw_ipset_getType(ipset));

	g_print("  options:");
	options = fw_ipset_getOptions(ipset);
	keys = g_hash_table_get_keys(options);
	for (i=0; i<g_list_length(keys); i++) {
	    gchar *str = (gchar *) g_list_nth_data(keys, i);
	    g_print(" %s=%s", str,
		    (gchar *)g_hash_table_lookup(options, str));
	}
	g_print("\n");

	g_print("  entries: ");
	fw_str_list_print_simple(fw_ipset_getEntries(ipset));
	g_print("\n");

	g_print("\n");
    }
    g_print("\n");
    g_print("\n");

    g_print("-------------------------------------------------------------------------------\n");

    g_print("helpers:\n\n");
    list = fw_client_listHelpers(fw);
    for (i=0; i<g_list_length(list); i++) {
	FWHelper *helper;
	name = g_list_nth_data(list, i);
	helper = fw_client_getHelperSettings(fw, name);

	g_print("%s\n", name);

	g_print("  family: %s\n", fw_helper_getFamily(helper));

	g_print("  module: %s\n", fw_helper_getModule(helper));

	g_print("  ports: ");
	fw_port_list_print_simple(fw_helper_getPorts(helper));
	g_print("\n");

    	g_print("\n");
    }
    g_print("\n");

    g_print("-------------------------------------------------------------------------------\n");

    hash = fw_client_getActiveZones(fw);
    {
	GHashTableIter iter;
	const char *zone_name;
	FWActiveZone *active_zone;

	GList *interfaces, *sources;

	g_print("active zones: {\n");

	g_hash_table_iter_init(&iter, hash);
	while (g_hash_table_iter_next(&iter,
				      (gpointer) &zone_name,
				      (gpointer) &active_zone))
	{
	    g_print("%s:\n", zone_name);

	    interfaces = fw_active_zone_getInterfaces(active_zone);
	    g_print("  interfaces: ");
	    fw_str_list_print_simple(interfaces);
	    g_print("\n");

	    sources = fw_active_zone_getSources(active_zone);
	    g_print("  sources: ");
	    fw_str_list_print_simple(sources);
	    g_print("\n");
	}
	g_print("}\n");
    }

    return 0;
}
