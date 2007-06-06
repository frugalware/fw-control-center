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
#include <gdk/gdkkeysyms.h>

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "gnetconfig-interface.h"
#include "gnetconfig-about.h"
#include "gnetconfig-messages.h"
#include "gnetconfig-misc.h"
#include "gnetconfig-new_interface.h"

#include <libfwutil.h>

extern GladeXML *xml;

fwnet_profile_t *active_profile;

GtkWidget *gn_main_window;
GtkWidget *gn_profile_combo;
GtkWidget *gn_interface_combo;
GtkWidget *gn_conntype_combo;
GtkWidget *gn_ipaddress_entry;
GtkWidget *gn_netmask_entry;
GtkWidget *gn_dsl_username_entry;
GtkWidget *gn_dsl_password_entry;
GtkWidget *gn_dsl_cpassword_entry;
GtkWidget *gn_dsl_interface;
GtkWidget *gn_gateway_entry;
GtkWidget *gn_essid_entry;
GtkWidget *gn_key_entry;
GtkWidget *gn_hostname_entry;
GtkWidget *gn_dns_listview;
GtkWidget *gn_dhcp_hostname_entry;
GtkWidget *gn_wireless_mode_combo;

/* New Widgets */
GtkWidget *gn_interface_treeview;
GtkWidget *gn_interface_dialog;
GtkWidget *gn_interface_textview;

GtkWidget *gn_staticip_table;
GtkWidget *gn_dhcp_table;
GtkWidget *gn_dsl_table;
GtkWidget *gn_wireless_table;

/* utility functions */
static void gnetconfig_populate_profile_list (void);
static void gnetconfig_load_profile (const char *name);
static void gnetconfig_populate_dns_list (GList *list);
static void gnetconfig_setup_new_profile (const char *profile);

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
static void cb_gn_hostname_save (GtkButton *button, gpointer data);
static void cb_gn_interface_start (GtkButton *button, gpointer data);
static void cb_gn_interface_stop (GtkButton *button, gpointer data);
static void cb_gn_interface_add (GtkButton *button, gpointer data);
static void cb_gn_interface_edited (GtkButton *button, gpointer data);
static void cb_gn_interface_selected (GtkTreeSelection *selection, gpointer data);
static void cb_gn_interface_delete (GtkButton *button, gpointer data);
static void cb_gn_delete_dns_clicked (GtkButton *button, gpointer data);
static void cb_gn_dns_listview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data);
static void cb_gn_interface_double_click (GtkTreeView *treeview);
static void cb_gn_interface_right_click (GtkTreeView *treeview, GdkEventButton *event);

