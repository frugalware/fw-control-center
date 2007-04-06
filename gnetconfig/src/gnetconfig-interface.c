/***************************************************************************
 *  gnetconfig-interface.c
 *  Author(s): 	Priyank Gosalia <priyankmg@gmail.com>
 *  Copyright 2007 Frugalware Developer Team
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

#include "gnetconfig-interface.h"
#include "gnetconfig-messages.h"
#include "gnetconfig-misc.h"

extern GladeXML *xml;

fwnet_profile_t *active_profile;
char *active_interface = NULL;

GtkWidget *gn_main_window;
GtkWidget *gn_profile_combo;
GtkWidget *gn_interface_combo;
GtkWidget *gn_conntype_combo;
GtkWidget *gn_ipaddress_entry;
GtkWidget *gn_netmask_entry;
GtkWidget *gn_gateway_entry;
GtkWidget *gn_hostname_entry;
GtkWidget *gn_dns_listview;

GtkWidget *gn_staticip_table;
GtkWidget *gn_dhcp_table;
GtkWidget *gn_dsl_table;

/* utility functions */
static void gnetconfig_populate_profile_list (void);
static void gnetconfig_populate_interface_list (fwnet_profile_t *profile);
static void gnetconfig_load_profile (const char *name);
static void gnetconfig_populate_dns_list (GList *list);
static void gnetconfig_setup_new_profile (const char *profile);

/* new profile dialog */
static void gnetconfig_new_profile_dialog_show (void);
static void cb_gn_new_profile_dialog_response (GtkDialog *dlg, gint arg1, gpointer dialog);

/* callbacks */
static void cb_gn_profile_changed (GtkComboBox *combo, gpointer data);
static void cb_gn_interface_changed (GtkComboBox *combo, gpointer data);
static void cb_gn_conntype_changed (GtkComboBox *combo, gpointer data);

void
gnetconfig_interface_init (void)
{
	GtkWidget		*widget = NULL;
	GtkTreeModel	*model = NULL;
	GtkCellRenderer	*renderer = NULL;
	
	/* setup widgets */
	gn_main_window		= glade_xml_get_widget (xml, "window1");
	gn_profile_combo	= glade_xml_get_widget (xml, "fwn_profile_list");
	gn_interface_combo	= glade_xml_get_widget (xml, "fwn_interface_list");
	gn_conntype_combo	= glade_xml_get_widget (xml, "fwn_conntype_list");
	gn_ipaddress_entry	= glade_xml_get_widget (xml, "fwn_ip");
	gn_netmask_entry	= glade_xml_get_widget (xml, "fwn_netmask");
	gn_gateway_entry	= glade_xml_get_widget (xml, "fwn_gateway");
	gn_hostname_entry	= glade_xml_get_widget (xml, "fwn_hostname");
	gn_dns_listview		= glade_xml_get_widget (xml, "fwn_dns_list");
	gn_staticip_table	= glade_xml_get_widget (xml, "fwn_staticip_table");
	gn_dhcp_table		= glade_xml_get_widget (xml, "fwn_dhcp_table");
	gn_dsl_table		= glade_xml_get_widget (xml, "fwn_dsl_table");

	/* setup profiles combobox */
	model = GTK_TREE_MODEL(gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING));
	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(gn_profile_combo), renderer, FALSE);
	gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(gn_profile_combo), renderer, "pixbuf", 0);
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(gn_profile_combo), renderer, FALSE);
	gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(gn_profile_combo), renderer, "text", 1);
	gtk_combo_box_set_model (GTK_COMBO_BOX(gn_profile_combo), model);
	g_signal_connect (G_OBJECT(gn_profile_combo), "changed", G_CALLBACK(cb_gn_profile_changed), NULL);

	/* setup interfaces combobox */
	model = GTK_TREE_MODEL(gtk_list_store_new (1, G_TYPE_STRING));
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(gn_interface_combo), renderer, FALSE);
	gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(gn_interface_combo), renderer, "text", 0);
	gtk_combo_box_set_model (GTK_COMBO_BOX(gn_interface_combo), model);
	g_signal_connect (G_OBJECT(gn_interface_combo), "changed", G_CALLBACK(cb_gn_interface_changed), NULL);
	
	/* setup connection type combobox */
	//model = GTK_TREE_MODEL(gtk_list_store_new (1, G_TYPE_STRING));
	//renderer = gtk_cell_renderer_text_new ();
	//gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(gn_conntype_combo), renderer, FALSE);
	//gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(gn_conntype_combo), renderer, "text", 0);
	//gtk_combo_box_set_model (GTK_COMBO_BOX(gn_conntype_combo), model);
	g_signal_connect (G_OBJECT(gn_conntype_combo), "changed", G_CALLBACK(cb_gn_conntype_changed), NULL);

	/* setup dns listview */
	model = GTK_TREE_MODEL(gtk_list_store_new (1, G_TYPE_STRING));
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(gn_dns_listview), -1, "IP Address", renderer, "text", 0, NULL);
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_dns_listview), model);
	//g_signal_connect (G_OBJECT(gn_interface_combo), "changed", G_CALLBACK(cb_gn_interface_changed), NULL);

	/* setup new profile dialog */
	widget = glade_xml_get_widget (xml, "fwn_menu_newprofile");
	g_signal_connect (G_OBJECT(widget), "activate", G_CALLBACK(gnetconfig_new_profile_dialog_show), NULL);

	/* Load main stuff */
	gnetconfig_populate_profile_list ();
	gtk_widget_show (gn_main_window);

	return;
}

