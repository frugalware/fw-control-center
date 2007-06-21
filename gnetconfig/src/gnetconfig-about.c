/***************************************************************************
 *  gnetconfig-about.c
 *  Author: Priyank Gosalia <priyankmg@gmail.com>	
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

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <gtk/gtk.h>
#include "gnetconfig.h"
#include "gnetconfig-about.h"

static gchar	*license = 
("This program is free software; you can redistribute it and/or "
"modify it under the terms of the GNU General Public Licence as "
"published by the Free Software Foundation; either version 2 of the "
"Licence, or (at your option) any later version.\n"
"\n"
"This program is distributed in the hope that it will be useful, "
"but WITHOUT ANY WARRANTY; without even the implied warranty of "
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU "
"General Public Licence for more details.\n"
"\n"
"You should have received a copy of the GNU General Public Licence "
"along with this program; if not, write to the Free Software "
"Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, "
"MA  02110-1301  USA");

gchar *authors[] = { "Priyank M. Gosalia <priyankmg@gmail.com>", NULL };
gchar *translators[] = { NULL };

GdkPixbuf *about_pixbuf = NULL;
extern GtkWidget *gn_main_window;

void
gnetconfig_about (void)
{
	if (!about_pixbuf)
		about_pixbuf = gtk_widget_render_icon (gn_main_window, GTK_STOCK_NETWORK, GTK_ICON_SIZE_DIALOG, NULL);
	gtk_show_about_dialog (GTK_WINDOW(gn_main_window),
                           "name", PACKAGE,
                           "version", VERSION,
                           "copyright", _("\xC2\xA9 2007 Frugalware Developer Team (GPL)"),
                           "comments", _("A graphical network configuration tool for Frugalware Linux."),
                           "license", license,
                           "authors", authors,
					  "translator-credits", translators,
                           "website", "http://www.frugalware.org/",
                           "website-label", "http://www.frugalware.org/",
                           "logo", about_pixbuf,
						   "wrap-license", TRUE,
                           NULL);
	return;
}

