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

#include <glib.h>
#include "fw_client.h"
#include "fw_active_zone.h"
#include "fw_port.h"
#include "fw_functions.h"

int
main(int argc, char **argv) {
    FWClient *fw;
    GList *list;
    GHashTable *hash;
    const gchar *item, *item2, *zone;

    fw = fw_client_new();

    g_print("-------------------------------------------------------------------------------\n");

    g_print("default zone: '%s'\n", fw_client_getDefaultZone(fw));

    g_print("-------------------------------------------------------------------------------\n");

/*
    g_print("zones: ");
    list = fw_client_getZones(fw);
    fw_str_list_print(list);
    g_print("\n");
*/

/*
    g_print("-------------------------------------------------------------------------------\n");

    g_print("services: ");
    list = fw_client_listServices(fw);
    fw_str_list_print(list);
    g_print("\n");
*/

/*
    g_print("-------------------------------------------------------------------------------\n");

    g_print("icmptypes: ");
    list = fw_client_listIcmpTypes(fw);
    fw_str_list_print(list);
    g_print("\n");
*/

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
	    g_print("  '%s':\n", zone_name);

	    interfaces = fw_active_zone_getInterfaces(active_zone);
	    g_print("    'interfaces': ");
	    fw_str_list_print(interfaces);
	    g_print("\n");

	    sources = fw_active_zone_getSources(active_zone);
	    g_print("    'sources': ");
	    fw_str_list_print(sources);
	    g_print("\n");
	}
	g_print("}\n");
    }

/*
    g_print("-------------------------------------------------------------------------------\n");

    item = "em1";
    g_print("zone of interface '%s': '%s'\n", item,
	    fw_client_getZoneOfInterface(fw, item));

    item = "192.168.254.253";
    g_print("zone of source '%s': '%s'\n", item,
	    fw_client_getZoneOfSource(fw, item));
*/

/*
    g_print("-------------------------------------------------------------------------------\n");

    zone = "";
    item = "foobar";
    g_print("add interface '%s' to zone '%s': '%s'\n", item, zone,
	    fw_client_addInterface(fw, zone, item));

    zone = "work";
    g_print("change zone of interface '%s' to zone '%s': '%s'\n", item, zone,
	    fw_client_changeZoneOfInterface(fw, zone, item));

    g_print("is interface '%s' bound to zone '%s'? %s\n", item, zone,
	    fw_client_queryInterface(fw, zone, item) == TRUE ? "yes": "no");

    g_print("interfaces in zone '%s': ", zone);
    list = fw_client_getInterfaces(fw, zone);
    fw_str_list_print(list);
    g_print("\n");

    g_print("remove interface '%s' from zone '%s': '%s'\n", item, zone,
	    fw_client_removeInterface(fw, zone, item));

    g_print("interfaces in zone '%s': ", zone);
    list = fw_client_getInterfaces(fw, zone);
    fw_str_list_print(list);
    g_print("\n");

    g_print("-------------------------------------------------------------------------------\n");
*/

/*
    zone = "";
    item = "192.168.254.252";
    g_print("add source '%s' to zone '%s': '%s'\n", item, zone,
	    fw_client_addSource(fw, zone, item));

    zone = "work";
    g_print("change zone of source '%s' to zone '%s': '%s'\n", item, zone,
	    fw_client_changeZoneOfSource(fw, zone, item));

    g_print("is source '%s' bound to zone '%s'? %s\n", item, zone,
	    fw_client_querySource(fw, zone, item) == TRUE ? "yes": "no");

    g_print("sources in zone '%s': ", zone);
    list = fw_client_getSources(fw, zone);
    fw_str_list_print(list);
    g_print("\n");

    g_print("remove source '%s' from zone '%s': '%s'\n", item, zone,
	    fw_client_removeSource(fw, zone, item));

    g_print("sources in zone '%s': ", zone);
    list = fw_client_getSources(fw, zone);
    fw_str_list_print(list);
    g_print("\n");

    g_print("-------------------------------------------------------------------------------\n");
*/

/*
    zone = "work";
    item = "rule service name=ftp audit limit value='1/m' accept";
    g_print("add rich rule '%s' to zone '%s': '%s'\n", item, zone,
	    fw_client_addRichRule(fw, zone, item, 0));

    g_print("is rich rule '%s' in zone '%s'? %s\n", item, zone,
	    fw_client_queryRichRule(fw, zone, item) == TRUE ? "yes": "no");

    g_print("rich rules in zone '%s': ", zone);
    list = fw_client_getRichRules(fw, zone);
    fw_str_list_print(list);
    g_print("\n");

    g_print("remove rich rule '%s' from zone '%s': '%s'\n", item, zone,
	    fw_client_removeRichRule(fw, zone, item));

    g_print("rich rules in zone '%s': ", zone);
    list = fw_client_getRichRules(fw, zone);
    fw_str_list_print(list);
    g_print("\n");

    g_print("-------------------------------------------------------------------------------\n");
*/

/*
    zone = "work";
    item = "tftp";
    g_print("add service '%s' to zone '%s': '%s'\n", item, zone,
	    fw_client_addService(fw, zone, item, 0));

    g_print("is service '%s' in zone '%s'? %s\n", item, zone,
	    fw_client_queryService(fw, zone, item) == TRUE ? "yes": "no");

    g_print("services in zone '%s': ", zone);
    list = fw_client_getServices(fw, zone);
    fw_str_list_print(list);
    g_print("\n");

    g_print("remove service '%s' from zone '%s': '%s'\n", item, zone,
	    fw_client_removeService(fw, zone, item));

    g_print("services in zone '%s': ", zone);
    list = fw_client_getServices(fw, zone);
    fw_str_list_print(list);
    g_print("\n");

    g_print("-------------------------------------------------------------------------------\n");
*/

    zone = "work";
    item = "tftp";
    item2 = "tcp";
    g_print("add port '%s' to zone '%s': '%s'\n", item, zone,
	    fw_client_addPort(fw, zone, item, item2, 0));

    g_print("is port '%s' in zone '%s'? %s\n", item, zone,
	    fw_client_queryPort(fw, zone, item, item2) == TRUE ? "yes": "no");

    g_print("ports in zone '%s': ", zone);
    list = fw_client_getPorts(fw, zone);
    {
	gint i;

	if (list == NULL) {
	    g_print("NULL\n");
	}

	g_print("[ ");
	for (i=0; i<g_list_length(list); i++) {
	    FWPort *prt = g_list_nth_data(list, i);

	    g_print("%s%s/%s", (i>0) ? ", " : "",
		    fw_port_getPort(prt), fw_port_getProtocol(prt));
	}
	g_print(" ]");
    }
    g_print("\n");

    g_print("remove port '%s' from zone '%s': '%s'\n", item, zone,
	    fw_client_removePort(fw, zone, item, item2));

#ifdef FOOBAR
    g_print("ports in zone '%s': ", zone);
/*    list = fw_client_getPorts(fw, zone); */
/*    fw_str_list_print(list); */
    g_print("\n");
#endif

    g_print("-------------------------------------------------------------------------------\n");

    return 0;
}
