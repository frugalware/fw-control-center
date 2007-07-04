/*
 *  gnetconfig-interface.c for gnetconfig
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

#include <locale.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib/gstdio.h>
#include <gdk/gdkkeysyms.h>

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "gnetconfig-interface.h"
#include "gnetconfig-about.h"
#include "gnetconfig-messages.h"
#include "gnetconfig-misc.h"
#include "gnetconfig-new_profile.h"
#include "gnetconfig-new_interface.h"
#include "gnetconfig-interface_opts.h"

#include <libfwutil.h>

#define IFACE_WIRED "/share/pixmaps/gnetconfig-wired.png"
#define IFACE_WIRELESS "/share/pixmaps/gnetconfig-wireless.png"
#define IFACE_UP "/share/pixmaps/network-up.png"
#define IFACE_DN "/share/pixmaps/network-down.png"

extern GladeXML *xml;

fwnet_profile_t *active_profile;

GtkWidget *gn_main_window;
GtkWidget *gn_profile_combo;
GtkWidget *gn_interface_combo;
GtkWidget *gn_conntype_combo;
GtkWidget *gn_dhcp_client_combo;
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
GtkWidget *gn_config_dsl_check;
GtkWidget *gn_statusbar;
GtkWidget *gn_iflabel;
GtkWidget *gn_profile_desc;

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
static void gnetconfig_update_status (const gchar *msg);

/* new nameserver dialog */
static void gnetconfig_new_nameserver_dialog_show (void);

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
static void cb_gn_delete_profile_clicked (GtkButton *button, gpointer data);
static void cb_gn_profile_desc_save_clicked (GtkButton *button, gpointer data);
static void cb_gn_dns_listview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data);
static void cb_gn_interface_double_click (GtkTreeView *treeview);
static void cb_gn_interface_right_click (GtkTreeView *treeview, GdkEventButton *event);
static void cb_gn_config_dsl_check_toggle (GtkToggleButton *togglebutton, gpointer data);

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
	gn_dhcp_client_combo	= glade_xml_get_widget (xml, "fwn_dhcp_client_combo");
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
	gn_config_dsl_check	= glade_xml_get_widget (xml, "fwn_config_dsl_check2");
	gn_statusbar		= glade_xml_get_widget (xml, "fwn_statusbar");
	gn_iflabel		= glade_xml_get_widget (xml, "fwn_interface_label");
	gn_profile_desc = glade_xml_get_widget (xml, "fwn_profile_desc");

	/* new widgets */
	gn_interface_dialog = glade_xml_get_widget (xml, "interface_edit_dialog");
	gn_interface_treeview = glade_xml_get_widget (xml, "interface_treeview");

	/* Set the title for some stuff */
	gtk_window_set_title(GTK_WINDOW(gn_main_window), PACKAGE_STRING);
	gtk_window_set_title(GTK_WINDOW(gn_interface_dialog), _("Configure interface"));

	gtk_window_set_transient_for (GTK_WINDOW(gn_interface_dialog), GTK_WINDOW(gn_main_window));
	gtk_window_set_position (GTK_WINDOW(gn_interface_dialog), GTK_WIN_POS_CENTER_ON_PARENT);

	renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes ("IF_Icon",
							renderer,
							"pixbuf", 0,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_interface_treeview), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("IF_Name",
							renderer,
							"text", 1,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_column_set_min_width (column, 120);
	g_object_set (G_OBJECT(column), "expand", TRUE, "spacing", 4, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_interface_treeview), column);

	renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes ("IF_StatusIcon",
							renderer,
							"pixbuf", 2,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_interface_treeview), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("IF_StatusText",
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

	/* other stuff */
	widget = glade_xml_get_widget (xml, "fwn_interface_save");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_save_interface_clicked),
			NULL);

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
			NULL);
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
	widget = glade_xml_get_widget (xml, "fwn_delete_profile");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_delete_profile_clicked),
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
	widget = glade_xml_get_widget (xml, "fwn_profile_desc_save");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_profile_desc_save_clicked),
			NULL);

	g_signal_connect (G_OBJECT(gn_config_dsl_check),
			"toggled",
			G_CALLBACK(cb_gn_config_dsl_check_toggle),
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

	g_signal_connect (gn_interface_dialog,
			"delete-event",
			G_CALLBACK (gtk_widget_hide),
			NULL);

	/* Load main stuff */
	gnetconfig_populate_profile_list ();
	gnetconfig_new_profile_dialog_init ();
	gnetconfig_interface_opts_init ();
	gnetconfig_new_interface_dialog_setup ();
	gtk_widget_show (gn_main_window);

	/* unref the libglade object */
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
		g_error (_("Error parsing profile directory."));
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
				gchar *msg = g_strdup_printf ("%s %s", _("Active network profile:"), fn);
				gnetconfig_update_status (msg);
				g_free (msg);
			}
			g_free (fn);
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
	GdkPixbuf		*up_pixbuf;
	GdkPixbuf		*dn_pixbuf;
	GdkPixbuf		*wired_pixbuf;
	GdkPixbuf		*wireless_pixbuf;
	gchar			*ptr = NULL;
	gboolean		flag = FALSE;

	n_ifs = g_list_length (profile->interfaces);
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_interface_treeview));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);
	ptr = g_strdup_printf ("%s%s", PREFIX, IFACE_UP);
	up_pixbuf = gdk_pixbuf_new_from_file_at_size (ptr, 26, 26, NULL);
	g_free (ptr);
	ptr = g_strdup_printf ("%s%s", PREFIX, IFACE_DN);
	dn_pixbuf = gdk_pixbuf_new_from_file_at_size (ptr, 26, 26, NULL);
	g_free (ptr);
	ptr = g_strdup_printf ("%s%s", PREFIX, IFACE_WIRED);
	wired_pixbuf = gdk_pixbuf_new_from_file_at_size (ptr, 26, 26, NULL);
	g_free (ptr);
	ptr = g_strdup_printf ("%s%s", PREFIX, IFACE_WIRELESS);
	wireless_pixbuf = gdk_pixbuf_new_from_file_at_size (ptr, 26, 26, NULL);
	g_free (ptr);

	if ((strcmp(profile->name, fwnet_lastprofile())) == 0)
		flag = TRUE;

	for (i=0;i<n_ifs;i++)
	{
		interface = g_list_nth_data (profile->interfaces, i);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 1, interface->name, -1);
		if (!fwnet_is_wireless_device(interface->name))
			gtk_list_store_set (store, &iter, 0, wired_pixbuf, -1);
		else
			gtk_list_store_set (store, &iter, 0, wireless_pixbuf, -1);

		ptr = g_strdup_printf ("ifconfig %s | grep UP > /dev/null", interface->name);
		if (flag == TRUE)
		{
			if (!fwutil_system(ptr))
				gtk_list_store_set (store, &iter, 2, up_pixbuf, 3, " UP", -1);
			else
				gtk_list_store_set (store, &iter, 2, dn_pixbuf, 3, " DOWN", -1);
		}
		else
		{
			gtk_list_store_set (store, &iter, 2, dn_pixbuf, 3, " DOWN", -1);
		}
		g_free (ptr);
	}
	g_object_unref (up_pixbuf);
	g_object_unref (dn_pixbuf);
	g_object_unref (wired_pixbuf);
	g_object_unref (wireless_pixbuf);

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

