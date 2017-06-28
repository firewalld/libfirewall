/*
 * Copyright (C) 2015 Red Hat, Inc.
 *
 * Authors:
 * Jiri Popelka <jpopelka@redhat.com>
 * Jiri Klimes <jklimes@redhat.com>
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

#ifndef FIREWALLCTL_H_
#define FIREWALLCTL_H_

#include <glib.h>
#include "fw_client.h"


/* firewallctl exit codes */
typedef enum {
	/* Indicates successful execution */
	FWC_RESULT_SUCCESS = 0,

	/* Unknown / unspecified error */
	FWC_RESULT_ERROR_UNKNOWN = 1,

	/* Wrong invocation of firewallctl */
	FWC_RESULT_ERROR_USER_INPUT = 2,

} FWCResultCode;

/* FwCtl - main structure */
typedef struct _FwCtl {
	FWClient *fw;                          /* Client object */
	int return_value;                      /* Return code of firewallctl */
	GString *return_text;                  /* Reason text */
	int timeout;                           /* Operation timeout */
	gboolean quiet_flag;                   /* '--quiet' option */
} FwCtl;


#endif
