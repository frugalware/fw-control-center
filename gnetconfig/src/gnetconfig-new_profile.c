/***************************************************************************
 *  gnetconfig-new_profile.c
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

#include "gnetconfig-new_profile.h"

extern fwnet_profile_t 	*active_profile;
extern GladeXML 	*xml;
extern GtkWidget	*gn_main_window;
extern GtkWidget	*gn_profile_combo;
extern GtkWidget	*gn_dns_listview;

static void gnetconfig_new_profile_dialog_show (void);
static void cb_gn_new_profile_dialog_response (GtkDialog *dlg, gint arg1, gpointer dialog);

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
	GtkWidget 	*dialog;
	GtkWidget 	*label;
	GtkWidget 	*entry;
	static gchar	*message = "Enter a name for the new profile: ";

	dialog = gtk_dialog_new_with_buttons (_("New Profile"),
                                         gn_main_window,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_REJECT,
                                         NULL);
	gtk_window_set_resizable (GTK_WINDOW(dialog), FALSE);
	label = gtk_label_new (message);
	entry = gtk_entry_new ();

	g_signal_connect_swapped (dialog,
                             "response",
                             G_CALLBACK (cb_gn_new_profile_dialog_response),
                             dialog);
	gtk_misc_set_padding (GTK_MISC(label), 5, 5);
	gtk_dialog_set_has_separator (GTK_DIALOG(dialog), FALSE);
	gtk_container_set_border_width (GTK_CONTAINER((GTK_DIALOG(dialog))->vbox), 10);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), entry);

	gtk_widget_show_all (dialog);

	return;
}

int
gnetconfig_setup_new_profile (const char *profile)
{
	FILE		*fp = NULL;
	fwnet_profile_t	*new_profile = NULL;
	GtkListStore	*profile_list = NULL;
	GtkTreeModel	*profile_model = NULL;
	GtkTreeIter	iter;
	gint		n;

	if ((new_profile = gnetconfig_new_profile (profile)) == NULL)
	{
		gn_error ("Error creating profile.", ERROR_GUI);
		return 1;
	}

	/* set as active profile */
	active_profile = new_profile;
	profile_model = gtk_combo_box_get_model (GTK_COMBO_BOX(gn_profile_combo));
	profile_list = GTK_LIST_STORE (profile_model);
	gtk_list_store_append (profile_list, &iter);
	gtk_list_store_set (profile_list, &iter, 1, profile, -1);
	n = gtk_tree_model_iter_n_children (profile_model, NULL);
	gtk_combo_box_set_active (GTK_COMBO_BOX(gn_profile_combo), n-1);

	/* Reset all entries */
	gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gn_dns_listview))));

	return;
}

/* CALLBACKS */

static void
cb_gn_new_profile_dialog_response (GtkDialog *dlg, gint arg1, gpointer dialog)
{
	if (arg1 == GTK_RESPONSE_ACCEPT)
	{
		GList		*wlist = NULL;
		gchar		*filename = NULL;
		const gchar	*pname;

		wlist = gtk_container_get_children (GTK_CONTAINER(GTK_DIALOG(dialog)->vbox));
		wlist = g_list_next (wlist);
		pname = gtk_entry_get_text (GTK_ENTRY(wlist->data));

		/* check if profile already exists */
		filename = g_strdup_printf ("/etc/sysconfig/network/%s", pname);
		if (g_file_test(filename, G_FILE_TEST_EXISTS))
		{	
			gn_error ("profile already exists", ERROR_GUI);
			return;
		}

		// further processing
		gnetconfig_setup_new_profile (pname);

		g_free (filename);
		g_list_free (wlist);
	}

	gtk_widget_destroy (GTK_WIDGET(dlg));
	return;
}