void
gnetconfig_interface_init (void)
{
	GtkWidget		*widget = NULL;
	GtkTreeModel		*model = NULL;
	GtkCellRenderer		*renderer = NULL;
	GtkListStore		*store = NULL;
	GtkTreeViewColumn 	*column = NULL;

	/* setup widgets */
	gn_main_window		= glade_xml_get_widget (xml, "window1");
	gn_profile_combo	= glade_xml_get_widget (xml, "fwn_profile_list");
	gn_conntype_combo	= glade_xml_get_widget (xml, "fwn_conntype_list");
	gn_ipaddress_entry	= glade_xml_get_widget (xml, "fwn_ip");
	gn_netmask_entry	= glade_xml_get_widget (xml, "fwn_netmask");
	gn_gateway_entry	= glade_xml_get_widget (xml, "fwn_gateway");
	gn_essid_entry		= glade_xml_get_widget (xml, "fwn_essid_entry");
	gn_key_entry		= glade_xml_get_widget (xml, "fwn_key_entry");
	gn_hostname_entry	= glade_xml_get_widget (xml, "fwn_hostname");
	gn_dns_listview		= glade_xml_get_widget (xml, "fwn_dns_list");
	gn_staticip_table	= glade_xml_get_widget (xml, "fwn_staticip_table");
	gn_dhcp_table		= glade_xml_get_widget (xml, "fwn_dhcp_table");
	gn_dsl_table		= glade_xml_get_widget (xml, "fwn_dsl_table");
	gn_wireless_table	= glade_xml_get_widget (xml, "fwn_wireless_table");
	gn_dhcp_hostname_entry	= glade_xml_get_widget (xml, "fwn_dhcp_hostname");
	gn_interface_textview 	= glade_xml_get_widget (xml, "fwn_interface_textview");
	gn_wireless_mode_combo	= glade_xml_get_widget (xml, "fwn_wmode_combo");
	gn_dsl_username_entry	= glade_xml_get_widget (xml, "fwn_dsl_username");
	gn_dsl_password_entry	= glade_xml_get_widget (xml, "fwn_dsl_password");
	gn_dsl_cpassword_entry	= glade_xml_get_widget (xml, "fwn_dsl_cpassword");

	/* new widgets */
	gn_interface_dialog = glade_xml_get_widget (xml, "interface_edit_dialog");
	gn_interface_treeview = glade_xml_get_widget (xml, "interface_treeview");

	renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes (_("IF_Icon"),
							renderer,
							"pixbuf", 0,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_interface_treeview), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("IF_Name"),
							renderer,
							"text", 1,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_column_set_min_width (column, 120);
	g_object_set (G_OBJECT(column), "expand", TRUE, "spacing", 4, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_interface_treeview), column);

	renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes (_("IF_StatusIcon"),
							renderer,
							"pixbuf", 2,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_interface_treeview), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("IF_StatusText"),
							renderer,
							"text", 3,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_interface_treeview), column);

	store = gtk_list_store_new (4, GDK_TYPE_PIXBUF, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_interface_treeview), GTK_TREE_MODEL(store));
	g_signal_connect (gn_interface_treeview,
			"row-activated",
			G_CALLBACK(cb_gn_interface_double_click),
			NULL);
	g_signal_connect (gn_interface_treeview,
			"button-release-event",
			G_CALLBACK(cb_gn_interface_right_click),
			NULL);
	g_signal_connect (G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(gn_interface_treeview))),
			"changed",
			G_CALLBACK(cb_gn_interface_selected),
			NULL);

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

	/* setup connection type combobox */
	g_signal_connect (G_OBJECT(gn_conntype_combo), "changed", G_CALLBACK(cb_gn_conntype_changed), NULL);

	/* setup dns listview */
	model = GTK_TREE_MODEL(gtk_list_store_new (1, G_TYPE_STRING));
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(gn_dns_listview), -1, "IP Address", renderer, "text", 0, NULL);
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_dns_listview), model);

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
	widget = glade_xml_get_widget (xml, "fwn_if_add");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_interface_add),
			NULL);
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
	widget = glade_xml_get_widget (xml, "fwn_if_start");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_interface_start),
			NULL);
	widget = glade_xml_get_widget (xml, "fwn_if_stop");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_interface_stop),
			NULL);
	widget = glade_xml_get_widget (xml, "fwn_if_delete");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_interface_delete),
			NULL);
	widget = glade_xml_get_widget (xml, "fwn_hostname_save");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_hostname_save),
			NULL);
	widget = glade_xml_get_widget (xml, "fwn_about");
	g_signal_connect (G_OBJECT(widget),
			"activate",
			G_CALLBACK(gnetconfig_about),
			NULL);

	/* keybindings */
	g_signal_connect (gn_dns_listview,
			"key_release_event",
			G_CALLBACK (cb_gn_dns_listview_keypress),
			NULL);

	/* the delete-event */
	g_signal_connect (gn_main_window,
			"delete-event",
			G_CALLBACK (gtk_main_quit),
			NULL);

	/* Load main stuff */
	gnetconfig_populate_profile_list ();
	gnetconfig_new_interface_dialog_setup ();
	gtk_widget_show (gn_main_window);

	/* unref xml object */
	g_object_unref (xml);

	return;
}

