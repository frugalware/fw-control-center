/***************************************************************************
 *  gnetconfig-interface.c
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
GtkWidget *gn_dhcp_hostname_entry;

/* New Widgets */
GtkWidget *gn_interface_treeview;
GtkWidget *gn_interface_dialog;

GtkWidget *gn_staticip_table;
GtkWidget *gn_dhcp_table;
GtkWidget *gn_dsl_table;

/* utility functions */
static void gnetconfig_populate_profile_list (void);
static void gnetconfig_populate_interface_list (fwnet_profile_t *profile);
static void gnetconfig_load_profile (const char *name);
static void gnetconfig_populate_dns_list (GList *list);
static void gnetconfig_setup_new_profile (const char *profile);
static int gnetconfig_save_profile (fwnet_profile_t *profile);

/* new profile dialog */
static void gnetconfig_new_profile_dialog_show (void);
static void cb_gn_new_profile_dialog_response (GtkDialog *dlg, gint arg1, gpointer dialog);

/* new nameserver dialog */
static void gnetconfig_new_nameserver_dialog_show (void);
static void cb_gn_new_nameserver_dialog_response (GtkDialog *dlg, gint arg1, gpointer dialog);

/* callbacks */
static void cb_gn_profile_changed (GtkComboBox *combo, gpointer data);
static void cb_gn_conntype_changed (GtkComboBox *combo, gpointer data);
static void cb_gn_save_interface_clicked (GtkButton *button, gpointer data);

/* new callbacks */
static void cb_gn_interface_edited (GtkButton *button, gpointer data);
static void cb_gn_delete_dns_clicked (GtkButton *button, gpointer data);