//	gtk_widget_show (gn_dns_listview);

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

	/* read the profile desc */
	if (strlen(active_profile->desc))
		gtk_entry_set_text (GTK_ENTRY(gn_profile_desc), active_profile->desc);
	else
		gtk_entry_set_text (GTK_ENTRY(gn_profile_desc), "");

	/* read the hostname */
	gnetconfig_read_hostname (hostname);
	if (strlen(hostname))
		gtk_entry_set_text (GTK_ENTRY(gn_hostname_entry), hostname);
	else
		gn_error (_("Couldn't read hostname."));

	return;
}

int
gnetconfig_save_profile (fwnet_profile_t *profile)
{
	gchar	*buf = NULL;

	buf = g_strdup (profile->name);
	if(fwnet_writeconfig (profile, NULL))
		return 1;

	/* the profile data gets corrupted after saving and
	 * hence needs to be reloaded */
	gnetconfig_profile_free (profile);
	active_profile = fwnet_parseprofile (buf);
	g_free (buf);

	return 0;
}

static void
gnetconfig_new_nameserver_dialog_show (void)
{
	char	*ip = NULL;
	gint	res;

	up:ip = gn_input (_("New DNS"), _("Enter the ip address of the nameserver:"), &res);
	if (res == GTK_RESPONSE_ACCEPT)
	{
		if (ip == NULL || !strlen(ip))
		{
			gn_error (_("Required field cannot be left blank."));
			goto up;
		}
		active_profile->dnses = g_list_append (active_profile->dnses, (gpointer)g_strdup(ip));
		gnetconfig_save_profile (active_profile);
		gnetconfig_populate_dns_list (active_profile->dnses);
		g_free (ip);
	}

	return;
}