static void
gnetconfig_populate_profile_list (void)
{
	GDir		*dir = NULL;
	GError		*error = NULL;
	const gchar	*file;
	GtkTreeModel	*gn_profile_model = NULL;
	GtkListStore	*gn_profile_store = NULL;
	GtkTreeIter	iter;
	GdkPixbuf	*pixbuf = NULL;
	char		*fn = NULL;
	gint		index = -1;

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

void
gnetconfig_populate_interface_list (fwnet_profile_t *profile)
{
	gint			i, n_ifs = 0;
	fwnet_interface_t	*interface;
	GtkTreeModel		*model = NULL;
	GtkListStore		*store = NULL;
	GtkTreeIter		iter;
	GdkPixbuf		*pixbuf;
	GdkPixbuf		*yes_pixbuf;
	GdkPixbuf		*no_pixbuf;
	gchar			*ptr = NULL;
	gboolean		flag = FALSE;

	n_ifs = g_list_length (profile->interfaces);
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_interface_treeview));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);
	pixbuf = gtk_widget_render_icon (GTK_WIDGET(gn_interface_treeview),
					GTK_STOCK_NETWORK,
					GTK_ICON_SIZE_LARGE_TOOLBAR, NULL);
	yes_pixbuf = gtk_widget_render_icon (GTK_WIDGET(gn_interface_treeview),
					GTK_STOCK_YES,
					GTK_ICON_SIZE_MENU, NULL);
	no_pixbuf = gtk_widget_render_icon (GTK_WIDGET(gn_interface_treeview),
					GTK_STOCK_NO,
					GTK_ICON_SIZE_MENU, NULL);

	if ((strcmp(profile->name, fwnet_lastprofile())) == 0)
		flag = TRUE;

	for (i=0;i<n_ifs;i++)
	{
		interface = g_list_nth_data (profile->interfaces, i);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, pixbuf, 1, interface->name, -1);
		ptr = g_strdup_printf ("ifconfig %s | grep UP > /dev/null", interface->name);
		if (flag == TRUE)
		{
			if (!fwutil_system(ptr))
				gtk_list_store_set (store, &iter, 2, yes_pixbuf, 3, " UP", -1);
			else
				gtk_list_store_set (store, &iter, 2, no_pixbuf, 3, " DOWN", -1);
		}
		else
		{
			gtk_list_store_set (store, &iter, 2, no_pixbuf, 3, " DOWN", -1);
		}
		g_free (ptr);
	}
	g_object_unref (pixbuf);
	g_object_unref (yes_pixbuf);
	g_object_unref (no_pixbuf);

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
	fwnet_profile_t	*profile;
	char		hostname[256];

	/* free old profile */
	if (active_profile != NULL)
		gnetconfig_profile_free (active_profile);

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

int
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
	gnetconfig_profile_free (profile);
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
	fwnet_profile_t	*new_profile = NULL;
	GtkListStore	*profile_list = NULL;
	GtkTreeModel	*profile_model = NULL;
	GtkTreeIter	iter;
	gint		n;

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

		/* clear interface textview */
		GtkTextBuffer	*buffer = NULL;
		GtkTextIter	t_iter;
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(gn_interface_textview));
		if (buffer)
		{
			gtk_text_buffer_set_text (buffer, "", 0);
			gtk_text_buffer_get_iter_at_offset (buffer, &t_iter, 0);
		}
	}

	return;
}

static void
cb_gn_interface_add (GtkButton *button, gpointer data)
{
	gnetconfig_new_interface_dialog_show ();
	return;
}

