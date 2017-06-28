#!/usr/bin/python

import gi
gi.require_version('firewall', '0.1')
from gi.repository import firewall

fw = firewall.Client()

print("Authorize All")
fw.authorizeAll()

print("\n-------------------------------------------------------------------------------\n")

#print("Reload")
#fw.reload()
#print("Complete Reload")
#fw.completeReload()

#print("\n-------------------------------------------------------------------------------\n")

#print("Runtime To Permanent")
#fw.runtimeToPermanent()

#print("\n-------------------------------------------------------------------------------\n")

#print("panic mode: %s" % fw.queryPanicMode())
#fw.enablePanicMode()
#print(fw.queryPanicMode())
#fw.disablePanicMode()
#print(fw.queryPanicMode())

#print("\n-------------------------------------------------------------------------------\n")

default_zone = fw.getDefaultZone()
print("default zone: '%s'" % default_zone)
#fw.setDefaultZone("work")
#print(fw.getDefaultZone())
#fw.setDefaultZone(default_zone)
#print(fw.getDefaultZone())

print("-------------------------------------------------------------------------------\n")

print fw.getZoneSettings("public")

fw.getZoneSettings("public").print_str()

print("\n-------------------------------------------------------------------------------\n")

print(fw.listServices())

print("\n-------------------------------------------------------------------------------\n")

fw.getServiceSettings("mdns").print_str()

fw.getServiceSettings("tftp").print_str()

print("\n-------------------------------------------------------------------------------\n")

l = fw.listIPSets()
print(l)

print("\n-------------------------------------------------------------------------------\n")

if len(l) > 0:
    fw.getIPSetSettings(l[0]).print_str()

print("\n-------------------------------------------------------------------------------\n")

print(fw.listIcmpTypes())

print("\n-------------------------------------------------------------------------------\n")

fw.getIcmpTypeSettings("source-quench").print_str()

print("\n-------------------------------------------------------------------------------\n")

print(fw.getZones())

print("\n-------------------------------------------------------------------------------\n")

active_zones = fw.getActiveZones()
for zone in active_zones:
    print("%s:" % zone)
    print("  'interfaces': %s" % active_zones[zone].getInterfaces())
    print("  'sources': %s" % active_zones[zone].getSources())

print("\n-------------------------------------------------------------------------------\n")

interface="em1"
print("Zone Of Interface '%s': '%s'" % (interface, fw.getZoneOfInterface(interface)))
source = "192.168.254.253"
print("Zone Of Source '%s': '%s'" % (source, fw.getZoneOfSource(source)))

print("\n-------------------------------------------------------------------------------\n")

zone = ""
item = "foobar"
print("add interface '%s' to zone '%s': '%s'" % (item, zone,
                                                 fw.addInterface(zone, item)))

zone = "work"
print("change zone of interface '%s' to zone '%s': '%s'" % (item, zone,
                                                            fw.changeZoneOfInterface(zone, item)))

print("is interface '%s' bound to zone '%s'? %s" % (item, zone,
                                                    "yes" if fw.queryInterface(zone, item) else "no"))

print("interfaces in zone '%s': %s" % (zone, fw.getInterfaces(zone)))

print("remove interface '%s' from zone '%s': '%s'" % (item, zone,
                                                      fw.removeInterface(zone, item)))

print("interfaces in zone '%s' %s: " % (zone, fw.getInterfaces(zone)))

print("\n-------------------------------------------------------------------------------\n")

zone = ""
item = "192.168.254.252"
print("add source '%s' to zone '%s': '%s'" % (item, zone,
                                              fw.addSource(zone, item)))

zone = "work"
print("change zone of source '%s' to zone '%s': '%s'" % (item, zone,
                                                         fw.changeZoneOfSource(zone, item)))

print("is source '%s' bound to zone '%s'? %s" % (item, zone,
                                                 "yes" if fw.querySource(zone, item) else "no"))

print("sources in zone '%s': %s" % (zone, fw.getSources(zone)))

print("remove source '%s' from zone '%s': '%s'" % (item, zone,
                                                   fw.removeSource(zone, item)))

print("sources in zone '%s' %s: " % (zone, fw.getSources(zone)))

print("\n-------------------------------------------------------------------------------\n")

zone = "work"
item = "rule service name=ftp audit limit value='1/m' accept"
print("add rich rule '%s' to zone '%s': '%s'" % (item, zone,
                                                 fw.addRichRule(zone, item, 0)))

