/*
 *  gnetconfig-interface_opts.c for gnetconfig
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

#include "gnetconfig-interface_opts.h"
#include "gnetconfig-interface.h"
#include "gnetconfig-messages.h"
#include "gnetconfig-misc.h"

extern fwnet_profile_t	*active_profile;
extern GladeXML		*xml;
extern GtkWidget	*gn_main_window;
extern GtkWidget	*gn_iflabel;

GtkWidget *gn_preup_treeview;
GtkWidget *gn_predown_treeview;
GtkWidget *gn_postup_treeview;
GtkWidget *gn_postdown_treeview;

static fwnet_interface_t *gnetconfig_get_active_interface (void);

static void cb_gn_preup_add (GtkButton *button, gpointer data);
static void cb_gn_preup_remove (GtkButton *button, gpointer data);
static void cb_gn_predown_add (GtkButton *button, gpointer data);
static void cb_gn_predown_remove (GtkButton *button, gpointer data);
static void cb_gn_postup_add (GtkButton *button, gpointer data);
static void cb_gn_postup_remove (GtkButton *button, gpointer data);
static void cb_gn_postdown_add (GtkButton *button, gpointer data);
static void cb_gn_postdown_remove (GtkButton *button, gpointer data);

static void cb_gn_preup_treeview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data);
static void cb_gn_predown_treeview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data);
static void cb_gn_postup_treeview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data);
static void cb_gn_postdown_treeview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data);

void
gnetconfig_interface_opts_init (void)
{
	GtkWidget		*widget = NULL;
	GtkListStore		*store_1 = NULL;
	GtkListStore		*store_2 = NULL;
	GtkListStore		*store_3 = NULL;
	GtkListStore		*store_4 = NULL;
	GtkCellRenderer		*t_renderer = NULL;
	GtkCellRenderer		*p_renderer = NULL;
	GtkTreeViewColumn	*column = NULL;

	gn_preup_treeview = glade_xml_get_widget (xml, "preup_treeview");
	gn_predown_treeview = glade_xml_get_widget (xml, "predown_treeview");
	gn_postup_treeview = glade_xml_get_widget (xml, "postup_treeview");
	gn_postdown_treeview = glade_xml_get_widget (xml, "postdown_treeview");

	store_1 = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	store_2 = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	store_3 = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	store_4 = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	
	t_renderer = gtk_cell_renderer_text_new ();
	p_renderer = gtk_cell_renderer_pixbuf_new ();

	column = gtk_tree_view_column_new_with_attributes ("Icon", p_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_preup_treeview), column);
	column = gtk_tree_view_column_new_with_attributes ("Icon", p_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_predown_treeview), column);
	column = gtk_tree_view_column_new_with_attributes ("Icon", p_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_postup_treeview), column);
	column = gtk_tree_view_column_new_with_attributes ("Icon", p_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_postdown_treeview), column);

	column = gtk_tree_view_column_new_with_attributes ("Command", t_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_preup_treeview), column);
	column = gtk_tree_view_column_new_with_attributes ("Command", t_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_predown_treeview), column);
	column = gtk_tree_view_column_new_with_attributes ("Command", t_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_postup_treeview), column);
	column = gtk_tree_view_column_new_with_attributes ("Command", t_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(gn_postdown_treeview), column);

	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_preup_treeview), GTK_TREE_MODEL(store_1));
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_predown_treeview), GTK_TREE_MODEL(store_2));
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_postup_treeview), GTK_TREE_MODEL(store_3));
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_postdown_treeview), GTK_TREE_MODEL(store_4));

	/* setup buttons */
	widget = glade_xml_get_widget (xml, "preup_add");
	g_signal_connect (G_OBJECT(widget), "clicked", G_CALLBACK(cb_gn_preup_add), NULL);
	widget = glade_xml_get_widget (xml, "preup_remove");
	g_signal_connect (G_OBJECT(widget), "clicked", G_CALLBACK(cb_gn_preup_remove), NULL);

	widget = glade_xml_get_widget (xml, "predown_add");
	g_signal_connect (G_OBJECT(widget), "clicked", G_CALLBACK(cb_gn_predown_add), NULL);
	widget = glade_xml_get_widget (xml, "predown_remove");
	g_signal_connect (G_OBJECT(widget), "clicked", G_CALLBACK(cb_gn_predown_remove), NULL);

	widget = glade_xml_get_widget (xml, "postup_add");
	g_signal_connect (G_OBJECT(widget), "clicked", G_CALLBACK(cb_gn_postup_add), NULL);
	widget = glade_xml_get_widget (xml, "postup_remove");
	g_signal_connect (G_OBJECT(widget), "clicked", G_CALLBACK(cb_gn_postup_remove), NULL);

	widget = glade_xml_get_widget (xml, "postdown_add");
	g_signal_connect (G_OBJECT(widget), "clicked", G_CALLBACK(cb_gn_postdown_add), NULL);
	widget = glade_xml_get_widget (xml, "postdown_remove");
	g_signal_connect (G_OBJECT(widget), "clicked", G_CALLBACK(cb_gn_postdown_remove), NULL);

	/* Keypress events */
	g_signal_connect (G_OBJECT(gn_preup_treeview),
			"key_release_event",
			G_CALLBACK(cb_gn_preup_treeview_keypress),
			NULL);
	g_signal_connect (G_OBJECT(gn_predown_treeview),
			"key_release_event",
			G_CALLBACK(cb_gn_predown_treeview_keypress),
			NULL);
	g_signal_connect (G_OBJECT(gn_postup_treeview),
			"key_release_event",
			G_CALLBACK(cb_gn_postup_treeview_keypress),
			NULL);
	g_signal_connect (G_OBJECT(gn_postdown_treeview),
			"key_release_event",
			G_CALLBACK(cb_gn_postdown_treeview_keypress),
			NULL);

	return;
}