static void
cb_gn_interface_start (GtkButton *button, gpointer data)
{
	gchar			*ptr = NULL;
	gchar			*ifname = NULL;
	GList			*interface = NULL;
	GtkTreeModel		*model = NULL;
	GtkTreeSelection	*selection = NULL;
	GtkTreeIter		iter;
	gboolean		found = FALSE;
	fwnet_interface_t*	inte = NULL;
	gint			ret;

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
	if (!found)
		gn_error ("Unknown error.", ERROR_GUI);

	ptr = g_strdup_printf ("ifconfig %s | grep UP > /dev/null", inte->name);
	if (!fwutil_system(ptr))
		gn_error ("Interface is already started.", ERROR_GUI);
	else
	{
		ret = fwnet_ifup (inte, active_profile);
		gnetconfig_populate_interface_list (active_profile);
	}
	g_free (ptr);

	return;
}

static void
cb_gn_interface_stop (GtkButton *button, gpointer data)
{
	gchar			*ptr = NULL;
	gchar			*ifname = NULL;
	GList			*interface = NULL;
	GtkTreeModel		*model = NULL;
	GtkTreeSelection	*selection = NULL;
	GtkTreeIter		iter;
	gboolean		found = FALSE;
	fwnet_interface_t*	inte = NULL;
	gint			ret;

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
	if (!found)
		gn_error ("Unknown error.", ERROR_GUI);

	ptr = g_strdup_printf ("ifconfig %s | grep UP > /dev/null", inte->name);
	if (fwutil_system(ptr))
		gn_error ("Interface is not running.", ERROR_GUI);
	else
	{
		ret = fwnet_ifdown (inte, active_profile);
		gnetconfig_populate_interface_list (active_profile);
	}
	g_free (ptr);

	return;
}

static void
cb_gn_interface_edited (GtkButton *button, gpointer data)
{
	GtkTreeModel		*model = NULL;
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
	gint			dsl_conn = -1;

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
		gtk_widget_hide (gn_wireless_table);
		gtk_widget_hide (gn_dsl_table);
		/* set the correct connection type */
		if ((!fwnet_is_dhcp(inte)))
		{
			/* Static IP Active */
			gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), GN_STATIC);
			gtk_widget_set_sensitive (gn_ipaddress_entry, TRUE);
			gtk_widget_set_sensitive (gn_netmask_entry, TRUE);
			gtk_widget_set_sensitive (gn_gateway_entry, TRUE);
			gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), "");
			gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), "");
			gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), "");
			options = inte->options;
			if (!options) return; /* FIX ME */
			if (sscanf (options->data, "%s netmask %s", ip, netmask))
			{
				if (strlen(ip))
					gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), ip);
				if (strlen(netmask))
					gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), netmask);
			}
			sscanf (inte->gateway, "%*s gw %s", ip);
			if (strlen(ip))
				gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), ip);
			dsl_conn = GN_STATIC;
		}
		else if ((fwnet_is_dhcp(inte)==1) && (!fwnet_is_wireless_device(inte->name)))
		{
			/* DHCP Active */
			gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), "");
			gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), "");
			gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), "");
			gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), GN_DHCP);
			gtk_widget_set_sensitive (gn_ipaddress_entry, FALSE);
			gtk_widget_set_sensitive (gn_netmask_entry, FALSE);
			gtk_widget_set_sensitive (gn_gateway_entry, FALSE);
			if (sscanf (inte->dhcp_opts, "%*s %*s -h %s", host))
				gtk_entry_set_text (GTK_ENTRY(gn_dhcp_hostname_entry), host);
			dsl_conn = GN_DHCP;
		}
		if (strlen(active_profile->adsl_interface))
		{
			/* DSL Active */
			gtk_widget_show (gn_dsl_table);
			switch (dsl_conn)
			{
				case GN_STATIC:
					gtk_widget_show (gn_staticip_table);
					gtk_widget_hide (gn_dhcp_table);
					break;
				case GN_DHCP:
					gtk_widget_show (gn_dhcp_table);
					gtk_widget_hide (gn_staticip_table);
					break;
				default:
					gtk_widget_hide (gn_dhcp_table);
					gtk_widget_hide (gn_staticip_table);
					break;
			}
			gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), GN_DSL);
			gtk_entry_set_text (GTK_ENTRY(gn_dsl_username_entry), (active_profile->adsl_username!=NULL)?active_profile->adsl_username : "");
			gtk_entry_set_text (GTK_ENTRY(gn_dsl_password_entry), (active_profile->adsl_password!=NULL)?active_profile->adsl_password : "");
			gtk_entry_set_text (GTK_ENTRY(gn_dsl_cpassword_entry), (active_profile->adsl_password!=NULL)?active_profile->adsl_password : "");
		}
		if (fwnet_is_wireless_device(inte->name) && (strlen(inte->essid)))
		{
			/* Wireless Active */
			gtk_widget_show (gn_wireless_table);
			gtk_combo_box_set_active (GTK_COMBO_BOX(gn_wireless_mode_combo), gnetconfig_get_wireless_mode (inte->mode));
			if (strlen(inte->essid))
				gtk_entry_set_text (GTK_ENTRY(gn_essid_entry), inte->essid);
			if (strlen(inte->key))
				gtk_entry_set_text (GTK_ENTRY(gn_key_entry), inte->key);
			options = inte->options;
			if (!options) return;
			if (sscanf (options->data, "%s netmask %s", ip, netmask) == 2)
			{
				if (strlen(ip))
					gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), ip);
				if (strlen(netmask))
					gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), netmask);
			}
			*ip = '\0';
			if (sscanf (inte->gateway, "%*s gw %s", ip))
				gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), ip);
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
cb_gn_interface_delete (GtkButton *button, gpointer data)
{
	gchar			*ptr = NULL;
	gchar			*ifname = NULL;
	GList			*interface = NULL;
	GtkTreeModel		*model = NULL;
	GtkTreeSelection	*selection = NULL;
	GtkTreeIter		iter;
	gboolean		found = FALSE;
	fwnet_interface_t*	inte = NULL;
	gint			ret;

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
	if (!found)
		gn_error ("Unknown error.", ERROR_GUI);

	ptr = g_strdup_printf ("ifconfig %s | grep UP > /dev/null", inte->name);
	if (!fwutil_system(ptr))
	{
		gn_error ("The selected interface is running. Please stop it first.", ERROR_GUI);
	}
	else
	{
		active_profile->interfaces = g_list_delete_link (active_profile->interfaces, interface);
		fwnet_writeconfig (active_profile, NULL);
		gn_message ("Interface deleted successfully");
		gnetconfig_populate_interface_list (active_profile);
	}
	g_free (ptr);

	return;
}

