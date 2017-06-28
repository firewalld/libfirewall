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

#ifndef __FIREWALL_H__
#define __FIREWALL_H__

#if ENABLE_NLS
# include <libintl.h>
# define _(S) gettext(S)
#else
# define _(S) (S)
#endif

#define FW_DEBUG 1

#define FW_DBUS_INTERFACE_VERSION  1
#define FW_DBUS_INTERFACE_REVISION  1

#define FW_DBUS_NAME                      "org.fedoraproject.FirewallD1"

#define FW_DBUS_INTERFACE                 "org.fedoraproject.FirewallD1"
#define FW_DBUS_INTERFACE_ZONE            "org.fedoraproject.FirewallD1.zone"
#define FW_DBUS_INTERFACE_IPSET           "org.fedoraproject.FirewallD1.ipset"
#define FW_DBUS_INTERFACE_DIRECT          "org.fedoraproject.FirewallD1.direct"
#define FW_DBUS_INTERFACE_POLICIES        "org.fedoraproject.FirewallD1.policies"
#define FW_DBUS_INTERFACE_CONFIG          "org.fedoraproject.FirewallD1.config"
#define FW_DBUS_INTERFACE_CONFIG_ZONE     "org.fedoraproject.FirewallD1.config.zone"
#define FW_DBUS_INTERFACE_CONFIG_SERVICE  "org.fedoraproject.FirewallD1.config.service"
#define FW_DBUS_INTERFACE_CONFIG_IPSET    "org.fedoraproject.FirewallD1.config.ipset"
#define FW_DBUS_INTERFACE_CONFIG_ICMPTYPE "org.fedoraproject.FirewallD1.config.icmptype"
#define FW_DBUS_INTERFACE_CONFIG_POLICIES "org.fedoraproject.FirewallD1.config.policies"
#define FW_DBUS_INTERFACE_CONFIG_DIRECT   "org.fedoraproject.FirewallD1.config.direct"
#define FW_DBUS_INTERFACE_CONFIG_HELPER   "org.fedoraproject.FirewallD1.config.helper"

#define FW_DBUS_PATH                      "/org/fedoraproject/FirewallD1"
#define FW_DBUS_PATH_CONFIG               "/org/fedoraproject/FirewallD1/config"
#define FW_DBUS_PATH_CONFIG_ICMPTYPE      "/org/fedoraproject/FirewallD1/config/icmptype"
#define FW_DBUS_PATH_CONFIG_SERVICE       "/org/fedoraproject/FirewallD1/config/service"
#define FW_DBUS_PATH_CONFIG_ZONE          "/org/fedoraproject/FirewallD1/config/zone"
#define FW_DBUS_PATH_CONFIG_HELPER          "/org/fedoraproject/FirewallD1/config/helper"

#endif /* __FIREWALL_H__ */