static void
gnetconfig_update_status (const gchar *msg)
{
	static guint ci = 0;

	if (ci)
		gtk_statusbar_pop (GTK_STATUSBAR(gn_statusbar), ci);
	ci = gtk_statusbar_get_context_id (GTK_STATUSBAR(gn_statusbar), "-");
	ci = gtk_statusbar_push (GTK_STATUSBAR(gn_statusbar), ci, msg);

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
cb_gn_delete_profile_clicked (GtkButton *button, gpointer data)
{
	GtkTreeModel	*model = NULL;
	GtkTreeIter	iter;
	gchar		*path = NULL;
	gchar		*profile = NULL;

	gtk_combo_box_get_active_iter (GTK_COMBO_BOX(gn_profile_combo), &iter);
	model = gtk_combo_box_get_model (GTK_COMBO_BOX(gn_profile_combo));
	gtk_tree_model_get (model, &iter, 1, &profile, -1);
	if (strcmp(profile, fwnet_lastprofile()))
	{
		switch ( gn_question (_("Are you sure you want to delete this profile ?")) )
		{
			case GTK_RESPONSE_YES:
				path = g_strdup_printf ("/etc/sysconfig/network/%s", profile);
				gint ret = g_remove (path);
				if (ret == -1)
					gn_error (_("There was an error deleting the profile."));
				else
					gn_message (_("Profile deleted successfully"));
				g_free (path);
				gnetconfig_populate_profile_list ();
				break;

			case GTK_RESPONSE_NO:
			case GTK_RESPONSE_DELETE_EVENT:
					break;
		}
	}
	else
	{
		gn_error (_("You cannot delete an active network profile."));
		g_free (profile);
	}

	return;
}

static void
cb_gn_profile_desc_save_clicked (GtkButton *button, gpointer data)
{
	char *desc = NULL;

	desc = (char*) gtk_entry_get_text (GTK_ENTRY(gn_profile_desc));
	if (desc == NULL)
		*active_profile->desc = '\0';
	else
		snprintf (active_profile->desc, PATH_MAX, desc);
	gnetconfig_save_profile (active_profile);

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
	gchar			*profile = NULL;
	gchar			*ifname = NULL;
	GList			*interface = NULL;
	GtkTreeModel		*model = NULL;
	GtkTreeSelection	*selection = NULL;
	GtkTreeIter		iter;
	gboolean		found = FALSE;
	fwnet_interface_t*	inte = NULL;
	gint			ret;

	gtk_combo_box_get_active_iter (GTK_COMBO_BOX(gn_profile_combo), &iter);
	model = gtk_combo_box_get_model (GTK_COMBO_BOX(gn_profile_combo));
	gtk_tree_model_get (model, &iter, 1, &profile, -1); 
	if (strcmp(profile,fwnet_lastprofile())!=0)
	{
		gn_error (_("This interface cannot be activated. It belongs to an inactive profile."));
		return;
	}

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
		gn_error (_("Unknown error."));

	ptr = g_strdup_printf ("ifconfig %s | grep UP > /dev/null", inte->name);
	if (!fwutil_system(ptr))
		gn_error (_("Interface is already started."));
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
	gchar			*profile = NULL;
	GList			*interface = NULL;
	GtkTreeModel		*model = NULL;
	GtkTreeSelection	*selection = NULL;
	GtkTreeIter		iter;
	gboolean		found = FALSE;
	fwnet_interface_t*	inte = NULL;
	gint			ret;

	gtk_combo_box_get_active_iter (GTK_COMBO_BOX(gn_profile_combo), &iter);
	model = gtk_combo_box_get_model (GTK_COMBO_BOX(gn_profile_combo));
	gtk_tree_model_get (model, &iter, 1, &profile, -1); 
	if (strcmp(profile,fwnet_lastprofile())!=0)
	{
		gn_error (_("This interface cannot be de-activated. It belongs to an inactive profile."));
		return;
	}

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
		gn_error (_("Unknown error."));

	ptr = g_strdup_printf ("ifconfig %s | grep UP > /dev/null", inte->name);
	if (fwutil_system(ptr))
		gn_error (_("Interface is not running."));
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

		/* set the interface name label */
		markup = g_markup_printf_escaped ("<span size=\"medium\"><b>%s</b></span>", inte->name);
		gtk_label_set_markup (GTK_LABEL(gn_iflabel), markup);
		g_free (markup);

		/* populate the opts list */
		gnetconfig_populate_opts (inte);

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
			if (strlen(inte->gateway))
			{
				sscanf (inte->gateway, "%*s gw %s", ip);
				gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), ip);
			}
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
			if (!strlen(inte->dhcpclient) || !strcmp(inte->dhcpclient,"dhcpcd"))
				gtk_combo_box_set_active (GTK_COMBO_BOX(gn_dhcp_client_combo), GN_DHCPCD);
			else if (!strcmp(inte->dhcpclient, "dhclient"))
				gtk_combo_box_set_active (GTK_COMBO_BOX(gn_dhcp_client_combo), GN_DHCLIENT);
			if (strlen(inte->dhcp_opts) && (g_strrstr(inte->dhcp_opts, "-h")!=NULL))
			{
				if (sscanf (inte->dhcp_opts, "%*s %*s -h %s", host))
					gtk_entry_set_text (GTK_ENTRY(gn_dhcp_hostname_entry), host);
			}
			dsl_conn = GN_DHCP;
		}
		if (strlen(active_profile->adsl_interface) && !(strcmp(inte->name, active_profile->adsl_interface)))
		{
			/* DSL Active */
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(gn_config_dsl_check), TRUE);
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

		gtk_widget_show (GTK_WIDGET(gn_interface_dialog));
	}
	else
		gn_error (_("No network interface found."));

	return;
}