static void
cb_gn_interface_selected (GtkTreeSelection *selection, gpointer data)
{
	GList			*list = NULL;
	GList			*interface = NULL;
	GtkTreeModel		*model = NULL;
	gchar			*iface = NULL;
	gchar			*string = NULL;
	GtkTextBuffer		*buffer = NULL;
	GtkTreeIter		iter;
	GtkTextIter		t_iter;
	fwnet_interface_t	*inte = NULL;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_interface_treeview));
	list = gtk_tree_selection_get_selected_rows (selection, &model);
	if (!list)
		return;
	gtk_tree_model_get_iter (model, &iter, list->data);
	gtk_tree_model_get (model, &iter, 1, &iface, -1);

	for (interface = active_profile->interfaces;interface != NULL;interface=g_list_next(interface))
	{
		inte = interface->data;
		if (strcmp(iface, inte->name) == 0)
			break;
	}

	/* initialize the buffer */
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(gn_interface_textview));
	gtk_text_buffer_set_text (buffer, "", 0);
	gtk_text_buffer_get_iter_at_offset (buffer, &t_iter, 0);

	if (!fwnet_is_dhcp(inte) || (strlen(active_profile->adsl_interface) && !fwnet_is_dhcp(inte)))
	{
		char	ip[16], netmask[16];
		GList	*options = NULL;

		options = inte->options;
		if (!options) return;
		sscanf (options->data, "%s netmask %s", ip, netmask);
		string = g_strdup_printf ("Connection type: Static IP");
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		if (fwnet_is_wireless_device(inte->name))
			string = g_strdup_printf (" (Wireless Connection)\n\n");
		else if (strlen(active_profile->adsl_interface))
			string = g_strdup_printf (" (DSL Connection)\n\n");
		else
			string = g_strdup_printf (" \n\n");
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
		string = g_strdup_printf ("IP Address:\t %s\nSubnet Mask:\t %s\n", ip, netmask);
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
		sscanf (inte->gateway, "%*s gw %s", ip);
		string = g_strdup_printf ("Gateway:\t\t %s\n", ip);
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
	}
	else if (fwnet_is_dhcp(inte) && (!strlen(active_profile->adsl_interface)))
	{
		char host[255];

		string = g_strdup_printf ("Connection type:\t DHCP\n\n");
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
		if (!sscanf(inte->dhcp_opts, "%*s %*s -h %s", host) || (strlen(host)))
			string = g_strdup_printf ("DHCP Hostname:\t %s\n", host);
		else
			string = g_strdup_printf ("DHCP Hostname:\t (none)\n");
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
		if (strlen(inte->dhcpclient))
			string = g_strdup_printf ("DHCP Client:\t\t %s\n", inte->dhcpclient);
		else
			string = g_strdup_printf ("DHCP Client:\t\t dhcpcd\n");
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
	}
	if (strlen(inte->mac))
	{
		string = g_strdup_printf ("MAC Address:\t %s", inte->mac);
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
	}

	g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
	g_list_free (list);
	g_free (iface);

	return;
}

