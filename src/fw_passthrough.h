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

#ifndef __FW_PASSTHROUGH_H__
#define __FW_PASSTHROUGH_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"

#define FW_PASSTHROUGH_TYPE            (fw_passthrough_get_type())
#define FW_PASSTHROUGH(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_PASSTHROUGH_TYPE, FWPassthrough))
#define FW_PASSTHROUGH_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_PASSTHROUGH_TYPE, FWPassthroughClass))
#define FW_IS_PASSTHROUGH(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_PASSTHROUGH_TYPE, FWPassthroughClass))
#define FW_PASSTHROUGH_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_PASSTHROUGH_TYPE, FWPassthroughClass))

typedef struct {
    GObject parent;
} FWPassthrough;

typedef struct {
    GObjectClass parent;
} FWPassthroughClass;

GType fw_passthrough_get_type(void);
FWPassthrough *fw_passthrough_new(void);

gchar *fw_passthrough_getIpv(FWPassthrough *obj);
GList *fw_passthrough_getArgs(FWPassthrough *obj);

void fw_passthrough_setIpv(FWPassthrough *obj, char *ipv);
void fw_passthrough_setArgs(FWPassthrough *obj, GList *args);

#endif /* __FW_PASSTHROUGH_H__ */
