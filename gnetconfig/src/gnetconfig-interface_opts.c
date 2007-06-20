/***************************************************************************
 *  gnetconfig-interface_opts.c
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

#include "gnetconfig-interface_opts.h"
#include "gnetconfig-interface.h"
#include "gnetconfig-messages.h"
#include "gnetconfig-misc.h"

extern fwnet_profile_t	*active_profile;
extern GladeXML			*xml;

GtkWidget *gn_preup_treeview;
GtkWidget *gn_predown_treeview;
GtkWidget *gn_postup_treeview;
GtkWidget *gn_postdown_treeview;

static void cb_gn_preup_add (GtkButton *button, gpointer data);
static void cb_gn_preup_remove (GtkButton *button, gpointer data);
static void cb_gn_predown_add (GtkButton *button, gpointer data);
static void cb_gn_predown_remove (GtkButton *button, gpointer data);
static void cb_gn_postup_add (GtkButton *button, gpointer data);
static void cb_gn_postup_remove (GtkButton *button, gpointer data);
static void cb_gn_postdown_add (GtkButton *button, gpointer data);
static void cb_gn_postdown_remove (GtkButton *button, gpointer data);

void
gnetconfig_interface_opts_init (void)
{
	GtkWidget			*widget = NULL;
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

	return;
}

void
gnetconfig_populate_opts (fwnet_interface_t *inte)
{
	GtkTreeModel	*model = NULL;
	GtkListStore	*store = NULL;
	GtkTreeIter		iter1;
	GtkTreeIter		iter2;
	GtkTreeIter		iter3;
	GtkTreeIter		iter4;
	GList			*olist = NULL;
	gchar			*str = NULL;
	GdkPixbuf		*pixbuf = NULL;

	if (!inte)
		return;

	pixbuf = gtk_widget_render_icon (gn_preup_treeview, GTK_STOCK_EXECUTE, GTK_ICON_SIZE_MENU, NULL);
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

/* CALLBACKS */

static void
cb_gn_preup_add (GtkButton *button, gpointer data)
{
	return;
}

static void
cb_gn_preup_remove (GtkButton *button, gpointer data)
{
	return;
}

static void
cb_gn_predown_add (GtkButton *button, gpointer data)
{
	return;
}

static void
cb_gn_predown_remove (GtkButton *button, gpointer data)
{
	return;
}

static void
cb_gn_postup_add (GtkButton *button, gpointer data)
{
	return;
}

static void
cb_gn_postup_remove (GtkButton *button, gpointer data)
{
	return;
}

static void
cb_gn_postdown_add (GtkButton *button, gpointer data)
{
	return;
}

static void
cb_gn_postdown_remove (GtkButton *button, gpointer data)
{
	return;
}

