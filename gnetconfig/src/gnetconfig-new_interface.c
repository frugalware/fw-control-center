/***************************************************************************
 *  gnetconfig-new_interface.c
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

#include "gnetconfig-new_interface.h"
#include "gnetconfig-messages.h"
#include "gnetconfig-misc.h"

extern GladeXML 	*xml;
extern GtkWidget	*gn_main_window;
extern fwnet_profile_t	*active_profile;

GtkWidget *dialog;

GtkWidget *gn_nconntype_combo;
GtkWidget *gn_nstaticip_table;
GtkWidget *gn_ndhcp_table;
GtkWidget *gn_ndsl_table;
GtkWidget *gn_nwireless_table;
GtkWidget *gn_nif_name_entry;
GtkWidget *gn_nipaddress_entry;
GtkWidget *gn_ngateway_entry;
GtkWidget *gn_nnetmask_entry;
GtkWidget *gn_nessid_entry;
GtkWidget *gn_nkey_entry;
GtkWidget *gn_nwireless_mode_combo;
GtkWidget *gn_ndhcp_hostname_entry;

static void cb_gn_nconntype_changed (GtkComboBox *combo, gpointer data);
static void cb_gn_new_int_save_clicked (GtkWidget *widget, gpointer data);
static void cb_gn_new_int_close_clicked (GtkWidget *widget, gpointer data);
static void cb_gn_new_int_autodetect_clicked (GtkWidget *widget, gpointer data);

void
gnetconfig_new_interface_dialog_setup (void)
{
	GtkWidget *widget;

	dialog = glade_xml_get_widget (xml, "new_interface_dialog");

	/* Lookup all widgets */
	gn_nif_name_entry	= glade_xml_get_widget (xml, "fwn_if_name");
	gn_nstaticip_table	= glade_xml_get_widget (xml, "fwn_staticip_table2");
	gn_nwireless_table	= glade_xml_get_widget (xml, "fwn_wireless_table2");
	gn_ndhcp_table		= glade_xml_get_widget (xml, "fwn_dhcp_table2");
	gn_nconntype_combo	= glade_xml_get_widget (xml, "fwn_conntype_list2");
	gn_nipaddress_entry	= glade_xml_get_widget (xml, "fwn_ip2");
	gn_nnetmask_entry	= glade_xml_get_widget (xml, "fwn_netmask2");
	gn_ngateway_entry	= glade_xml_get_widget (xml, "fwn_gateway2");
	gn_ndhcp_hostname_entry = glade_xml_get_widget (xml, "fwn_dhcp_hostname2");

	/* setup signals and callbacks */
	widget = glade_xml_get_widget (xml, "fwn_new_int_save");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_new_int_save_clicked),
			NULL);

	widget = glade_xml_get_widget (xml, "fwn_new_int_close");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_new_int_close_clicked),
			NULL);

	widget = glade_xml_get_widget (xml, "fwn_if_autodetect");
	g_signal_connect (G_OBJECT(widget),
			"clicked",
			G_CALLBACK(cb_gn_new_int_autodetect_clicked),
			NULL);

	g_signal_connect (G_OBJECT(gn_nconntype_combo),
			"changed",
			G_CALLBACK(cb_gn_nconntype_changed),
			NULL);

	/* center the dialog on the main window */
	gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(gn_main_window));
	gtk_window_set_position (GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);

	/* initialize interface detection dialog */
	gnetconfig_if_detect_dlg_init ();

	return;
}
		
void
gnetconfig_new_interface_dialog_show (void)
{
	if (!GTK_WIDGET_VISIBLE(dialog))
	{
		gtk_widget_show (dialog);
		gtk_window_present (GTK_WINDOW(dialog));
	}
	else
	{
		gtk_window_present (GTK_WINDOW(dialog));
	}
	return;
}

/* CALLBACKS */

static void
cb_gn_nconntype_changed (GtkComboBox *combo, gpointer data)
{
	switch (gtk_combo_box_get_active (combo))
	{
		gtk_entry_set_text (GTK_ENTRY(gn_nipaddress_entry), "");
		gtk_entry_set_text (GTK_ENTRY(gn_nnetmask_entry), "");
		gtk_entry_set_text (GTK_ENTRY(gn_ngateway_entry), "");
		
		case GN_DHCP: /* DHCP */
			gtk_widget_show (gn_ndhcp_table);
			gtk_widget_hide (gn_nstaticip_table);
			//gtk_widget_hide (gn_ndsl_table);
			break;

		case GN_STATIC: /* Static ip */
			gtk_widget_show (gn_nstaticip_table);
			gtk_widget_hide (gn_ndhcp_table);
			gtk_widget_set_sensitive (gn_nipaddress_entry, TRUE);
			gtk_widget_set_sensitive (gn_nnetmask_entry, TRUE);
			gtk_widget_set_sensitive (gn_ngateway_entry, TRUE);
			break;

		case GN_DSL: /* DSL */
			//gtk_widget_show (gn_ndsl_table);
			break;

		case GN_LO: /* lo */
			break;
	}

	return;
}

static void
cb_gn_new_int_save_clicked (GtkWidget *widget, gpointer data)
{
	fwnet_interface_t 	*nif;
	gchar			opts[50];

	nif = (fwnet_interface_t*)malloc(sizeof(fwnet_interface_t));
	if (!nif)
	{
		gn_error ("Error allocating memory for new profile.", ERROR_GUI);
		return;
	}
	memset (nif, 0, sizeof(fwnet_interface_t));
	snprintf (nif->name, IF_NAMESIZE, (char*)gtk_entry_get_text(GTK_ENTRY(gn_nif_name_entry)));
	switch (gtk_combo_box_get_active (GTK_COMBO_BOX(gn_nconntype_combo)))
	{
		case GN_STATIC:
			snprintf (opts, 49, "%s netmask %s",
				(char*)gtk_entry_get_text (GTK_ENTRY(gn_nipaddress_entry)),
				(char*)gtk_entry_get_text (GTK_ENTRY(gn_nnetmask_entry)));
			nif->options = g_list_append (nif->options, opts);
			snprintf (nif->gateway, FWNET_GW_MAX_SIZE, "default gw %s",
				(char*)gtk_entry_get_text (GTK_ENTRY(gn_ngateway_entry)));
			break;
		case GN_DHCP:
			sprintf (opts, "dhcp");
			nif->options = g_list_append (nif->options, opts);
			gchar *temp = (gchar*)gtk_entry_get_text (GTK_ENTRY(gn_ndhcp_hostname_entry));
			if (strlen(temp))
				snprintf (nif->dhcp_opts, PATH_MAX, "-t 10 -h %s\n", temp);
			else
				nif->dhcp_opts[0] = '\0';
			break;
	}
	active_profile->interfaces = g_list_append (active_profile->interfaces, nif);
	gnetconfig_save_profile (active_profile);
	gnetconfig_populate_interface_list (active_profile);

	return;
}

static void
cb_gn_new_int_close_clicked (GtkWidget *widget, gpointer data)
{
	gtk_widget_hide (dialog);
	return;
}

static void
cb_gn_new_int_autodetect_clicked (GtkWidget *widget, gpointer data)
{
	gnetconfig_detect_interfaces ();
	return;
}

