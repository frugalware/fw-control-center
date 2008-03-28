/*
 *  gnetconfig-new_interface.c for gnetconfig
 *
 *  Copyright (C) 2007-2008 by Priyank Gosalia <priyankmg@gmail.com>
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
#include <gdk/gdkkeysyms.h>

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "gnetconfig-new_interface.h"
#include "gnetconfig-interface.h"
#include "gnetconfig-interface_detect.h"
#include "gnetconfig-messages.h"
#include "gnetconfig-misc.h"

extern GladeXML 	*xml;
extern GtkWidget	*gn_main_window;
extern fwnet_profile_t	*active_profile;

GtkWidget *gn_if_add_dialog;

GtkWidget *gn_nconntype_combo;
GtkWidget *gn_ndhcp_client_combo;
GtkWidget *gn_nstaticip_table;
GtkWidget *gn_ndhcp_table;
GtkWidget *gn_ndsl_table;
GtkWidget *gn_nwireless_table;
GtkWidget *gn_nif_name_entry;
GtkWidget *gn_nipaddress_entry;
GtkWidget *gn_ngateway_entry;
GtkWidget *gn_nnetmask_entry;
GtkWidget *gn_nwmode_combo;
GtkWidget *gn_nessid_entry;
GtkWidget *gn_nkey_entry;
GtkWidget *gn_nwireless_mode_combo;
GtkWidget *gn_ndhcp_hostname_entry;
GtkWidget *gn_nif_dsl_check;
GtkWidget *gn_ndsl_username;
GtkWidget *gn_ndsl_password;
GtkWidget *gn_ndsl_cpassword;

static gboolean check_dsl = FALSE;

static void gnetconfig_reset_nif_dialog (void);

static void cb_gn_nconntype_changed (GtkComboBox *combo, gpointer data);
static void cb_gn_new_int_save_clicked (GtkWidget *widget, gpointer data);
static void cb_gn_new_int_close_clicked (GtkWidget *widget, gpointer data);
static void cb_gn_new_int_autodetect_clicked (GtkWidget *widget, gpointer data);
static void cb_gn_config_dsl_changed (GtkToggleButton *togglebutton, gpointer data);

void
gnetconfig_new_interface_dialog_setup (void)
{
	GtkWidget *widget;

	gn_if_add_dialog = glade_xml_get_widget (xml, "new_interface_dialog");

	/* Lookup all widgets */
	gn_nif_name_entry	= glade_xml_get_widget (xml, "fwn_if_name");
	gn_nstaticip_table	= glade_xml_get_widget (xml, "fwn_staticip_table2");
	gn_nwireless_table	= glade_xml_get_widget (xml, "fwn_wireless_table2");
	gn_ndhcp_table		= glade_xml_get_widget (xml, "fwn_dhcp_table2");
	gn_ndsl_table		= glade_xml_get_widget (xml, "fwn_dsl_table3");
	gn_nconntype_combo	= glade_xml_get_widget (xml, "fwn_conntype_list2");
	gn_ndhcp_client_combo	= glade_xml_get_widget (xml, "fwn_dhcp_client_combo2");
	gn_nipaddress_entry	= glade_xml_get_widget (xml, "fwn_ip2");
	gn_nnetmask_entry	= glade_xml_get_widget (xml, "fwn_netmask2");
	gn_ngateway_entry	= glade_xml_get_widget (xml, "fwn_gateway2");
	gn_ndhcp_hostname_entry = glade_xml_get_widget (xml, "fwn_dhcp_hostname2");
	gn_nkey_entry		= glade_xml_get_widget (xml, "fwn_key_entry2");
	gn_nessid_entry		= glade_xml_get_widget (xml, "fwn_essid_entry2");
	gn_nwmode_combo		= glade_xml_get_widget (xml, "fwn_wmode_combo2");
	gn_ndsl_username	= glade_xml_get_widget (xml, "fwn_dsl_username3");
	gn_ndsl_password	= glade_xml_get_widget (xml, "fwn_dsl_password3");
	gn_ndsl_cpassword	= glade_xml_get_widget (xml, "fwn_dsl_cpassword3");

	gtk_window_set_title(GTK_WINDOW(gn_if_add_dialog), _("Add new interface"));

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

	gn_nif_dsl_check = glade_xml_get_widget (xml, "fwn_config_dsl_check");
	g_signal_connect (G_OBJECT(gn_nif_dsl_check),
			"toggled",
			G_CALLBACK(cb_gn_config_dsl_changed),
			NULL);

	g_signal_connect (G_OBJECT(gn_nconntype_combo),
			"changed",
			G_CALLBACK(cb_gn_nconntype_changed),
			NULL);

	g_signal_connect (G_OBJECT(gn_if_add_dialog),
			"delete-event",
			G_CALLBACK(gtk_widget_hide),
			NULL);

	/* center the dialog on the main window */
	gtk_window_set_transient_for (GTK_WINDOW(gn_if_add_dialog), GTK_WINDOW(gn_main_window));
	gtk_window_set_position (GTK_WINDOW(gn_if_add_dialog), GTK_WIN_POS_CENTER_ON_PARENT);

	/* initialize interface detection dialog */
	gnetconfig_if_detect_dlg_init ();

	/* hide the dsl checkbox for now */
	gtk_widget_hide (gn_nif_dsl_check);

	return;
}

