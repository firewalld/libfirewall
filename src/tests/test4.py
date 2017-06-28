#!/usr/bin/python

import gi
gi.require_version('firewall', '0.1')
from gi.repository import firewall

fw = firewall.Client()

print("Authorize All")
fw.authorizeAll()

###############

print("\nHELPER\n")

new_item = firewall.Helper()
new_item.setVersion("ver")
new_item.setShort("short")
new_item.setDescription("desc")
new_item.setModule("nf_conntrack_foo")
new_item.addPort("432", "tcp")
new_item.addPort("4321", "udp")
print(fw.config().addHelper("foo", new_item))

test_item = fw.config().getHelperByName("foo")

settings = test_item.getSettings()
settings.addPort("999", "udp")
settings.print_str()
if not settings.queryPort("999", "udp"):
    print("FAIL: Port 999/udp not added")
settings.print_str()

test_item.update(settings)
test_item.rename("foo2")
test_item.addPort("777", "tcp")
if not test_item.queryPort("777", "tcp"):
    print("FAIL: Port 777/tcp not added")

test_item = fw.config().getHelperByName("foo2")
settings = test_item.getSettings()
if not settings.queryPort("777", "tcp"):
    print("FAIL: Update failed")
test_item.remove()

print("\nICMP TYPE\n")

new_item = firewall.IcmpType()
new_item.setVersion("ver")
new_item.setShort("short")
new_item.setDescription("desc")
new_item.addDestination("ipv4")
new_item.print_str()
fw.config().addIcmpType("foo", new_item)

test_item = fw.config().getIcmpTypeByName("foo")

print("getSettings")
settings = test_item.getSettings()
settings.print_str()
settings.addDestination("ipv6")
settings.print_str()

test_item.update(settings)
test_item.rename("foo2")
#test_item.removeDestination("ipv4")
print(test_item.queryDestination("ipv4"))

test_item = fw.config().getIcmpTypeByName("foo2")
print(test_item.queryDestination("ipv4"))
settings = test_item.getSettings()
settings.print_str()
test_item.remove()

print("\nIPSET\n")

#new_item = firewall.IPSet()
#new_item.setVersion("ver")
#new_item.setShort("short")
#new_item.setDescription("desc")
#new_item.setType("hash:net")
#new_item.addEntry("1.2.3.4")
#new_item.print_str()
#fw.config().addIPSet("foo", new_item)






#new_item = firewall.Service()
#new_item.setVersion("ver")
#new_item.setShort("short")
#new_item.setDescription("desc")
#new_item.addPort("21", "tcp")
#new_item.addModule("tftp")
#new_item.setDestination("ipv4", "1.2.3.4")
#new_item.addProtocol("ah")
#new_item.addSourcePort("22", "tcp")
#new_item.print_str()
#fw.config().addService("foo", new_item)

#new_item = firewall.Zone()
#new_item.setVersion("ver")
#new_item.setShort("short")
#new_item.setDescription("desc")
#new_item.setTarget("default")
#new_item.addService("ftp")
#new_item.addPort("21", "tcp")
#new_item.addIcmpBlock("echo-reply")
#new_item.setMasquerade(True)
#new_item.addForwardPort("80", "tcp", "81", "1.3.4.5")
#new_item.addInterface("iface")
#new_item.addSource("2.3.4.5")
#new_item.addRichRule('rule service name="radius" reject')
#new_item.addProtocol("ah")
#new_item.addSourcePort("22", "tcp")
#new_item.print_str()
#fw.config().addZone("foo", new_item)