void
gnetconfig_interface_init (void)
{
	GtkWidget	*widget = NULL;
	GtkTreeModel	*model = NULL;
	GtkCellRenderer	*renderer = NULL;
	GtkListStore	*store = NULL;
	
	/* setup widgets */
	gn_main_window		= glade_xml_get_widget (xml, "window1");
	gn_profile_combo	= glade_xml_get_widget (xml, "fwn_profile_list");
//	gn_interface_combo	= glade_xml_get_widget (xml, "fwn_interface_list");
	gn_conntype_combo	= glade_xml_get_widget (xml, "fwn_conntype_list");
	gn_ipaddress_entry	= glade_xml_get_widget (xml, "fwn_ip");
	gn_netmask_entry	= glade_xml_get_widget (xml, "fwn_netmask");
	gn_gateway_entry	= glade_xml_get_widget (xml, "fwn_gateway");
	gn_hostname_entry	= glade_xml_get_widget (xml, "fwn_hostname");
	gn_dhcp_hostname_entry	= glade_xml_get_widget (xml, "fwn_dhcp_hostname");
	gn_dns_listview		= glade_xml_get_widget (xml, "fwn_dns_list");
	gn_staticip_table	= glade_xml_get_widget (xml, "fwn_staticip_table");
	gn_dhcp_table		= glade_xml_get_widget (xml, "fwn_dhcp_table");
	gn_dsl_table		= glade_xml_get_widget (xml, "fwn_dsl_table");

	/* new widgets */
	gn_interface_dialog = glade_xml_get_widget (xml, "interface_edit_dialog");
	gn_interface_treeview = glade_xml_get_widget (xml, "interface_treeview");
	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (gn_interface_treeview),
							-1,
							"Icon",
							renderer,
							"pixbuf", 0,
							NULL);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (gn_interface_treeview),
							-1,
							"Name",
							renderer,
							"text", 1,
							NULL);
	store = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_interface_treeview), GTK_TREE_MODEL(store));

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
	//model = GTK_TREE_MODEL(gtk_list_store_new (1, G_TYPE_STRING));
	//renderer = gtk_cell_renderer_text_new ();
	//gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(gn_interface_combo), renderer, FALSE);
	//gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(gn_interface_combo), renderer, "text", 0);
	//gtk_combo_box_set_model (GTK_COMBO_BOX(gn_interface_combo), model);
	//g_signal_connect (G_OBJECT(gn_interface_combo), "changed", G_CALLBACK(cb_gn_interface_changed), NULL);
	
	/* setup connection type combobox */
	g_signal_connect (G_OBJECT(gn_conntype_combo), "changed", G_CALLBACK(cb_gn_conntype_changed), NULL);

	/* setup dns listview */
	model = GTK_TREE_MODEL(gtk_list_store_new (1, G_TYPE_STRING));
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(gn_dns_listview), -1, "IP Address", renderer, "text", 0, NULL);
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_dns_listview), model);
	//g_signal_connect (G_OBJECT(gn_interface_combo), "changed", G_CALLBACK(cb_gn_interface_changed), NULL);

	/* setup new profile dialog */
	widget = glade_xml_get_widget (xml, "fwn_menu_newprofile");
	g_signal_connect (G_OBJECT(widget),
			"activate",
			G_CALLBACK(gnetconfig_new_profile_dialog_show),
			NULL);

	/* other stuff */
	widget = glade_xml_get_widget (xml, "fwn_interface_save");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_save_interface_clicked),
			(gpointer)glade_xml_get_widget(xml, "fwn_interface_label"));

	/* new interface editor stuff */
	widget = glade_xml_get_widget (xml, "fwn_if_edit");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_interface_edited),
			(gpointer)glade_xml_get_widget(xml, "fwn_interface_label"));
	widget = glade_xml_get_widget (xml, "fwn_dns_save");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(gnetconfig_new_nameserver_dialog_show),
			NULL);
	widget = glade_xml_get_widget (xml, "fwn_dns_delete");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_delete_dns_clicked),
			NULL);

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
	GdkPixbuf		*pixbuf;

	n_ifs = g_list_length (profile->interfaces);
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_interface_treeview));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);
	pixbuf = gtk_widget_render_icon (GTK_WIDGET(gn_interface_treeview),
					GTK_STOCK_NETWORK,
					GTK_ICON_SIZE_LARGE_TOOLBAR, NULL);
	for (i=0;i<n_ifs;i++)
	{
		interface = g_list_nth_data (profile->interfaces, i);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, pixbuf, 1, interface->name, -1);
	}

	//if (n_ifs!=0)
	//	gtk_combo_box_set_active (GTK_COMBO_BOX(gn_interface_treeview), 0);

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

	if (!(profile = fwnet_parseprofile ((char*)name)))
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