static void
gnetconfig_populate_profile_list (void)
{
	GDir			*dir = NULL;
	GError			*error = NULL;
	const gchar		*file;
	GtkTreeModel		*gn_profile_model = NULL;
	GtkListStore		*gn_profile_store = NULL;
	GtkTreeIter		iter;
	GdkPixbuf		*pixbuf = NULL;
	char			*fn = NULL;
	gint			index = -1;

	if (!(dir = g_dir_open ("/etc/sysconfig/network", 0, &error)))
	{
		g_error ("Error parsing profile directory.");
		return;
	}
	file = g_dir_read_name (dir);
	gn_profile_model = gtk_combo_box_get_model (GTK_COMBO_BOX(gn_profile_combo));
	gn_profile_store = GTK_LIST_STORE (gn_profile_model);
	gtk_list_store_clear (gn_profile_store);
	pixbuf = gtk_widget_render_icon (gn_profile_combo, GTK_STOCK_APPLY, GTK_ICON_SIZE_MENU, NULL);

	while (file != NULL)
	{
		gtk_list_store_append (gn_profile_store, &iter);
		gtk_list_store_set (gn_profile_store, &iter, 1, file, -1);
		if ((fn=fwnet_lastprofile()))
		{
			if (!strcmp(file,fn))
			{	
				index++;
				gtk_list_store_set (gn_profile_store, &iter, 0, pixbuf, -1);
			}
		}
		file = g_dir_read_name (dir);
	}
	gtk_combo_box_set_active (GTK_COMBO_BOX(gn_profile_combo), index);
	g_dir_close (dir);

	return;
}

static void
gnetconfig_populate_interface_list (fwnet_profile_t *profile)
{
	gint			i, n_ifs = 0;
	fwnet_interface_t	*interface;
	GtkTreeModel		*model = NULL;
	GtkListStore		*store = NULL;
	GtkTreeIter		iter;

	n_ifs = g_list_length (profile->interfaces);
	model = gtk_combo_box_get_model (GTK_COMBO_BOX(gn_interface_combo));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);
	for (i=0;i<n_ifs;i++)
	{
		interface = g_list_nth_data (profile->interfaces, i);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, interface->name, -1);
	}

	if (n_ifs!=0)
		gtk_combo_box_set_active (GTK_COMBO_BOX(gn_interface_combo), 0);

	return;
}

static void
gnetconfig_populate_dns_list (GList *list)
{
	GtkTreeModel	*model = NULL;
	GtkListStore	*store = NULL;
	GtkTreeIter	iter;

	if (!list)
		return;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_dns_listview));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);

	while (list != NULL)
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, list->data, -1);
		list = g_list_next (list);
	}

	gtk_widget_show (gn_dns_listview);

	return;
}

