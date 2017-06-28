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

#ifndef __FW_TYPES_H__
#define __FW_TYPES_H__

#include <glib.h>

typedef struct {
    gchar *version;           /* string */
    gchar *short_description; /* string */
    gchar *description;       /* string */
    gchar *target;            /* string */
    GList *services;          /* list of string */
    GList *ports;             /* list of FWPort */
    GList *icmp_blocks;       /* string list */
    gboolean masquerade;      /* boolean */
    GList *forward_ports;     /* list of FWForwardPort */
    GList *interfaces;        /* list of string */
    GList *sources;           /* list of string */
    GList *rules_str;         /* list of string */
} FWZone2;

#endif /* __FW_TYPES_H__ */