static int
gnetconfig_save_profile (fwnet_profile_t *profile)
{
	gchar	hostname[256];
	gchar	*buf = NULL;
	
	sprintf (hostname, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(gn_hostname_entry)));

	buf = g_strdup (profile->name);
	if(!fwnet_writeconfig (profile, hostname))
		printf ("profile saved\n");
	else
		printf ("profile not saved \n");
	
	/* the profile data gets corrupted after saving and
	 * hence needs to be reloaded */
	g_free (profile); // Replace with a better function
	active_profile = fwnet_parseprofile (buf);
	g_free (buf);

	return 0;
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
gnetconfig_new_nameserver_dialog_show (void)
{
	GtkWidget 	*dialog;
	GtkWidget 	*label;
	GtkWidget 	*entry;
	static gchar	*message = "Enter the ip address of the nameserver: ";

	dialog = gtk_dialog_new_with_buttons (_("New DNS"),
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
                             G_CALLBACK (cb_gn_new_nameserver_dialog_response),
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
	GtkTreeIter		iter;
	gint			n;

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
cb_gn_interface_edited (GtkButton *button, gpointer data)
{
	GtkTreeModel 		*model = NULL;
	GtkTreeSelection	*selection = NULL;
	GtkTreeIter		iter;
	gchar			*ifname = NULL;
	GList			*interface = NULL;
	GList			*options = NULL;
	gboolean		found = FALSE;
	fwnet_interface_t	*inte = NULL;
	char			ip[20];
	char			netmask[20];
	char			host[256];
	gchar			*markup = NULL;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_interface_treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(gn_interface_treeview));
	if ( FALSE == gtk_tree_selection_get_selected (selection, &model, &iter) )
		return;
	gtk_tree_model_get (model, &iter, 1, &ifname, -1);
	
	/* We use the following code as g_list_find() doesn't seem to work for strings */
	for (interface = active_profile->interfaces;interface != NULL;interface=g_list_next(interface))
	{
		inte = interface->data;
		if (strcmp(ifname, inte->name) == 0)
		{	
			found = TRUE;
			break;
		}
	}
	
	if (found == TRUE)
	{
		g_print ("Interface found\n");
		/* set the correct connection type */
		if ((!fwnet_is_dhcp(inte)) && (!strlen(active_profile->adsl_interface)))
		{	
			/* Static IP Active */
			g_print ("i'm static %s\n", inte->name);
			gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), 1);
			gtk_widget_set_sensitive (gn_ipaddress_entry, TRUE);
			gtk_widget_set_sensitive (gn_netmask_entry, TRUE);
			gtk_widget_set_sensitive (gn_gateway_entry, TRUE);
			gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), "");
			gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), "");
			gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), "");
			options = inte->options;
			if (!options) return; /* FIX ME */
			sscanf (options->data, "%s netmask %s", ip, netmask);
			gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), ip);
			gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), netmask);
			sscanf (inte->gateway, "%*s gw %s", ip);
			gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), ip);
		}
		else if ((fwnet_is_dhcp(inte)==1) && (!strlen(active_profile->adsl_interface)))
		{	
			/* DHCP Active */
			gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), "");
			gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), "");
			gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), "");
			gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), 0);
			gtk_widget_set_sensitive (gn_ipaddress_entry, FALSE);
			gtk_widget_set_sensitive (gn_netmask_entry, FALSE);
			gtk_widget_set_sensitive (gn_gateway_entry, FALSE);
			sscanf (inte->dhcp_opts, "%*s -h %s", host);
			g_print ("\nhost : %d", strlen(host));
			gtk_entry_set_text (GTK_ENTRY(gn_dhcp_hostname_entry), host); 
		}
		else if (strlen(active_profile->adsl_interface))
		{
			/* DSL Active */
			gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), 2);
		}
		/* set the interface name label */
		markup = g_markup_printf_escaped ("<span size=\"medium\"><b>%s</b></span>", inte->name);
		gtk_label_set_markup (GTK_LABEL(data), markup);
		g_free (markup);
		gtk_widget_show (GTK_WIDGET(gn_interface_dialog));
	}
	else
		gn_error ("No network interface found.", ERROR_GUI);
		
	return;
}