print("is rich rule '%s' in zone '%s'? %s\n" % (item, zone,
                                                "yes" if fw.queryRichRule(zone, item) else "no"))

print("rich rules in zone '%s': %s" % (zone, fw.getRichRules(zone)))

print("remove rich rule '%s' from zone '%s': '%s'\n" % (item, zone,
                                                        fw.removeRichRule(zone, item)))

print("rich rules in zone '%s': %s" % (zone, fw.getRichRules(zone)))

print("-------------------------------------------------------------------------------\n")

zone = "work"
item = "tftp"
print("add service '%s' to zone '%s': '%s'\n" % (item, zone,
                                                 fw.addService(zone, item, 0)))

print("is service '%s' in zone '%s'? %s\n" % (item, zone,
                                              "yes" if fw.queryService(zone, item) else "no"))

print("services in zone '%s': %s" % (zone, fw.getServices(zone)))

print("remove service '%s' from zone '%s': '%s'\n" % (item, zone, fw.removeService(zone, item)))

print("services in zone '%s': %s" % (zone, fw.getServices(zone)))

print("\n-------------------------------------------------------------------------------\n")

zone = "work"
item = "70"
item2 = "tcp"
print("add port '%s' to zone '%s': '%s'" % (item, zone,
                                            fw.addPort(zone, item, item2, 0)))

print("is port '%s' in zone '%s'? %s" % (item, zone,
                                         "yes" if fw.queryPort(zone, item, item2) else "no"))

print("ports in zone '%s':  [%s]" % (zone, ", ".join(z.getStr() for z in fw.getPorts(zone))))

print("remove port '%s' from zone '%s': '%s'" % (item, zone,
                                                   fw.removePort(zone, item, item2)))

print("ports in zone '%s':  [%s]" % (zone, ", ".join(z.getStr() for z in fw.getPorts(zone))))

print("\n-------------------------------------------------------------------------------\n")

print("add masquerade zone '%s': '%s'" % (zone, fw.addMasquerade(zone, 0)))

print("is masquerade enabled in zone '%s'? %s" % (zone,
                                                  "yes" if fw.queryMasquerade(zone) else "no"))

print("remove masquerade from zone '%s': '%s'" % (zone,
                                                    fw.removeMasquerade(zone)))

print("is masquerade enabled in zone '%s'? %s" % (zone,
                                                  "yes" if fw.queryMasquerade(zone) else "no"))

print("\n-------------------------------------------------------------------------------\n")

zone = "work"
item = "70"
item2 = "tcp"
item3 = "71"
item4 = "192.168.254.251"
print("add forward port '%s' to zone '%s': '%s'" % (item, zone,
                                                    fw.addForwardPort(zone, item, item2, item3, item4, 0)))

print("is forward port '%s' in zone '%s'? %s" % (item, zone,
                                                 "yes" if fw.queryForwardPort(zone, item, item2, item3, item4) else "no"))

print("forward ports in zone '%s':  [%s]" % (zone, ", ".join(z.getStr() for z in fw.getForwardPorts(zone))))

print("remove forward port '%s' from zone '%s': '%s'" % (item, zone,
                                                         fw.removeForwardPort(zone, item, item2, item3, item4)))

print("forward ports in zone '%s':  [%s]" % (zone, ", ".join(z.getStr() for z in fw.getForwardPorts(zone))))

print("\n-------------------------------------------------------------------------------\n")

zone = "work"
item = "time-exceeded"
print("add icmpblock '%s' to zone '%s': '%s'" % (item, zone,
                                                 fw.addIcmpBlock(zone, item, 0)))

print("is icmpblock '%s' in zone '%s'? %s" % (item, zone,
                                              "yes" if fw.queryIcmpBlock(zone, item) else "no"))

print("icmpblocks in zone '%s': %s" % (zone, fw.getIcmpBlocks(zone)))

print("remove icmpblock '%s' from zone '%s': '%s'" % (item, zone, fw.removeIcmpBlock(zone, item)))

print("icmpblocks in zone '%s': %s\n" % (zone, fw.getIcmpBlocks(zone)))

print("-------------------------------------------------------------------------------\n")

ipv = "ipv4"
table = "filter"
chain = "my_test_chain"
print("add chain '%s' to ipv '%s' and table '%s'" % (chain, ipv, table))
fw.addChain(ipv, table, chain)

print("is chain '%s' in ipv '%s' and table '%s': %s" % \
      (chain, ipv, table, fw.queryChain(ipv, table, chain)))

