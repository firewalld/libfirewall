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


SOURCES = firewallctl.c
PROGRAMS = $(SOURCES:.c=)
HEADERS = firewallctl.h

CC = gcc
C_INCLUDES = `pkg-config --cflags gio-2.0`
CFLAGS = $(C_INCLUDES) -g -O -Wall -std=c11 -I../src/
LIBS = `pkg-config --libs gio-2.0` ../src/libfirewall.la

NAMESPACE=firewall

all: ../src/libfirewall.la $(PROGRAMS)

firewallctl: firewallctl.o
	libtool link $(CC) $(CFLAGS) $< -o $@ $(LIBS)

clean:
	-rm -f *.o *~ $(PROGRAMS)
	-rm -rf .libs