void
gnetconfig_populate_opts (fwnet_interface_t *inte)
{
	GtkTreeModel	*model = NULL;
	GtkListStore	*store = NULL;
	GtkTreeIter	iter1;
	GtkTreeIter	iter2;
	GtkTreeIter	iter3;
	GtkTreeIter	iter4;
	GList		*olist = NULL;
	gchar		*str = NULL;
	GdkPixbuf	*pixbuf = NULL;

	if (!inte)
		return;

	pixbuf = gtk_widget_render_icon (gn_main_window, GTK_STOCK_EXECUTE, GTK_ICON_SIZE_MENU, NULL);
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_preup_treeview));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);
	olist = inte->pre_ups;
	while (olist != NULL)
	{
		gtk_list_store_append (store, &iter1);
		str = g_strdup_printf (olist->data);
		gtk_list_store_set (store, &iter1, 0, pixbuf, 1, str, -1);
		olist = g_list_next (olist);
		g_free (str);
	}

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_predown_treeview));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);
	olist = inte->pre_downs;
	while (olist != NULL)
	{
		gtk_list_store_append (store, &iter2);
		str = g_strdup_printf (olist->data);
		gtk_list_store_set (store, &iter2, 0, pixbuf, 1, str, -1);
		olist = g_list_next (olist);
		g_free (str);
	}
	
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_postup_treeview));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);
	olist = inte->post_ups;
	while (olist != NULL)
	{
		gtk_list_store_append (store, &iter3);
		str = g_strdup_printf (olist->data);
		gtk_list_store_set (store, &iter3, 0, pixbuf, 1, str, -1);
		olist = g_list_next (olist);
		g_free (str);
	}
	
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_postdown_treeview));
	store = GTK_LIST_STORE (model);
	gtk_list_store_clear (store);
	olist = inte->post_downs;
	while (olist != NULL)
	{
		gtk_list_store_append (store, &iter4);
		str = g_strdup_printf (olist->data);
		gtk_list_store_set (store, &iter4, 0, pixbuf, 1, str, -1);
		olist = g_list_next (olist);
		g_free (str);
	}

	g_object_unref (pixbuf);
	return;
}

