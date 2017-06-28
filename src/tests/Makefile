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

SOURCES = test.c \
	fwlist.c \
	fwlist_config.c
PROGRAMS = $(SOURCES:.c=)

CC = gcc
C_INCLUDES = `pkg-config --cflags gio-2.0`
CFLAGS = $(C_INCLUDES) -g -O -Wall -std=c11 -I..
LIBS = `pkg-config --libs gio-2.0` ../libfirewall.la

all: $(PROGRAMS)

test: test.o
	libtool link $(CC) $(CFLAGS) $< -o $@ $(LIBS)

fwlist: fwlist.o
	libtool link $(CC) $(CFLAGS) $< -o $@ $(LIBS)

fwlist_config: fwlist_config.o
	libtool link $(CC) $(CFLAGS) $< -o $@ $(LIBS)

clean:
	-rm -f *.o test *~ $(PROGRAMS)
	-rm -rf .libs
