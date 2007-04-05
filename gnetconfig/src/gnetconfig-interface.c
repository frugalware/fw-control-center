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

/* utility functions */
static void gnetconfig_populate_profile_list (void);
static void gnetconfig_populate_interface_list (fwnet_profile_t *profile);
static void gnetconfig_load_profile (const char *name);

/* callbacks */
static void cb_gn_profile_changed (GtkComboBox *combo, gpointer data);
static void cb_gn_interface_changed (GtkComboBox *combo, gpointer data);
static void cb_gn_conntype_changed (GtkComboBox *combo, gpointer data);

void
gnetconfig_interface_init (void)
{
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

	/* setup profiles combobox */
	model = GTK_TREE_MODEL(gtk_list_store_new (1, G_TYPE_STRING));
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(gn_profile_combo), renderer, FALSE);
	gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(gn_profile_combo), renderer, "text", 0);
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
	while (file != NULL)
	{
		if ((fn=fwnet_lastprofile()))
		{
			if (!strcmp(file,fn))
				index++;
		}
		gtk_list_store_append (gn_profile_store, &iter);
		gtk_list_store_set (gn_profile_store, &iter, 0, file, -1);
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
gnetconfig_load_profile (const char *name)
{
	fwnet_profile_t		*profile;
	char				hostname[256];

	if (!(profile = fwnet_parseprofile (name)))
		return;

	/* set the active profile */
	active_profile = profile;

	/* populate the list of interfaces */
	gnetconfig_populate_interface_list (profile);

	/* read the hostname */
	gnetconfig_read_hostname(hostname);
	if (hostname)
		gtk_entry_set_text (GTK_ENTRY(gn_hostname_entry), hostname);
	else
		g_error ("couldn't read hostname.");

	return;
}

static void
cb_gn_profile_changed (GtkComboBox *combo, gpointer data)
{
	GtkTreeIter	iter;
	GtkTreeModel	*model = NULL;
	gchar		*text = NULL;
	
	if (gtk_combo_box_get_active_iter(combo, &iter))
	{
		model = gtk_combo_box_get_model(combo);
		gtk_tree_model_get (model, &iter, 0, &text, -1);
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
	GtkListStore		*store = NULL;
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
			options = inte->options;
			sscanf (options->data, "%s netmask %s", ip, netmask);
			gtk_entry_set_text (GTK_ENTRY(gn_ipaddress_entry), ip);
			gtk_entry_set_text (GTK_ENTRY(gn_netmask_entry), netmask);
			sscanf (inte->gateway, "%*s gw %s", ip);
			gtk_entry_set_text (GTK_ENTRY(gn_gateway_entry), ip);

			/* set the correct connection type */
			if ((!fwnet_is_dhcp(inte)) && (!strlen(active_profile->adsl_interface)))
				gtk_combo_box_set_active (GTK_COMBO_BOX(gn_conntype_combo), 1);
		}
		else
			g_error ("no network interface found");
	}

	return;
}

static void
cb_gn_conntype_changed (GtkComboBox *combo, gpointer data)
{
	return;
}