static void
cb_gn_interface_delete (GtkButton *button, gpointer data)
{
	gchar			*ptr = NULL;
	gchar			*ifname = NULL;
	gchar			*profile = NULL;
	GList			*interface = NULL;
	GtkTreeModel		*model = NULL;
	GtkTreeSelection	*selection = NULL;
	GtkTreeIter		iter;
	gboolean		found = FALSE;
	fwnet_interface_t*	inte = NULL;

	if (gn_question (_("Are you sure you want to delete this interface ?")) == GTK_RESPONSE_NO)
		return;

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
		gn_error (_("Unknown error."));

	gtk_combo_box_get_active_iter (GTK_COMBO_BOX(gn_profile_combo), &iter);
	model = gtk_combo_box_get_model (GTK_COMBO_BOX(gn_profile_combo));
	gtk_tree_model_get (model, &iter, 1, &profile, -1);
	ptr = g_strdup_printf ("ifconfig %s | grep UP > /dev/null", inte->name);
	if (!(strcmp(active_profile->name, profile)) && !fwutil_system(ptr))
	{
		gn_error (_("The selected interface is running. Please stop it first."));
	}
	else
	{
		active_profile->interfaces = g_list_delete_link (active_profile->interfaces, interface);
		fwnet_writeconfig (active_profile, NULL);
		gn_message (_("Interface deleted successfully"));
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
		gint	w = 0;

		options = inte->options;
		if (!options) return;
		sscanf (options->data, "%s netmask %s", ip, netmask);
		string = g_strdup_printf (_("Connection type: Static IP"));
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		if (fwnet_is_wireless_device(inte->name))
		{
			w = 1;
			string = g_strdup_printf (_(" (Wireless Connection)\n\n"));
		}
		else if (strlen(active_profile->adsl_interface))
		{	
			if (!strcmp(active_profile->adsl_interface, inte->name))
				string = g_strdup_printf (" (DSL Connection)\n\n");
		}
		else
		{
			string = g_strdup_printf (" \n\n");
		}
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
		string = g_strdup_printf ("IP Address:\t %s\nSubnet Mask:\t %s\n", ip, netmask);
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
		if (strlen(inte->gateway))
		{
			ip[0] = '\0';
			sscanf (inte->gateway, "%*s gw %s", ip);
			string = g_strdup_printf ("Gateway:\t\t %s\n", ip);
			gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
			g_free (string);
		}
		if (w == 1)
		{
			string = g_strdup_printf (_("\nWireless connection details:\n"));
			gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
			g_free (string);
			string = g_strdup_printf ("Mode: \t%s\nESSID: \t%s\n",
						inte->mode,
						inte->essid);
			gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
			g_free (string);
		}
	}
	else if (fwnet_is_dhcp(inte) && (!strlen(active_profile->adsl_interface)))
	{
		char host[255];

		string = g_strdup_printf ("Connection type:\t DHCP\n\n");
		gtk_text_buffer_insert (buffer, &t_iter, string, strlen(string));
		g_free (string);
		if ((strlen(inte->dhcp_opts)) && (g_strrstr(inte->dhcp_opts, "-h")!=NULL))
		{
			if (sscanf(inte->dhcp_opts, "%*s %*s -h %s", host))
			string = g_strdup_printf ("DHCP Hostname:\t %s\n", host);
		}
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

	menu_item = gtk_image_menu_item_new_with_label (_("Configure"));
	image = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(menu_item), image);
	g_signal_connect (G_OBJECT(menu_item), "activate", G_CALLBACK(cb_gn_interface_edited), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL(menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_with_label (_("Delete"));
	image = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(menu_item), image);
	g_signal_connect (G_OBJECT(menu_item), "activate", G_CALLBACK(cb_gn_interface_delete), NULL);
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

		case GN_LO: /* lo */
			break;
	}

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
	GtkTreeModel		*model = NULL;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection = NULL;
	gchar			*dns = NULL;
	GList			*l = NULL;

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
	gint			type = -1;

	if_name = (gchar*)gtk_label_get_text (GTK_LABEL(gn_iflabel));
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

			switch (gtk_combo_box_get_active (GTK_COMBO_BOX(gn_dhcp_client_combo)))
			{
				case GN_DHCPCD:
					snprintf (interface->dhcpclient, PATH_MAX, "dhcpcd");
					break;
				case GN_DHCLIENT:
					snprintf (interface->dhcpclient, PATH_MAX, "dhclient");
					break;
				default:
					*interface->dhcpclient = '\0';
			}
			type = GN_DHCP;

			break;
		}
	}
	if (TRUE == gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gn_config_dsl_check))) /* The interface is to be configured for dsl */
	{
		gchar *username;
		gchar *pass;
		gchar *cpass;

		username = (gchar*)gtk_entry_get_text (GTK_ENTRY(gn_dsl_username_entry));
		pass = (gchar*)gtk_entry_get_text (GTK_ENTRY(gn_dsl_password_entry));
		cpass = (gchar*)gtk_entry_get_text (GTK_ENTRY(gn_dsl_cpassword_entry));

		if (!strlen(username) || !strlen(pass) || !strlen(cpass))
		{
			gn_error (_("Required fields for DSL connection cannot be left blank."));
			return;
		}
		if (strcmp(pass,cpass) != 0)
		{
			gn_error (_("PPPoE passwords do not match. Please re-enter."));
			return;
		}
		/* hopefully, everything is ok now and we should save the profile */
		snprintf (active_profile->adsl_username, PATH_MAX, username);
		snprintf (active_profile->adsl_password, PATH_MAX, pass);
		snprintf (active_profile->adsl_interface, PATH_MAX, gtk_label_get_text(GTK_LABEL(gn_iflabel)));
	}
	if ( fwnet_is_wireless_device(interface->name) || strlen(interface->essid) || strlen(interface->key) )
	{
		char *key, *essid;
		char *mode;
		ipaddr	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_ipaddress_entry));
		netmask	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_netmask_entry));
		gateway	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_gateway_entry));
		essid	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_essid_entry));
		key	= (char*)gtk_entry_get_text (GTK_ENTRY(gn_key_entry));
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

			switch (gtk_combo_box_get_active (GTK_COMBO_BOX(gn_dhcp_client_combo)))
			{
				case GN_DHCPCD:
					snprintf (interface->dhcpclient, PATH_MAX, "dhcpcd");
					break;
				case GN_DHCLIENT:
					snprintf (interface->dhcpclient, PATH_MAX, "dhclient");
					break;
				default:
					*interface->dhcpclient = '\0';
			}
		}
	}

	gnetconfig_save_profile (active_profile);

	return;
}

static void
cb_gn_config_dsl_check_toggle (GtkToggleButton *togglebutton, gpointer data)
{
	if (TRUE == gtk_toggle_button_get_active(togglebutton))
	{
		gtk_widget_show (gn_dsl_table);
	}
	else
	{
		gtk_widget_hide (gn_dsl_table);
	}

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

