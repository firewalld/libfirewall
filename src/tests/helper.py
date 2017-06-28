#!/usr/bin/python

import gi
gi.require_version('firewall', '0.1')
from gi.repository import firewall
#from dbus.exceptions import DBusException
from firewall.errors import FirewallError
#from gi.repository import GDBus

fw = firewall.Client()

ipset = fw.getIPSetSettings("whitelist")
if ipset is None:
    pass
else:
    pass

print("DONE")
