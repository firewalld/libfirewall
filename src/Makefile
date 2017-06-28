#
# Copyright (C) 2016,2017 Red Hat, Inc.
#
# Authors:
# Thomas Woerner <twoerner@redhat.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

SUBDIRS = tests

SOURCES = fw_client.c \
	fw_zone.c \
	fw_service.c \
	fw_icmptype.c \
	fw_ipset.c \
	fw_helper.c \
	fw_active_zone.c \
	fw_port.c \
	fw_port_list.c \
	fw_forward_port.c \
	fw_forward_port_list.c \
	fw_direct_simple_rule.c \
	fw_direct_rule.c \
	fw_args.c \
	fw_passthrough.c \
	fw_config.c \
	fw_config_helper.c \
	fw_config_icmptype.c \
	fw_config_ipset.c \
	fw_config_service.c \
	fw_config_zone.c \
	fw_functions.c

OBJECTS = $(SOURCES:.c=.lo)
HEADERS = $(SOURCES:.c=.h) firewall.h

CC = gcc
C_INCLUDES = `pkg-config --cflags gio-2.0`
CFLAGS = $(C_INCLUDES) -g -O -Wall -std=c11
LIBS = `pkg-config --libs gio-2.0`
LIBDIR = /usr/lib64

NAMESPACE=firewall
NSVERSION=0.1
GIR_FILE=$(NAMESPACE)-$(NSVERSION).gir
TYPELIB_FILE=$(NAMESPACE)-$(NSVERSION).typelib

all: libfirewall.la $(TYPELIB_FILE)
	for i in $(SUBDIRS); do \
		echo "making all" "in $(CURRENT_DIR)/$$i..."; \
		$(MAKE) -C $$i all; \
	done

libfirewall.la: $(OBJECTS)
	libtool link $(CC) $(LIBS) -rpath $(LIBDIR) $(OBJECTS) -o $@

$(TYPELIB_FILE): $(GIR_FILE)
	g-ir-compiler $(GIR_FILE) --output=$(TYPELIB_FILE)

$(GIR_FILE): $(SOURCES) $(HEADERS)
	libtool exec g-ir-scanner $^ --warn-all --library=firewall $(C_INCLUDES) --include=GObject-2.0 --namespace=$(NAMESPACE) --identifier-prefix=FW --symbol-prefix=fw_ --nsversion=$(NSVERSION) --output=$@

%.lo: %.c %.h
	libtool compile $(CC) $(CFLAGS) -c $< -o $@

clean:
	for i in $(SUBDIRS); do \
		echo "making clean" "in $(CURRENT_DIR)/$$i..."; \
		$(MAKE) -C $$i clean; \
	done
	-rm -f *.lo *.o libfirewall.la $(TYPELIB_FILE) $(GIR_FILE) *~
	-rm -rf .libs