static fwnet_interface_t *
gnetconfig_get_active_interface (void)
{
	fwnet_interface_t	*inf = NULL;
	GList			*iflist = NULL;
	gchar			*name = NULL;

	name = (gchar*)gtk_label_get_text (GTK_LABEL(gn_iflabel));
	for (iflist = active_profile->interfaces; iflist != NULL; iflist = g_list_next(iflist))
	{
		inf = iflist->data;
		if (strcmp(name, inf->name) == 0)
			break;
	}

	return inf;
}

/* CALLBACKS */

static void
cb_gn_preup_add (GtkButton *button, gpointer data)
{
	fwnet_interface_t	*ift;
	char			*command;
	gint			res;

	ift = gnetconfig_get_active_interface ();
	up:command = gn_input ("Gnetconfig", _("Enter the name of the command:"), &res);
	if (res == GTK_RESPONSE_ACCEPT)
	{
		if (!command || !strlen(command))
		{
			gn_error (_("Required field cannot be blank. Please provide a valid command name."));
			goto up;
		}
		ift->pre_ups = g_list_append (ift->pre_ups, (gpointer)strdup(command));
		gnetconfig_save_profile (active_profile);
		gnetconfig_populate_opts (ift);
		g_free (command);
	}

	return;
}

static void
cb_gn_preup_remove (GtkButton *button, gpointer data)
{
	GtkTreeModel		*model = NULL;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection = NULL;
	GList			*list = NULL;
	char			*command = NULL;
	fwnet_interface_t	*inf = NULL;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_preup_treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(gn_preup_treeview));
	if (FALSE == gtk_tree_selection_get_selected(selection, &model, &iter))
		return;

	gtk_tree_model_get (model, &iter, 1, &command, -1);
	inf = gnetconfig_get_active_interface();
	for (list = inf->pre_ups; list != NULL; list = list->next)
	{
		if (list->data && !strcmp((char*)list->data, command))
		{
			inf->pre_ups = g_list_delete_link (inf->pre_ups, list);
			break;
		}
	}
	
	gnetconfig_save_profile (active_profile);
	gnetconfig_populate_opts (gnetconfig_get_active_interface());
	
	return;
}

static void
cb_gn_predown_add (GtkButton *button, gpointer data)
{
	fwnet_interface_t	*ift;
	char			*command;
	gint			res;

	ift	= gnetconfig_get_active_interface ();
	up:command = gn_input ("Gnetconfig", _("Enter the name of the command:"), &res);
	if (res == GTK_RESPONSE_ACCEPT)
	{
		if (!command || !strlen(command))
		{	
			gn_error (_("Required field cannot be blank. Please provide a valid command name."));
			goto up;
		}
		ift->pre_downs = g_list_append (ift->pre_downs, (gpointer)strdup(command));
		gnetconfig_save_profile (active_profile);
		gnetconfig_populate_opts (ift);
		g_free (command);
	}

	return;
}

static void
cb_gn_predown_remove (GtkButton *button, gpointer data)
{
	GtkTreeModel		*model = NULL;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection = NULL;
	GList			*list = NULL;
	char			*command = NULL;
	fwnet_interface_t	*inf = NULL;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_predown_treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(gn_predown_treeview));
	if (FALSE == gtk_tree_selection_get_selected(selection, &model, &iter))
		return;

	gtk_tree_model_get (model, &iter, 1, &command, -1);
	inf = gnetconfig_get_active_interface();
	for (list = inf->pre_downs; list != NULL; list = list->next)
	{
		if (list->data && !strcmp((char*)list->data, command))
		{
			inf->pre_downs = g_list_delete_link (inf->pre_downs, list);
			break;
		}
	}
	
	gnetconfig_save_profile (active_profile);
	gnetconfig_populate_opts (gnetconfig_get_active_interface());
	
	return;
}

