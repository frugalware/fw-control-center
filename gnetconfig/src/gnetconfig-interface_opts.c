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
	GtkWidget		*widget = NULL;
	GtkListStore	*store = NULL;

	gn_preup_treeview = glade_xml_get_widget (xml, "preup_treeview");
	gn_predown_treeview = glade_xml_get_widget (xml, "predown_treeview");
	gn_postup_treeview = glade_xml_get_widget (xml, "postup_treeview");
	gn_postdown_treeview = glade_xml_get_widget (xml, "postdown_treeview");

	store = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_preup_treeview), GTK_TREE_MODEL(store));
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_predown_treeview), GTK_TREE_MODEL(store));
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_postup_treeview), GTK_TREE_MODEL(store));
	gtk_tree_view_set_model (GTK_TREE_VIEW(gn_postdown_treeview), GTK_TREE_MODEL(store));

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

