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

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>

#include "firewallctl.h"

# define FIREWALLCTL_VERSION "0.1"

#define DDD() printf("%s:%s():%d\n",__FILE__,__func__,__LINE__);
#define DDDd(i) printf("%s:%s():%d -- %s=%d\n",__FILE__,__func__,__LINE__,#i,i);
#define DDDs(s) printf("%s:%s():%d -- %s=%s\n",__FILE__,__func__,__LINE__,#s,s);

/* Global FwCtl object */
FwCtl fw_ctl;

int
matches (const char *cmd, const char *pattern)
{
	size_t len = strlen (cmd);
	if (!len || len > strlen (pattern))
		return -1;
	return memcmp (pattern, cmd, len);
}

gboolean
fwc_arg_is_help (const char *arg)
{
	if (!arg)
		return FALSE;
	if (   matches (arg, "help") == 0
	    || (g_str_has_prefix (arg, "-")  && matches (arg+1, "help") == 0)
	    || (g_str_has_prefix (arg, "--") && matches (arg+2, "help") == 0)) {
		return TRUE;
	}
	return FALSE;
}

static void
usage (const char *prog_name)
{
	g_printerr (_("Usage: %s [OPTIONS] OBJECT { COMMAND | help }\n"
	              "\n"
	              "OPTIONS\n"
	              "  -q[uiet]               do not print status messages\n"
	              "  -v[ersion]             show program version\n"
	              "  -h[elp]                print this help\n"
	              "\n"
	              "OBJECT\n"
	              "  z[one]                 zone configuration\n"
	              "  d[irect]               Direct configuration\n"
	              "  l[ockdown-whitelist]   Lockdown Whitelist\n"
	              "  s[tate]                general state/status commands\n"
	              "\n"),
	            prog_name);
}

/* Initialize FwCtl structure - set default values */
static void
fwc_init (FwCtl *fwc)
{
	fwc->fw = fw_client_new();
g_print("default zone: '%s'\n", fw_client_getDefaultZone(fwc->fw));
	fwc->return_value = FWC_RESULT_SUCCESS;
	fwc->return_text = g_string_new (_("Success"));
	fwc->timeout = -1;
	fwc->quiet_flag = TRUE;
}

static void
fwc_cleanup (FwCtl *fwc)
{
	g_string_free (fwc->return_text, TRUE);
}

static void
usage_zone ()
{

}

static gboolean
usage_zone_second_level (const char *cmd)
{
	gboolean ret = TRUE;

	if (matches (cmd, "add") == 0)
		g_printerr (_("Usage: firewallctl zone add { ARGUMENTS  [{--permanent | --runtime}] [--zone <zone>] | help }\n"
				"\n"
				"--permanent option can be used to set options permanently.\n"
				"These changes are not effective immediately, only after service\n"
				"restart/reload or system reboot.\n"
				"With --runtime option, a change will only be part of\n"
				"runtime configuration, i.e. overloaded with permanent configuration\n"
				"after service restart/reload.\n"
				"When none of --permanent, --runtime is used, both configuration are changed.\n"
				"\n"
				"If a timeout is supplied, the rule will be active for the specified amount\n"
				"of time and will be removed automatically afterwards.\n"
				"timeval is either a number (of seconds) or number followed by one\n"
				"of characters s (seconds), m (minutes), h (hours), for example 20m or 1h.\n"
				"\n"
				"ARGUMENTS :=  service <service> [--timeout <timeval>]\n"
				"\n"
				"Add a service in zone. If zone is omitted, default zone will be used.\n"
				"\n"
				"ARGUMENTS := port ...\n"
				"\n"
				"TODO\n"
				"ARGUMENTS := ... TODO\n"
				"\n"
				"\n"));
	else if (matches (cmd, "remove") == 0)
		g_printerr (_("TODO"));
	else if (matches (cmd, "query") == 0)
		g_printerr (_("TODO"));
	else if (matches (cmd, "list") == 0)
		g_printerr (_("TODO"));
	else if (matches (cmd, "get") == 0)
		g_printerr (_("TODO"));
	else if (matches (cmd, "set") == 0)
		g_printerr (_("TODO"));
	else if (matches (cmd, "runtime-to-permanent") == 0)
		g_printerr (_("TODO"));
	else
		ret = FALSE;
	return ret;
}

static FWCResultCode
do_zone_add (FwCtl *fwc, int argc, char **argv)
{
	return fwc->return_value;
}

static FWCResultCode
do_zone_remove (FwCtl *fwc, int argc, char **argv)
{
	return fwc->return_value;
}

static FWCResultCode
do_zone_query (FwCtl *fwc, int argc, char **argv)
{
	return fwc->return_value;
}

static FWCResultCode
do_zone_list_all_zones (FwCtl *fwc)
{
	return fwc->return_value;
}

static FWCResultCode
do_zone_list (FwCtl *fwc, int argc, char **argv)
{
	return fwc->return_value;
}

static FWCResultCode
do_zone_get (FwCtl *fwc, int argc, char **argv)
{
	return fwc->return_value;
}