static void
cb_gn_postup_add (GtkButton *button, gpointer data)
{
	fwnet_interface_t	*ift;
	char			*command;
	gint			res;

	ift	= gnetconfig_get_active_interface ();
	up:command = gn_input ("Gnetconfig", _("Enter the name of the command:"), &res);
	if (res == GTK_RESPONSE_ACCEPT)
	{
		if (!command || !strlen(command))
		{
			gn_error (_("Required field cannot be blank. Please provide a valid command name."));
			goto up;
		}
		ift->post_ups = g_list_append (ift->post_ups, (gpointer)strdup(command));
		gnetconfig_save_profile (active_profile);
		gnetconfig_populate_opts (ift);
		g_free (command);
	}

	return;
}

static void
cb_gn_postup_remove (GtkButton *button, gpointer data)
{
	GtkTreeModel		*model = NULL;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection = NULL;
	GList			*list = NULL;
	char			*command = NULL;
	fwnet_interface_t	*inf = NULL;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_postup_treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(gn_postup_treeview));
	if (FALSE == gtk_tree_selection_get_selected(selection, &model, &iter))
		return;

	gtk_tree_model_get (model, &iter, 1, &command, -1);
	inf = gnetconfig_get_active_interface();
	for (list = inf->post_ups; list != NULL; list = list->next)
	{
		if (list->data && !strcmp((char*)list->data, command))
		{
			inf->post_ups = g_list_delete_link (inf->post_ups, list);
			break;
		}
	}
	
	gnetconfig_save_profile (active_profile);
	gnetconfig_populate_opts (gnetconfig_get_active_interface());
	
	return;
}

static void
cb_gn_postdown_add (GtkButton *button, gpointer data)
{
	fwnet_interface_t	*ift;
	char			*command;
	gint			res;

	ift	= gnetconfig_get_active_interface ();
	up:command = gn_input ("Gnetconfig", _("Enter the name of the command:"), &res);
	if (res == GTK_RESPONSE_ACCEPT)
	{
		if (!command || !strlen(command))
		{
			gn_error (_("Required field cannot be blank. Please provide a valid command name."));
			goto up;
		}
		ift->post_downs = g_list_append (ift->post_downs, (gpointer)strdup(command));
		gnetconfig_save_profile (active_profile);
		gnetconfig_populate_opts (ift);
		g_free (command);
	}

	return;
}

static void
cb_gn_postdown_remove (GtkButton *button, gpointer data)
{
	GtkTreeModel		*model = NULL;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection = NULL;
	GList			*list = NULL;
	char			*command = NULL;
	fwnet_interface_t	*inf = NULL;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(gn_postdown_treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(gn_postdown_treeview));
	if (FALSE == gtk_tree_selection_get_selected(selection, &model, &iter))
		return;

	gtk_tree_model_get (model, &iter, 1, &command, -1);
	inf = gnetconfig_get_active_interface();
	for (list = inf->post_downs; list != NULL; list = list->next)
	{
		if (list->data && !strcmp((char*)list->data, command))
		{
			inf->post_downs = g_list_delete_link (inf->post_downs, list);
			break;
		}
	}
	
	gnetconfig_save_profile (active_profile);
	gnetconfig_populate_opts (gnetconfig_get_active_interface());
	
	return;
}

static void
cb_gn_preup_treeview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval != GDK_Delete)
		return;

	cb_gn_preup_remove (NULL, NULL);
	return;
}

static void
cb_gn_predown_treeview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval != GDK_Delete)
		return;

	cb_gn_predown_remove (NULL, NULL);
	return;
}

static void
cb_gn_postup_treeview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval != GDK_Delete)
		return;

	cb_gn_postup_remove (NULL, NULL);
	return;

}

static void
cb_gn_postdown_treeview_keypress (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval != GDK_Delete)
		return;

	cb_gn_postdown_remove (NULL, NULL);
	return;
}