static void
gnetconfig_load_profile (const char *name)
{
	fwnet_profile_t		*profile;
	char			hostname[256];

	if (!(profile = fwnet_parseprofile (name)))
		return;

	/* set the active profile */
	active_profile = profile;

	/* populate the list of interfaces */
	gnetconfig_populate_interface_list (profile);

	/* populate the dns list */
	gnetconfig_populate_dns_list (profile->dnses);

	/* read the hostname */
	gnetconfig_read_hostname(hostname);
	if (hostname)
		gtk_entry_set_text (GTK_ENTRY(gn_hostname_entry), hostname);
	else
		gn_error ("Couldn't read hostname.", ERROR_GUI);

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
                                         NULL,
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

static void
gnetconfig_setup_new_profile (const char *profile)
{
	fwnet_profile_t		*new_profile = NULL;
	GtkListStore		*profile_list = NULL;
	GtkTreeModel		*profile_model = NULL;
	GtkTreeIter			iter;
	gint				n;

	if ((new_profile = gnetconfig_new_profile (profile)) == NULL)
	{
		gn_error ("Error creating profile.", ERROR_GUI);
		return;
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
	gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), "");
	gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), 1);
	gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gn_dns_listview))));

	return;
}

/* CALLBACKS */

static void
cb_gn_profile_changed (GtkComboBox *combo, gpointer data)
{
	GtkTreeIter	iter;
	GtkTreeModel	*model = NULL;
	gchar		*text = NULL;
	
	if (gtk_combo_box_get_active_iter(combo, &iter))
	{
		model = gtk_combo_box_get_model(combo);
		gtk_tree_model_get (model, &iter, 1, &text, -1);
		gnetconfig_load_profile (text);
	}

	return;
}

static void
cb_gn_interface_changed (GtkComboBox *combo, gpointer data)
{
	GList			*interface = NULL;
	GList			*options = NULL;
	fwnet_interface_t	*inte;
	GtkTreeIter		iter;
	GtkTreeModel		*model = NULL;
	gchar			*text = NULL;
	char			ip[20];
	char			netmask[20];
	gint			if_pos = -1;

	if (gtk_combo_box_get_active_iter(combo, &iter))
	{
		model = gtk_combo_box_get_model(combo);
		gtk_tree_model_get (model, &iter, 0, &text, -1);
		if_pos = gtk_combo_box_get_active (combo);
		interface = g_list_nth (active_profile->interfaces, if_pos);
		if (interface != NULL)
		{
			inte = interface->data;

			/* set the correct connection type */
			if ((!fwnet_is_dhcp(inte)) && (!strlen(active_profile->adsl_interface)))
			{	
				/* Static IP Active */
				gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), 1);
				gtk_widget_set_sensitive (gn_ipaddress_entry, TRUE);
				gtk_widget_set_sensitive (gn_netmask_entry, TRUE);
				gtk_widget_set_sensitive (gn_gateway_entry, TRUE);
				options = inte->options;
				sscanf (options->data, "%s netmask %s", ip, netmask);
				gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), ip);
				gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), netmask);
				sscanf (inte->gateway, "%*s gw %s", ip);
				gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), ip);
			}
			else if ((fwnet_is_dhcp(inte)) && (!strlen(active_profile->adsl_interface)))
			{	
				/* DHCP Active */
				gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), 0);
				gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), "");
				gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), "");
				gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), "");
				gtk_widget_set_sensitive (gn_ipaddress_entry, FALSE);
				gtk_widget_set_sensitive (gn_netmask_entry, FALSE);
				gtk_widget_set_sensitive (gn_gateway_entry, FALSE);
			}
			else if (strlen(active_profile->adsl_interface))
			{
				/* DSL Active */
				gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), 2);
			}
		}
		else
			gn_error ("No network interface found.", ERROR_GUI);
	}

	return;
}

static void
cb_gn_conntype_changed (GtkComboBox *combo, gpointer data)
{
	gint sel;

	sel = gtk_combo_box_get_active (combo);
	switch (sel)
	{
		case 0: /* DHCP */
			gtk_widget_show (gn_dhcp_table);
			gtk_widget_hide (gn_staticip_table);
			gtk_widget_hide (gn_dsl_table);
			break;

		case 1: /* Static ip */
			gtk_widget_show (gn_staticip_table);
			gtk_widget_hide (gn_dhcp_table);
			gtk_widget_hide (gn_dsl_table);
			break;

		case 2: /* DSL */
			gtk_widget_hide (gn_staticip_table);
			gtk_widget_hide (gn_dhcp_table);
			gtk_widget_show (gn_dsl_table);
			break;

		case 3: /* lo */
			break;
	}

	return;
}

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
			gn_error ("profile already exists", ERROR_GUI);

		// further processing
		gnetconfig_setup_new_profile (pname);

		g_free (filename);
		g_list_free (wlist);
	}

	gtk_widget_destroy (GTK_WIDGET(dlg));
	return;
}

