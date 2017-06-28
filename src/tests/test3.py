#!/usr/bin/python

import gi
gi.require_version('firewall', '0.1')
from gi.repository import firewall

fw = firewall.Client()

print("Authorize All")
fw.authorizeAll()

###############

print("Helper\n")

new_item = firewall.Helper()
new_item.setVersion("ver")
new_item.setShort("short")
new_item.setDescription("desc")
new_item.setModule("nf_conntrack_foo")
new_item.addPort("432", "tcp")
new_item.addPort("4321", "udp")
new_item.print_str()
fw.config().addHelper("foo", new_item)

print("IcmpType\n")

new_item = firewall.IcmpType()
new_item.setVersion("ver")
new_item.setShort("short")
new_item.setDescription("desc")
new_item.addDestination("ipv4")
new_item.print_str()
fw.config().addIcmpType("foo", new_item)

print("IPSet\n")

new_item = firewall.IPSet()
new_item.setVersion("ver")
new_item.setShort("short")
new_item.setDescription("desc")
new_item.setType("hash:net")
new_item.addEntry("1.2.3.4")
new_item.print_str()
fw.config().addIPSet("foo", new_item)

print("Service\n")

new_item = firewall.Service()
new_item.setVersion("ver")
new_item.setShort("short")
new_item.setDescription("desc")
new_item.addPort("21", "tcp")
new_item.addModule("tftp")
new_item.setDestination("ipv4", "1.2.3.4")
new_item.addProtocol("ah")
new_item.addSourcePort("22", "tcp")
new_item.print_str()
fw.config().addService("foo", new_item)

print("Zone\n")

new_item = firewall.Zone()
new_item.setVersion("ver")
new_item.setShort("short")
new_item.setDescription("desc")
new_item.setTarget("default")
new_item.addService("ftp")
new_item.addPort("21", "tcp")
new_item.addIcmpBlock("echo-reply")
new_item.setMasquerade(True)
new_item.addForwardPort("80", "tcp", "81", "1.3.4.5")
new_item.addInterface("iface")
new_item.addSource("2.3.4.5")
new_item.addRichRule('rule service name="radius" reject')
new_item.addProtocol("ah")
new_item.addSourcePort("22", "tcp")
new_item.print_str()
fw.config().addZone("foo", new_item)