print("chains in ipv '%s' and table '%s': %s" % (ipv, table, fw.getChains(ipv, table)))

print("remove chain '%s' from ipv '%s' and table '%s'" % (chain, ipv, table))
fw.removeChain(ipv, table, chain)

print("chains in ipv '%s' and table '%s': %s" % (ipv, table, fw.getChains(ipv, table)))

print("\n-------------------------------------------------------------------------------\n")

ipv = "ipv4"
table = "filter"
chain = "my_test_chain"
print("add chain '%s' to ipv '%s' and table '%s'" % (chain, ipv, table))
fw.addChain(ipv, table, chain)

print("is chain '%s' in ipv '%s' and table '%s': %s" % \
      (chain, ipv, table, fw.queryChain(ipv, table, chain)))

print("chains in ipv '%s' and table '%s': %s" % (ipv, table, fw.getChains(ipv, table)))

print("remove chain '%s' from ipv '%s' and table '%s'" % (chain, ipv, table))
fw.removeChain(ipv, table, chain)

print("chains in ipv '%s' and table '%s': %s" % (ipv, table, fw.getChains(ipv, table)))

print("\n-------------------------------------------------------------------------------\n")

ipv = "ipv4"
table = "filter"
chain = "INPUT"
priority = 2
args = [ "-s", "1.2.3.4", "-j", "DROP" ]
print("add rule %s to ipv '%s', table '%s', chain '%s' with priority %d" % (args, ipv, table, chain, priority))
fw.addRule(ipv, table, chain, priority, args)

print("query if rule %s is in ipv '%s', table '%s', chain '%s' and priority %d: %s" % (args, ipv, table, chain, priority, fw.queryRule(ipv, table, chain, priority, args)))

print("rules in ipv '%s', table '%s', chain '%s': %s" % (ipv, table, chain, [ (rule.getPriority(), rule.getArgs()) for rule in fw.getRules(ipv, table, chain)] ))

print("all rules: %s" % ([ (rule.getIpv(), rule.getTable(), rule.getChain(), rule.getPriority(), rule.getArgs()) for rule in fw.getAllRules()] ))

print("remove rule %s from ipv '%s', table '%s', chain '%s' with priority %d" % (args, ipv, table, chain, priority))
fw.removeRule(ipv, table, chain, priority, args)

print("query if rule %s is in ipv '%s', table '%s', chain '%s' and priority %d: %s" % (args, ipv, table, chain, priority, fw.queryRule(ipv, table, chain, priority, args)))

print("rules in ipv '%s', table '%s', chain '%s': %s" % (ipv, table, chain, fw.getRules(ipv, table, chain)))

print("all rules: %s" % ([ (rule.getIpv(), rule.getTable(), rule.getChain(), rule.getPriority(), rule.getArgs()) for rule in fw.getAllRules()] ))

print("\n-------------------------------------------------------------------------------\n")

ipv = "ipv4"
args = [ "-A", "INPUT", "-s", "1.2.3.4", "-j", "DROP" ]
print("passthrough rule %s to ipv '%s': %s" % (args, ipv, fw.passthrough(ipv, args)))

args = [ "-D", "INPUT", "-s", "1.2.3.4", "-j", "DROP" ]
print("passthrough rule %s to ipv '%s': %s" % (args, ipv, fw.passthrough(ipv, args)))

print("\n-------------------------------------------------------------------------------\n")

ipv = "ipv4"
args = [ "-A", "INPUT", "-s", "1.2.3.4", "-j", "DROP" ]
print("add tracked passthrough rule %s with ipv '%s'" % (args, ipv))
fw.addPassthrough(ipv, args)

print("query tracked passthrough rule %s with ipv '%s': %s" % (args, ipv, fw.queryPassthrough(ipv, args)))

print("get tracked passthrough rules with ipv '%s': %s" % (ipv, [ a.getArgs() for a in fw.getPassthroughs(ipv) ]))

print("remove tracked passthrough rule %s with ipv '%s'" % (args, ipv))
fw.removePassthrough(ipv, args)

print("query tracked passthrough rule %s with ipv '%s': %s" % (args, ipv, fw.queryPassthrough(ipv, args)))

print("get tracked passthrough rules with ipv '%s': %s\n" % (ipv, fw.getPassthroughs(ipv)))

