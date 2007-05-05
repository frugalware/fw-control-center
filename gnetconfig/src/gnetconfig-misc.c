/***************************************************************************
 *  gnetconfig-misc.c
 *  Author(s): 	Priyank Gosalia <priyankmg@gmail.com>
 *  Copyright (C) 2007 Frugalware Developer Team
 ****************************************************************************/

/*
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

#include "gnetconfig-misc.h"
#include "glib.h"

#define _GNU_SOURCE

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
	fwnet_interface_t	*interface = NULL;

	/* following code is taken from netconfig */
	profile = (fwnet_profile_t*) malloc(sizeof(fwnet_profile_t));
	if (profile == NULL)
		return NULL;
	memset (profile, 0, sizeof(fwnet_profile_t));
	interface = (fwnet_interface_t*) malloc(sizeof(fwnet_interface_t));
	if (interface == NULL)
		return NULL;
	memset (interface, 0, sizeof(fwnet_interface_t));
	interface->options = NULL;
	profile->interfaces = g_list_append (profile->interfaces, interface);
	sprintf (profile->name, name);

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
		return -1;
}