static void
cb_gn_conntype_changed (GtkComboBox *combo, gpointer data)
{
	switch (gtk_combo_box_get_active (combo))
	{
		case GN_DHCP: /* DHCP */
			gtk_widget_show (gn_dhcp_table);
			gtk_widget_hide (gn_staticip_table);
			gtk_widget_hide (gn_dsl_table);
			break;

		case GN_STATIC: /* Static ip */
			gtk_widget_show (gn_staticip_table);
			gtk_widget_hide (gn_dhcp_table);
			gtk_widget_hide (gn_dsl_table);
			gtk_widget_set_sensitive (gn_ipaddress_entry, TRUE);
			gtk_widget_set_sensitive (gn_netmask_entry, TRUE);
			gtk_widget_set_sensitive (gn_gateway_entry, TRUE);
			break;

		case GN_DSL: /* DSL */
			gtk_widget_hide (gn_staticip_table);
			gtk_widget_hide (gn_dhcp_table);
			gtk_widget_show (gn_dsl_table);
			break;

		case GN_LO: /* lo */
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

static void
cb_gn_new_nameserver_dialog_response (GtkDialog *dlg, gint arg1, gpointer dialog)
{
	if (arg1 == GTK_RESPONSE_ACCEPT)
	{
		GList *wlist = gtk_container_get_children (GTK_CONTAINER(GTK_DIALOG(dialog)->vbox));
		wlist = g_list_next (wlist);
		const gchar *ip = gtk_entry_get_text (GTK_ENTRY(wlist->data));

		/* check if the entry is blank */
		if (!strlen(ip))
		{	
			gn_error ("Enter a valid ip address", ERROR_GUI);
			g_list_free (wlist);
			return;
		}

		/* further processing */
		active_profile->dnses = g_list_append (active_profile->dnses, (gpointer)g_strdup(ip));
		gnetconfig_save_profile (active_profile);
		gnetconfig_populate_dns_list (active_profile->dnses);
		g_list_free (wlist);
	}

	gtk_widget_destroy (GTK_WIDGET(dlg));

	return;
}

static void
cb_gn_delete_dns_clicked (GtkButton *button, gpointer data)
{
	GtkTreeModel		*model = NULL;
	GtkTreeIter			iter;
	GtkTreeSelection	*selection = NULL;
	gchar				*dns = NULL;
	GList				*l = NULL;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_dns_listview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(gn_dns_listview));
	if ( FALSE == gtk_tree_selection_get_selected (selection, &model, &iter) )
		return;

	gtk_tree_model_get (model, &iter, 0, &dns, -1);
	for (l = active_profile->dnses; l!=NULL; l=l->next)
	{
		if (l->data && !strcmp((char*)l->data, dns))
		{
			active_profile->dnses = g_list_delete_link (active_profile->dnses, l);
			break;
		}
	}

	gnetconfig_save_profile (active_profile);
	gnetconfig_populate_dns_list (active_profile->dnses);
	g_free (dns);

	return;
}

static void
cb_gn_save_interface_clicked (GtkButton *button, gpointer data)
{
	gchar			*ipaddr = NULL;
	gchar			*netmask = NULL;
	gchar			*gateway = NULL;
	gchar			*if_name = NULL;
	GtkTreeModel		*model = NULL;
	GtkTreeIter		iter;
	char			opstring[50];
	GList			*intf = NULL;
	fwnet_interface_t	*interface = NULL;

	if_name = gtk_label_get_text (GTK_LABEL(data));
	for (intf = active_profile->interfaces; intf != NULL; intf = g_list_next(intf))
	{
		interface = intf->data;
		if (strcmp(if_name, interface->name) == 0)
			break;
	}

	/* set the interface name if not already set */
	/* generally this happens when a new profile is created */
	if (!strlen(interface->name))
	{
		gtk_combo_box_get_active_iter(GTK_COMBO_BOX(gn_interface_combo), &iter);
		model = gtk_combo_box_get_model(GTK_COMBO_BOX(gn_interface_combo));
		gtk_tree_model_get (model, &iter, 0, &if_name, -1);
		snprintf (interface->name, IF_NAMESIZE, if_name);
		g_print ("setting interface name %s\n", if_name);
	}

	switch (gtk_combo_box_get_active(GTK_COMBO_BOX(gn_conntype_combo)))
	{
		case GN_STATIC:
			{
				ipaddr	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_ipaddress_entry));
				netmask	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_netmask_entry));
				gateway	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_gateway_entry));

				if (interface->options == NULL)
				{	
					snprintf (opstring, 49, "%s netmask %s", ipaddr, netmask);
					interface->options = g_list_append (interface->options, strdup(opstring));
				}
				else
					interface->options->data = g_strdup_printf ("%s netmask %s",
										ipaddr,
										netmask);
				g_print (interface->options->data);
				sprintf (interface->gateway, "%s", gateway);
				printf ("saving..\n");
				break;
			}
		case GN_DHCP:
			{
				snprintf (interface->dhcp_opts, PATH_MAX, "dhcp_opts = -t 10 -h %s\n",
					(char*)gtk_entry_get_text (GTK_ENTRY(gn_dhcp_hostname_entry)));
				if (interface->options == NULL)
				{
					snprintf (opstring, 49, "dhcp");
					interface->options = g_list_append (interface->options, strdup(opstring));
				}
				else
					sprintf (interface->options->data, "dhcp");
				break;
			}
	}

	gnetconfig_save_profile (active_profile);

	return;
}