ipv = "ipv4"
args = [ "-A", "INPUT", "-s", "1.2.3.4", "-j", "DROP" ]
print("add tracked passthrough rule %s with ipv '%s'" % (args, ipv))
fw.addPassthrough(ipv, args)
ipv = "ipv4"
args = [ "-A", "INPUT", "-s", "1.2.3.5", "-j", "ACCEPT" ]
print("add tracked passthrough rule %s with ipv '%s'" % (args, ipv))
fw.addPassthrough(ipv, args)
ipv = "ipv6"
args = [ "-A", "INPUT", "-s", "1:2:3::", "-j", "ACCEPT" ]
print("add tracked passthrough rule %s with ipv '%s'" % (args, ipv))
fw.addPassthrough(ipv, args)

print("get all passthrough rules: %s" % ([ [ p.getIpv(), p.getArgs() ] for p in fw.getAllPassthroughs() ]))

print("remove all passthrough rules")
fw.removeAllPassthroughs()

print("get all passthrough rules: %s" % ([ [ p.getIpv(), p.getArgs() ] for p in fw.getAllPassthroughs() ]))

print("\n-------------------------------------------------------------------------------\n")

print("enable lockdown")
fw.enableLockdown()

print("query lockdown: %s" % fw.queryLockdown())

print("disable lockdown")
fw.disableLockdown()

print("query lockdown: %s" % fw.queryLockdown())

print("\n-------------------------------------------------------------------------------\n")

print("get lockdown whitelist commands: %s" % fw.getLockdownWhitelistCommands())
command = "/usr/bin/foobar"
print("add lockdown whitelist command '%s'" % command)
fw.addLockdownWhitelistCommand(command)

print("get lockdown whitelist commands: %s" % fw.getLockdownWhitelistCommands())

print("query lockdown whitelist command '%s': %s" % (command, fw.queryLockdownWhitelistCommand(command)))

print("remove lockdown whitelist command '%s'" % command)
fw.removeLockdownWhitelistCommand(command)

print("query lockdown whitelist command '%s': %s" % (command, fw.queryLockdownWhitelistCommand(command)))

print("get lockdown whitelist commands: %s" % fw.getLockdownWhitelistCommands())

print("\n-------------------------------------------------------------------------------\n")

print("get lockdown whitelist contexts: %s" % fw.getLockdownWhitelistContexts())
context = "system_u:system_r:MadDaemon_t:s0"
print("add lockdown whitelist context '%s'" % context)
fw.addLockdownWhitelistContext(context)

print("get lockdown whitelist contexts: %s" % fw.getLockdownWhitelistContexts())

print("query lockdown whitelist context '%s': %s" % (context, fw.queryLockdownWhitelistContext(context)))

print("remove lockdown whitelist context '%s'" % context)
fw.removeLockdownWhitelistContext(context)

print("query lockdown whitelist context '%s': %s" % (context, fw.queryLockdownWhitelistContext(context)))

print("get lockdown whitelist contexts: %s" % fw.getLockdownWhitelistContexts())

print("\n-------------------------------------------------------------------------------\n")

print("get lockdown whitelist uids: %s" % fw.getLockdownWhitelistUids())
uid = 6666
print("add lockdown whitelist uid '%s'" % uid)
fw.addLockdownWhitelistUid(uid)

print("get lockdown whitelist uids: %s" % fw.getLockdownWhitelistUids())

print("query lockdown whitelist uid '%s': %s" % (uid, fw.queryLockdownWhitelistUid(uid)))

print("remove lockdown whitelist uid '%s'" % uid)
fw.removeLockdownWhitelistUid(uid)

print("query lockdown whitelist uid '%s': %s" % (uid, fw.queryLockdownWhitelistUid(uid)))

print("get lockdown whitelist uids: %s" % fw.getLockdownWhitelistUids())

print("\n-------------------------------------------------------------------------------\n")

print("get lockdown whitelist users: %s" % fw.getLockdownWhitelistUsers())
user = "theboss"
print("add lockdown whitelist user '%s'" % user)
fw.addLockdownWhitelistUser(user)

print("get lockdown whitelist users: %s" % fw.getLockdownWhitelistUsers())

print("query lockdown whitelist user '%s': %s" % (user, fw.queryLockdownWhitelistUser(user)))

print("remove lockdown whitelist user '%s'" % user)
fw.removeLockdownWhitelistUser(user)

print("query lockdown whitelist user '%s': %s" % (user, fw.queryLockdownWhitelistUser(user)))

print("get lockdown whitelist users: %s" % fw.getLockdownWhitelistUsers())

print("\n-------------------------------------------------------------------------------\n")