static void
gnetconfig_reset_nif_dialog (void)
{
	gtk_combo_box_set_active (GTK_COMBO_BOX(gn_nconntype_combo), -1);
	gtk_combo_box_set_active (GTK_COMBO_BOX(gn_nwmode_combo), -1);
	gtk_combo_box_set_active (GTK_COMBO_BOX(gn_ndhcp_client_combo), -1);
	gtk_entry_set_text (GTK_ENTRY(gn_nif_name_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_nipaddress_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_nnetmask_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_ngateway_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_nessid_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_nkey_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_ndhcp_hostname_entry), "");
	gtk_widget_hide (gn_nwireless_table);
	gtk_widget_hide (gn_nstaticip_table);
	gtk_widget_hide (gn_ndhcp_table);

	return;
}

void
gnetconfig_new_interface_dialog_show (void)
{
	gtk_widget_hide (gn_nwireless_table);

	if (!GTK_WIDGET_VISIBLE(gn_if_add_dialog))
	{
		gtk_widget_show (gn_if_add_dialog);
		gtk_window_present (GTK_WINDOW(gn_if_add_dialog));
	}
	else
	{
		gtk_window_present (GTK_WINDOW(gn_if_add_dialog));
	}
	return;
}

/* CALLBACKS */

static void
cb_gn_config_dsl_changed (GtkToggleButton *togglebutton, gpointer data)
{
	if (TRUE == gtk_toggle_button_get_active (togglebutton))
	{
		if (strlen(active_profile->adsl_interface))
		{
			gn_error (_("There is already an interface configured for DSL connection."));
			gtk_toggle_button_set_active (togglebutton, FALSE);
			return;
		}
		check_dsl = TRUE;
		/* show the dsl config box */
		gtk_widget_show (gn_ndsl_table);
	}
	else
	{
		check_dsl = FALSE;
		gtk_widget_hide (gn_ndsl_table);
	}

	return;
}