static FWCResultCode
do_zone_set (FwCtl *fwc, int argc, char **argv)
{
	return fwc->return_value;
}

/* Entry point function for zone-related commands: 'firewallctl zone ...' */
FWCResultCode
do_zone (FwCtl *fwc, int argc, char **argv)
{
	/* Exit early on help */
	if (fwc_arg_is_help (*argv)) {
		usage_zone ();
		return fwc->return_value;
	}
	if (argc != 0 && fwc_arg_is_help (*(argv+1))) {
		if (usage_zone_second_level (*argv))
			return fwc->return_value;
	}

	/* Now parse the command line and perform the required operation */
	if (argc == 0) {
		fwc->return_value = do_zone_list_all_zones (fwc);
	} else {
		if (matches (*argv, "add") == 0) {
			fwc->return_value = do_zone_add (fwc, argc-1, argv+1);
		} else if (matches(*argv, "remove") == 0) {
			fwc->return_value = do_zone_remove (fwc, argc-1, argv+1);
		} else if (matches(*argv, "query") == 0) {
			fwc->return_value = do_zone_query (fwc, argc-1, argv+1);
		} else if (matches(*argv, "list") == 0) {
			fwc->return_value = do_zone_list (fwc, argc-1, argv+1);
		} else if (matches(*argv, "get") == 0) {
			fwc->return_value = do_zone_get (fwc, argc-1, argv+1);
		} else if (matches(*argv, "set") == 0) {
			fwc->return_value = do_zone_set (fwc, argc-1, argv+1);
		} else {
			usage_zone ();
			g_string_printf (fwc->return_text, _("Error: '%s' is not valid 'zone' command."), *argv);
			fwc->return_value = FWC_RESULT_ERROR_USER_INPUT;
		}
	}

	return fwc->return_value;
}

/* Entry point function for direct-related commands: 'firewallctl direct ...' */
FWCResultCode
do_direct (FwCtl *fwc, int argc, char **argv)
{
	return fwc->return_value;
}

/* Entry point function for lockdown-whitelist - related commands: 'firewallctl lockdown-whitelist ...' */
FWCResultCode
do_lockdown (FwCtl *fwc, int argc, char **argv)
{
	return fwc->return_value;
}

/* Entry point function for state-related commands: 'firewallctl state ...' */
FWCResultCode
do_state (FwCtl *fwc, int argc, char **argv)
{
	return fwc->return_value;
}


static const struct cmd {
	const char *cmd;
	FWCResultCode (*func) (FwCtl *fwc, int argc, char **argv);
} fwctl_cmds[] = {
	{ "zone",                do_zone },
	{ "direct",              do_direct },
	{ "lockdown-whitelist",  do_lockdown },
	{ "state",               do_state },
	{ 0 }
};

static FWCResultCode
do_cmd (FwCtl *fwc, const char *argv0, int argc, char **argv)
{
	const struct cmd *c;

	for (c = fwctl_cmds; c->cmd; ++c) {
		if (matches (argv0, c->cmd) == 0)
			return c->func (fwc, argc-1, argv+1);
	}

	g_string_printf (fwc->return_text, _("Error: Object '%s' is unknown, try 'firewallctl help'."), argv0);
	fwc->return_value = FWC_RESULT_ERROR_USER_INPUT;
	return fwc->return_value;
}

static int
parse_argv(FwCtl * fwc, int argc, char *argv[]) {
	assert(argc >= 0);
	assert(argv);

	char *base;

	base = strrchr (argv[0], '/');
	if (base == NULL)
		base = argv[0];
	else
		base++;

	/* parse options */
	while (argc > 1) {
		char *opt = argv[1];
		/* '--' ends options */
		if (strcmp (opt, "--") == 0) {
			argc--; argv++;
			break;
		}
		if (opt[0] != '-')
			break;
		if (opt[1] == '-')
			opt++;
		if (matches (opt, "-version") == 0) {
			g_print (_("firewallctl tool, version %s\n"), FIREWALLCTL_VERSION);
			return FWC_RESULT_SUCCESS;
		} else if (matches (opt, "-help") == 0) {
			usage (base);
			return FWC_RESULT_SUCCESS;
		} else {
			g_string_printf (fwc->return_text, _("Error: Option '%s' is unknown, try 'firewallctl -help'."), opt);
			fwc->return_value = FWC_RESULT_ERROR_USER_INPUT;
			return fwc->return_value;
		}
		argc--;
		argv++;
	}

	if (argc > 1) {
		/* Now run the requested command */
		return do_cmd (fwc, argv[1], argc-1, argv+1);
	}

	usage (base);
	return fwc->return_value;

	return 0;
}

int
main(int argc, char*argv[]) {
	fwc_init (&fw_ctl);

	parse_argv(&fw_ctl, argc, argv);

	/* Print result descripting text */
	if (fw_ctl.return_value != FWC_RESULT_SUCCESS) {
		g_printerr ("%s\n", fw_ctl.return_text->str);
	}

	fwc_cleanup (&fw_ctl);
	return fw_ctl.return_value;
}
