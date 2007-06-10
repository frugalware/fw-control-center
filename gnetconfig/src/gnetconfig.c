/***************************************************************************
 *  gnetconfig.c
 *  Author(s): Priyank Gosalia <priyankmg@gmail.com>
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

#define _GNU_SOURCE
#include <locale.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "gnetconfig.h"
#include "gnetconfig-messages.h"
#include "gnetconfig-interface.h"

#define GLADE_FILE	"/share/gnetconfig/gnetconfig.glade"

GladeXML *xml = NULL;

int
main (int argc, char *argv[])
{
	gchar *path;

	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	textdomain (GETTEXT_PACKAGE);

	gtk_init (&argc, &argv);

	if ( geteuid() != 0 )
	{
		gn_error (_("Gnetconfig should be run as root."), ERROR_GUI);
		return 1;
	}

	path = g_strdup_printf ("%s%s", PREFIX, GLADE_FILE);
	xml = glade_xml_new (path, NULL, NULL);
	g_free (path);

	if (!xml)
	{
		gn_error (_("Failed to initialize interface."), ERROR_CONSOLE);
		return 1;
	}

	glade_xml_signal_autoconnect (xml);
	gnetconfig_interface_init ();

	gtk_main ();

	return 0;
}

