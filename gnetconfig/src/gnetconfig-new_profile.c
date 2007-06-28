/*
 *  gnetconfig-new_profile.c for gnetconfig
 *
 *  Copyright (C) 2007 by Priyank Gosalia <priyankmg@gmail.com>
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

#include "gnetconfig-new_profile.h"

extern fwnet_profile_t 	*active_profile;
extern GladeXML 	*xml;
extern GtkWidget	*gn_main_window;
extern GtkWidget	*gn_profile_combo;
extern GtkWidget	*gn_dns_listview;
extern GtkWidget	*gn_hostname_entry;

static void gnetconfig_new_profile_dialog_show (void);
static int gnetconfig_setup_new_profile (const char *profile);

void
gnetconfig_new_profile_dialog_init ()
{
	GtkWidget *widget;

	/* setup new profile dialog */
	widget = glade_xml_get_widget (xml, "fwn_menu_newprofile");
	g_signal_connect (G_OBJECT(widget),
			"activate",
			G_CALLBACK(gnetconfig_new_profile_dialog_show),
			NULL);
	return;
}

static void
gnetconfig_new_profile_dialog_show (void)
{
	gint	res;
	char	*pname = NULL;
	gchar	*filename = NULL;

	up: pname = gn_input (_("New Profile"),
				_("Enter a name for the new profile"),
				&res);
	if (res == GTK_RESPONSE_ACCEPT)
	{
		/* check if profile already exists */
		filename = g_strdup_printf ("/etc/sysconfig/network/%s", pname);
		if (g_file_test(filename, G_FILE_TEST_EXISTS))
		{	
			gn_error (_("A profile with this name already exists. Please provide a unique profile name."));
			g_free (filename);
			goto up;
			return;
		}

		/* further processing */
		if (gnetconfig_setup_new_profile (pname))
		{
			gn_error (_("Error setting up new profile"));
			g_free (filename);
			return;
		}
		g_free (pname);
		g_free (filename);
	}

	return;
}

static int
gnetconfig_setup_new_profile (const char *profile)
{
	fwnet_profile_t	*new_profile = NULL;
	GtkListStore	*profile_list = NULL;
	GtkTreeModel	*profile_model = NULL;
	GtkTreeIter	iter;
	gint		n;

	if ((new_profile = gnetconfig_new_profile (profile)) == NULL)
	{
		gn_error (_("Error creating profile."));
		return 1;
	}
	if (gn_question(_("Do you want to give a description to the new profile?")) == GTK_RESPONSE_YES)
	{
		char *ret = NULL;
		gint res;

		up: ret = gn_input (_("New Profile"), _("Enter a description for this profile:"), &res);
		if (res != GTK_RESPONSE_ACCEPT)
			goto down;
		if (ret!=NULL && strlen(ret))
		{
			snprintf (new_profile->desc, PATH_MAX, ret);
			fwnet_writeconfig (new_profile, NULL);
			g_free (ret);
		}
		else
		{
			gn_error (_("Please enter a valid description for the new profile or click the Cancel button to contiune without entering a description."));
			goto up;
		}
	}
	/* set as active profile */
	down:
	active_profile = new_profile;
	profile_model = gtk_combo_box_get_model (GTK_COMBO_BOX(gn_profile_combo));
	profile_list = GTK_LIST_STORE (profile_model);
	gtk_list_store_append (profile_list, &iter);
	gtk_list_store_set (profile_list, &iter, 1, profile, -1);
	n = gtk_tree_model_iter_n_children (profile_model, NULL);
	gtk_combo_box_set_active (GTK_COMBO_BOX(gn_profile_combo), n-1);

	/* Reset all entries */
	gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gn_dns_listview))));

	/* set default hostname */
	gtk_entry_set_text (GTK_ENTRY(gn_hostname_entry), "frugalware.example.net");

	return 0;
}
