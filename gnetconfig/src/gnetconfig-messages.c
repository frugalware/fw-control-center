/***************************************************************************
 *  gnetconfig-messages.c
 *  Author: Priyank Gosalia <priyankmg@gmail.com>	
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
#include "gnetconfig-messages.h"

extern GtkWidget *gn_main_window;

void
gn_error (const char *error_str, ErrorType type)
{
	if (!strlen(error_str))
		return;
	if (type == ERROR_CONSOLE)
	{
		fprintf (stderr, "\n\033[1;31m%s ==>\033[0;1m %s\033[1;0m\n", _("ERROR"), error_str);
	}
	else if (type == ERROR_GUI)
	{
		GtkWidget *error_dlg;

		error_dlg = gtk_message_dialog_new (GTK_WINDOW(gn_main_window),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_OK,
						"%s",
						error_str);
		gtk_window_set_resizable (GTK_WINDOW(error_dlg), FALSE);
		gtk_dialog_run (GTK_DIALOG(error_dlg));
		gtk_widget_destroy (error_dlg);
	}
	return;
}

void
gn_message (const char *message_str)
{
	GtkWidget *message_dlg;

	message_dlg = gtk_message_dialog_new (GTK_WINDOW(gn_main_window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_INFO,
					GTK_BUTTONS_OK,
					"%s",
					message_str);
	gtk_window_set_resizable (GTK_WINDOW(message_dlg), FALSE);
	gtk_dialog_run (GTK_DIALOG(message_dlg));
	gtk_widget_destroy (message_dlg);

	return;
}

gint
gn_question (const char *message_str)
{
	GtkWidget 	*dialog;
	gint 		ret;

	dialog = gtk_message_dialog_new (GTK_WINDOW(gn_main_window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"%s",
					message_str);
	gtk_window_set_resizable (GTK_WINDOW(dialog), FALSE), 
	ret = gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);

	return ret;
}


