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

#ifndef __FW_ARGS_H__
#define __FW_ARGS_H__

#include <glib.h>
#include <glib-object.h>
#include "firewall.h"

#define FW_ARGS_TYPE            (fw_args_get_type())
#define FW_ARGS(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), FW_ARGS_TYPE, FWArgs))
#define FW_ARGS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), FW_ARGS_TYPE, FWArgsClass))
#define FW_IS_ARGS(klass)       (G_TYPE_CHECK_INSTANCE_CLASS((klass), FW_ARGS_TYPE, FWArgsClass))
#define FW_ARGS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), FW_ARGS_TYPE, FWArgsClass))

typedef struct {
    GObject parent;
} FWArgs;

typedef struct {
    GObjectClass parent;
} FWArgsClass;

GType fw_args_get_type(void);
FWArgs *fw_args_new(void);

GList *fw_args_getArgs(FWArgs *obj);
void fw_args_setArgs(FWArgs *obj, GList *args);
void fw_args_addArg(FWArgs *obj, gchar *arg);

#endif /* __FW_ARGS_H__ */