static void
cb_gn_interface_right_click (GtkTreeView *treeview, GdkEventButton *event)
{
	GtkWidget *menu;
	GtkWidget *menu_item;
	GtkWidget *image;

	if (event->button != 3)
		return;

	menu = gtk_menu_new ();

	menu_item = gtk_image_menu_item_new_with_label (_("Start"));
	image = gtk_image_new_from_stock ("gtk-connect", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(menu_item), image);
	g_signal_connect (G_OBJECT(menu_item), "activate", G_CALLBACK(cb_gn_interface_start), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL(menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_with_label (_("Stop"));
	image = gtk_image_new_from_stock ("gtk-disconnect", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(menu_item), image);
	g_signal_connect (G_OBJECT(menu_item), "activate", G_CALLBACK(cb_gn_interface_start), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL(menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_with_label (_("Edit"));
	image = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(menu_item), image);
	g_signal_connect (G_OBJECT(menu_item), "activate", G_CALLBACK(cb_gn_interface_edited), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL(menu), menu_item);
	gtk_widget_show (menu_item);

	gtk_widget_show (menu);
	gtk_menu_popup (GTK_MENU(menu),
			NULL,
			NULL,
			NULL,
			NULL,
			3,
			gtk_get_current_event_time());

	return;
}

static void
cb_gn_interface_double_click (GtkTreeView *treeview)
{
	cb_gn_interface_edited (NULL, NULL);

	return;
}

static void
cb_gn_conntype_changed (GtkComboBox *combo, gpointer data)
{
	switch (gtk_combo_box_get_active (combo))
	{
		gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), "");
		gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), "");
		gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), "");
		
		case GN_DHCP: /* DHCP */
			gtk_widget_show (gn_dhcp_table);
			gtk_widget_hide (gn_staticip_table);
			gtk_widget_hide (gn_dsl_table);
			break;

		case GN_STATIC: /* Static ip */
			gtk_widget_show (gn_staticip_table);
			gtk_widget_hide (gn_dhcp_table);
			gtk_widget_set_sensitive (gn_ipaddress_entry, TRUE);
			gtk_widget_set_sensitive (gn_netmask_entry, TRUE);
			gtk_widget_set_sensitive (gn_gateway_entry, TRUE);
			break;

		case GN_DSL: /* DSL */
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
cb_gn_dns_listview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval != GDK_Delete)
		return;

	cb_gn_delete_dns_clicked (NULL, NULL);

	return;
}

