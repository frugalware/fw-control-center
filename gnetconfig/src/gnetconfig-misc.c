/*
 *  gnetconfig-misc.c for gnetconfig
 *
 *  Copyright (C) 2007 by Priyank Gosalia <priyankmg@gmail.com>
 *  Parts borrowed from net-tools.
 *  The parts are Copyright (C) 1998, 2000 by Andi Kleen
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ctype.h>

#include "gnetconfig-misc.h"
#include "glib.h"

void
gnetconfig_read_hostname (char *hostname)
{
	FILE *fp = NULL;

	if (!(fp = fopen ("/etc/HOSTNAME", "r")))
	{
		hostname = NULL;
		return;
	}
	fscanf (fp, "%s", hostname);
	fclose (fp);

	return;
}

int
gnetconfig_set_hostname (const char *hostname)
{
	FILE *fp = NULL;

	if (hostname == NULL || !(strlen(hostname)))
		return -1;

	if (!(fp=fopen ("/etc/HOSTNAME", "w")))
			return -1;

	fprintf (fp, "%s", hostname);
	fclose (fp);

	return 0;
}

fwnet_profile_t *
gnetconfig_new_profile (const char *name)
{
	fwnet_profile_t		*profile = NULL;

	/* following code is borrowed from netconfig */
	profile = (fwnet_profile_t*) malloc(sizeof(fwnet_profile_t));
	if (profile)
	{
		memset (profile, 0, sizeof(fwnet_profile_t));
		sprintf (profile->name, name);
		char *path = g_strdup_printf ("/etc/sysconfig/network/%s", name);
		FILE *fp = fopen (path, "w");
		fclose (fp);
	}

	return profile;
}

void
gnetconfig_profile_free (fwnet_profile_t *profile)
{
	int i, l = 0;

	if (profile == NULL)
		return;

	l = g_list_length (profile->interfaces);
	for (i=0;i<l;i++)
	{
		fwnet_interface_t *iface = (fwnet_interface_t *)g_list_nth_data (profile->interfaces, i);
		if (iface != NULL)
		{
			g_list_free (iface->options);
			g_list_free (iface->pre_ups);
			g_list_free (iface->pre_downs);
			g_list_free (iface->post_ups);
			g_list_free (iface->post_downs);
		}
		g_free (iface);
	}
	g_list_free (profile->interfaces);
	g_list_free (profile->dnses);
	g_free (profile);

	return;
}

int
gnetconfig_get_wireless_mode (const char *mode)
{
	if (strlen(mode) && (!strcmp(mode, "ad-hoc")))
		return GN_AD_HOC;
	else if (strlen(mode) && (!strcmp(mode, "managed")))
		return GN_MANAGED;
	else if (strlen(mode) && (!strcmp(mode, "master")))
		return GN_MASTER;
	else if (strlen(mode) && (!strcmp(mode, "monitor")))
		return GN_MONITOR;
	else if (strlen(mode) && (!strcmp(mode, "repeater")))
		return GN_REPEATER;
	else if (strlen(mode) && (!strcmp(mode, "secondary")))
		return GN_SECONDARY;
	else if (strlen(mode) && (!strcmp(mode, "auto")))
		return GN_AUTO;
	else
		return GN_AUTO;
}

char *
gnetconfig_get_wireless_mode_string (int mode)
{
	char *ret = NULL;

	switch (mode)
	{
		case GN_AD_HOC:		ret = g_strdup ("ad-hoc");
					break;
		case GN_MANAGED:	ret = g_strdup ("managed");
					break;
		case GN_MASTER:		ret = g_strdup ("master");
					break;
		case GN_MONITOR:	ret = g_strdup ("monitor");
					break;
		case GN_REPEATER:	ret = g_strdup ("repeater");
					break;
		case GN_SECONDARY:	ret = g_strdup ("secondary");
					break;
		case GN_AUTO:		ret = g_strdup ("auto");
					break;
		default:		break;
	}

	return ret;
}

char *
gnetconfig_get_ifname (char *name, char *p)
{
    while (isspace(*p))
	p++;
    while (*p) {
	if (isspace(*p))
	    break;
	if (*p == ':') {	/* could be an alias */
	    char *dot = p, *dotname = name;
	    *name++ = *p++;
	    while (isdigit(*p))
		*name++ = *p++;
	    if (*p != ':') {	/* it wasn't, backup */
		p = dot;
		name = dotname;
	    }
	    if (*p == '\0')
		return NULL;
	    p++;
	    break;
	}
	*name++ = *p++;
    }
    *name++ = '\0';
    return p;
}
