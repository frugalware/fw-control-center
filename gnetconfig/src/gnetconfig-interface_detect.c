/***************************************************************************
 *  gnetconfig-interface_detect.c
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

#include "gnetconfig-interface_detect.h"
#include "gnetconfig-messages.h"
#include "gnetconfig-misc.h"

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

static void cb_gn_if_detect_ok_clicked (GtkWidget *widget, gpointer data);
static void cb_gn_if_detect_cancel_clicked (GtkWidget *widget, gpointer data);

extern fwnet_profile_t 	*active_profile;
extern GtkWidget	*dialog;
extern GtkWidget 	*gn_nif_name_entry;

GtkWidget	*if_detect_dlg;
GtkWidget	*if_detect_treeview;

void
gnetconfig_if_detect_dlg_init (void)
{
	GtkWidget 		*button_save;
	GtkWidget 		*button_cancel;
	GtkButtonBox		*button_box;
	GtkWidget  		*vbox;
	GtkWidget		*label;
	GtkListStore 		*store;
	GtkCellRenderer	 	*renderer;
	GtkTreeViewColumn	*column;

	if_detect_dlg = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width (GTK_CONTAINER(if_detect_dlg), 8);
	gtk_window_set_resizable (GTK_WINDOW(if_detect_dlg), FALSE);
	gtk_window_set_transient_for (GTK_WINDOW(if_detect_dlg), GTK_WINDOW(dialog));
	vbox = gtk_vbox_new (FALSE, 4);
	if_detect_treeview = gtk_tree_view_new ();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(if_detect_treeview), FALSE);
	label = gtk_label_new (_("The following interfaces were detected"));
	gtk_box_pack_start (GTK_BOX(vbox), label, FALSE, TRUE, 2);
	gtk_box_pack_start (GTK_BOX(vbox), GTK_WIDGET(if_detect_treeview), TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER(if_detect_dlg), GTK_WIDGET(vbox));
	store = gtk_list_store_new (4, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	gtk_tree_view_set_model (GTK_TREE_VIEW(if_detect_treeview), GTK_TREE_MODEL(store));
	
	renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes ("S",
							renderer,
							"pixbuf", 0,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(if_detect_treeview), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Interface",
							renderer,
							"text", 1,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_column_set_min_width (column, 80);
	g_object_set (G_OBJECT(column), "expand", TRUE, "spacing", 4, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(if_detect_treeview), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Status",
							renderer,
							"text", 2,
							NULL);
	gtk_tree_view_column_set_min_width (column, 60);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(if_detect_treeview), column);

	button_save = gtk_button_new_from_stock (GTK_STOCK_OK);
	button_cancel = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	button_box = gtk_hbutton_box_new ();
	gtk_container_add (GTK_CONTAINER(button_box), button_save);
	gtk_container_add (GTK_CONTAINER(button_box), button_cancel);
	gtk_box_pack_start (GTK_BOX(vbox), GTK_WIDGET(button_box), FALSE, FALSE, 2);

	g_signal_connect (G_OBJECT(button_save),
			"clicked",
			G_CALLBACK(cb_gn_if_detect_ok_clicked),
			NULL);
	g_signal_connect (G_OBJECT(button_cancel),
			"clicked",
			G_CALLBACK(cb_gn_if_detect_cancel_clicked),
			NULL);

	return;
}

int
gnetconfig_detect_interfaces (void)
{
	FILE			*fh = NULL;
	char 			buf[512];
	char 			name[512];
	char 			*s;
	gint			n_if, i;
	GtkTreeModel 		*model;
	GtkListStore 		*store;
	GtkTreeIter		iter;
	GdkPixbuf		*yes_pixbuf;
	GdkPixbuf		*no_pixbuf;
	fwnet_interface_t	*interface;

	fh = fopen ("/proc/net/dev", "r");
	if (fh == NULL)
	{	
		gn_error ("Error opening /proc/net/dev", ERROR_CONSOLE);
		return 1;
	}
	n_if = g_list_length (active_profile->interfaces);
	yes_pixbuf = gtk_widget_render_icon (GTK_WIDGET(if_detect_treeview),
					GTK_STOCK_YES,
					GTK_ICON_SIZE_MENU, NULL);
	no_pixbuf = gtk_widget_render_icon (GTK_WIDGET(if_detect_treeview),
					GTK_STOCK_NO,
					GTK_ICON_SIZE_MENU, NULL);
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(if_detect_treeview));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);
	fgets (buf, sizeof(buf), fh);
	fgets (buf, sizeof(buf), fh);

	while ( fgets (buf, sizeof(buf), fh) )
	{
		gboolean configured = FALSE;
		s = gnetconfig_get_ifname (name, buf);
		if (!strcmp(name, "lo")) /* skip lo */
			continue;

		/* check if the interface is already present in our profile */
		for (i=0;i<n_if;i++)
		{
			interface = g_list_nth_data (active_profile->interfaces, i);
			if (!strcmp(name, interface->name))
			{
				configured = TRUE;
				break;
			}	
		}

		gtk_list_store_append (store, &iter);
		if (configured)
		{
			gtk_list_store_set (store, &iter,
					0, yes_pixbuf,
					1, name,
					2, _("Configured"),
					3, TRUE, -1);
		}
		else
		{
			gtk_list_store_set (store, &iter,
					0, no_pixbuf,
					1, name,
					2, _("Not Configured"),
					3, FALSE, -1);
		}
	}
	gtk_window_set_position (GTK_WINDOW(if_detect_dlg), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_widget_show_all (if_detect_dlg);
	return 0;
}

/* CALLBACKS */

static void
cb_gn_if_detect_ok_clicked (GtkWidget *widget, gpointer data)
{
	gchar			*ifname = NULL;
	GtkTreeModel		*model = NULL;
	GtkTreeSelection	*selection = NULL;
	GtkTreeIter		iter;
	gboolean		status = FALSE;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(if_detect_treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(if_detect_treeview));
	if ( FALSE == gtk_tree_selection_get_selected (selection, &model, &iter) )
		return;
	gtk_tree_model_get (model, &iter, 1, &ifname, 3, &status, -1);
	if (status == TRUE)
	{
		gn_error ("The selected interface is already configured. Please select another interface", ERROR_GUI);
		return;
	}
	else
	{
		gtk_entry_set_text (GTK_ENTRY(gn_nif_name_entry), ifname);
		gtk_widget_hide (if_detect_dlg);
		if (fwnet_is_wireless_device(ifname))
			gn_message (_("Gnetconfig has detected that the selected interface is a wireless device."));
	}	
	
	return;
}

static void
cb_gn_if_detect_cancel_clicked (GtkWidget *widget, gpointer data)
{
	gtk_widget_hide (if_detect_dlg);

	return;
}