static void
cb_gn_delete_dns_clicked (GtkButton *button, gpointer data)
{
	GtkTreeModel	*model = NULL;
	GtkTreeIter	iter;
	GtkTreeSelection	*selection = NULL;
	gchar		*dns = NULL;
	GList		*l = NULL;

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
	gchar		*ipaddr = NULL;
	gchar		*netmask = NULL;
	gchar		*gateway = NULL;
	gchar		*if_name = NULL;
	GtkTreeModel	*model = NULL;
	GtkTreeIter	iter;
	char		opstring[50];
	GList		*intf = NULL;
	fwnet_interface_t	*interface = NULL;
	gint		type = -1;

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
			sprintf (interface->gateway, "default gw %s", gateway);
			type = GN_STATIC;

			break;
		}
		case GN_DHCP:
		{
			char *dhcp_hname = NULL;
			
			dhcp_hname = (char*)gtk_entry_get_text (GTK_ENTRY(gn_dhcp_hostname_entry));
			if (strlen(dhcp_hname))
				snprintf (interface->dhcp_opts, PATH_MAX, "-t 10 -h %s\n", dhcp_hname);
			else
				snprintf (interface->dhcp_opts, PATH_MAX, "-t 10\n");

			if (interface->options == NULL)
			{
				snprintf (opstring, 49, "dhcp");
				interface->options = g_list_append (interface->options, strdup(opstring));
			}
			else
				sprintf (interface->options->data, "dhcp");
			type = GN_DHCP;

			break;
		}
	}
	if ( fwnet_is_wireless_device(interface->name) || strlen(interface->essid) || strlen(interface->key) )
	{
		char *key, *essid;
		char *mode;
		ipaddr	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_ipaddress_entry));
		netmask	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_netmask_entry));
		gateway	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_gateway_entry));
		essid	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_essid_entry));
		key		= (char*)gtk_entry_get_text (GTK_ENTRY(gn_key_entry));
		mode	= gnetconfig_get_wireless_mode_string (gtk_combo_box_get_active(GTK_COMBO_BOX(gn_wireless_mode_combo)));

		if (strlen(key))
			snprintf (interface->key, FWNET_ENCODING_TOKEN_MAX, key);
		else
			*interface->key = '\0';

		if (strlen(essid))
			snprintf (interface->essid, FWNET_ESSID_MAX_SIZE, essid);
		else
			*interface->essid = '\0';

		if (mode!=NULL && (strlen(mode)))
		{	
			snprintf (interface->mode, FWNET_MODE_MAX_SIZE, mode);
			g_free (mode);
		}

		if (type == GN_STATIC)
		{
			if (interface->options == NULL)
			{	
				snprintf (opstring, 49, "%s netmask %s", ipaddr, netmask);
				interface->options = g_list_append (interface->options, strdup(opstring));
			}
			else
				interface->options->data = g_strdup_printf ("%s netmask %s", ipaddr,	netmask);
			sprintf (interface->gateway, "default gw %s", gateway);
		}
		else if (type == GN_DHCP)
		{
			if (interface->options == NULL)
			{
				snprintf (opstring, 49, "dhcp");
				interface->options = g_list_append (interface->options, strdup(opstring));
			}
			else
				interface->options->data = g_strdup_printf ("dhcp");
			char *hn = (char*) gtk_entry_get_text (GTK_ENTRY(gn_dhcp_hostname_entry));
			if (!hn && strlen(hn))
				snprintf (interface->dhcp_opts, PATH_MAX, "-t 10 -h %s\n", hn);
			else
				snprintf (interface->dhcp_opts, PATH_MAX, "-t 10\n");
		}
	}

	gnetconfig_save_profile (active_profile);

	return;
}

static void
cb_gn_hostname_save (GtkButton *button, gpointer data)
{
	gchar hostname[256];

	sprintf (hostname, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(gn_hostname_entry)));
	fwnet_writeconfig (active_profile, hostname);

	return;
}