static void
cb_gn_nconntype_changed (GtkComboBox *combo, gpointer data)
{
	gchar *sel_if = (gchar*)gtk_entry_get_text (GTK_ENTRY(gn_nif_name_entry));
	if (fwnet_is_wireless_device(sel_if))
		gtk_widget_show (gn_nwireless_table);

	gtk_entry_set_text (GTK_ENTRY(gn_nipaddress_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_nnetmask_entry), "");
	gtk_entry_set_text (GTK_ENTRY(gn_ngateway_entry), "");

	switch (gtk_combo_box_get_active (combo))
	{
		case GN_DHCP: /* DHCP */
			gtk_widget_show (gn_ndhcp_table);
			gtk_combo_box_set_active (GTK_COMBO_BOX(gn_ndhcp_client_combo), 0);
			gtk_widget_hide (gn_nstaticip_table);
			gtk_widget_show (gn_nif_dsl_check);
			break;

		case GN_STATIC: /* Static ip */
			gtk_widget_show (gn_nstaticip_table);
			gtk_widget_hide (gn_ndhcp_table);
			gtk_widget_set_sensitive (gn_nipaddress_entry, TRUE);
			gtk_widget_set_sensitive (gn_nnetmask_entry, TRUE);
			gtk_widget_set_sensitive (gn_ngateway_entry, TRUE);
			gtk_widget_show (gn_nif_dsl_check);
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
	GList			*ifs;
	gchar			opts[50];
	gchar			*name = NULL;

	if (gtk_entry_get_text(GTK_ENTRY(gn_nif_name_entry))==NULL || !strlen(gtk_entry_get_text(GTK_ENTRY(gn_nif_name_entry))))
	{
		gn_error (_("Interface name cannot be blank."));
		return;
	}
	nif = (fwnet_interface_t*)malloc(sizeof(fwnet_interface_t));
	if (!nif)
	{
		gn_error (_("Error allocating memory for new interface."));
		return;
	}
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(gn_nconntype_combo)) == -1)
	{
		gn_error (_("No connection type selected. Please select a connection type first."));
		g_free (nif);
		return;
	}
	/* check if an interface with this name already exists */
	name = (char*)gtk_entry_get_text(GTK_ENTRY(gn_nif_name_entry));
	for (ifs = active_profile->interfaces; ifs != NULL; ifs = g_list_next(ifs))
	{
		fwnet_interface_t *temp = ifs->data;
		if (!strcmp(temp->name,name))
		{
			gn_error (_("An interface with the specified name already exists, please provide a different name."));
			return;
		}
	}

	memset (nif, 0, sizeof(fwnet_interface_t));
	snprintf (nif->name, IF_NAMESIZE, name);
	switch (gtk_combo_box_get_active (GTK_COMBO_BOX(gn_nconntype_combo)))
	{
		case GN_STATIC:
			if (!strlen((char*)gtk_entry_get_text(GTK_ENTRY(gn_nipaddress_entry))) ||
				!strlen((char*)gtk_entry_get_text(GTK_ENTRY(gn_nnetmask_entry))))
				{
					gn_error (_("Required fields cannot be blank."));
					g_free (nif);
					return;
				}
			snprintf (opts, 49, "%s netmask %s",
				(char*)gtk_entry_get_text (GTK_ENTRY(gn_nipaddress_entry)),
				(char*)gtk_entry_get_text (GTK_ENTRY(gn_nnetmask_entry)));
			nif->options = g_list_append (nif->options, opts);
			if ((gtk_entry_get_text(GTK_ENTRY(gn_ngateway_entry)))!=NULL &&
			strlen(gtk_entry_get_text(GTK_ENTRY(gn_ngateway_entry))))
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
				*nif->dhcp_opts = '\0';
			if (gtk_combo_box_get_active (GTK_COMBO_BOX(gn_ndhcp_client_combo)) == GN_DHCPCD)
				snprintf (nif->dhcpclient, PATH_MAX, "dhcpcd");
			else
				snprintf (nif->dhcpclient, PATH_MAX, "dhclient");
			break;
	}
	if (check_dsl == TRUE) /* The interface is to be configured for dsl */
	{
		gchar *username;
		gchar *pass;
		gchar *cpass;

		username = (gchar*)gtk_entry_get_text (GTK_ENTRY(gn_ndsl_username));
		pass = (gchar*)gtk_entry_get_text (GTK_ENTRY(gn_ndsl_password));
		cpass = (gchar*)gtk_entry_get_text (GTK_ENTRY(gn_ndsl_cpassword));

		if (!strlen(username) || !strlen(pass) || !strlen(cpass))
		{
			gn_error (_("Required fields for DSL connection cannot be left blank."));
			g_free (nif);
			return;
		}
		if (strcmp(pass,cpass) != 0)
		{
			gn_error (_("PPPoE passwords do not match. Please re-enter."));
			g_free (nif);
			return;
		}
		/* hopefully, everything is ok now and we should save the profile */
		snprintf (active_profile->adsl_username, PATH_MAX, username);
		snprintf (active_profile->adsl_password, PATH_MAX, pass);
		snprintf (active_profile->adsl_interface, PATH_MAX, nif->name);
	}
	if (fwnet_is_wireless_device(nif->name))
	{
		char *key, *essid, *mode;
		mode = gnetconfig_get_wireless_mode_string (gtk_combo_box_get_active(GTK_COMBO_BOX(gn_nwmode_combo)));
		key = (char*)gtk_entry_get_text (GTK_ENTRY(gn_nkey_entry));
		essid = (char*)gtk_entry_get_text (GTK_ENTRY(gn_nessid_entry));

		if (strlen(key))
			snprintf (nif->key, FWNET_ENCODING_TOKEN_MAX, key);
		else
			*nif->key = '\0';

		if (strlen(essid))
			snprintf (nif->essid, FWNET_ESSID_MAX_SIZE, essid);
		else
			*nif->essid = '\0';

		if (mode != NULL)
		{
			if (strlen(mode))
			{
				snprintf (nif->mode, FWNET_MODE_MAX_SIZE, mode);
				g_free (mode);
			}
		}
	}
	active_profile->interfaces = g_list_append (active_profile->interfaces, nif);
	gnetconfig_save_profile (active_profile);
	gnetconfig_populate_interface_list (active_profile);

	return;
}

static void
cb_gn_new_int_close_clicked (GtkWidget *widget, gpointer data)
{
	gtk_widget_hide (gn_if_add_dialog);
	gnetconfig_reset_nif_dialog ();

	return;
}

static void
cb_gn_new_int_autodetect_clicked (GtkWidget *widget, gpointer data)
{
	gnetconfig_detect_interfaces ();
	return;
}

