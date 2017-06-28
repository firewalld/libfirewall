#!/usr/bin/python

import gi
gi.require_version('firewall', '0.1')
from gi.repository import firewall

fw = firewall.Client()

print("Authorize All")
fw.authorizeAll()

###############

items = fw.listHelpers()
print items

items = fw.config().listHelpers()
print items

items = fw.config().getHelperNames()
print items

item = fw.config().getHelperByName("tftp")
print item

###############

items = fw.listIcmpTypes()
print items

items = fw.config().listIcmpTypes()
print items

items = fw.config().getIcmpTypeNames()
print items

item = fw.config().getIcmpTypeByName("echo-request")
print item

###############

items = fw.listIPSets()
print items

items = fw.config().listIPSets()
print items

items = fw.config().getIPSetNames()
print items

item = fw.config().getIPSetByName("tftp")
print item

###############

items = fw.listServices()
print items

items = fw.config().listServices()
print items

items = fw.config().getServiceNames()
print items

item = fw.config().getServiceByName("tftp")
print item

###############

items = fw.getZones()
print items

items = fw.config().listZones()
print items

items = fw.config().getZoneNames()
print items

item = fw.config().getZoneByName("home")
print item

