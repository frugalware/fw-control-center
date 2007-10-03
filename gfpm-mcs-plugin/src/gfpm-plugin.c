/*
 * Copyright (c) 2007 Priyank Gosalia <priyankmg@gmail.com>
 * Copyright (c) 2007 Bernhard Walle <bwalle@suse.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; You may only use version 2 of the License,
 * you have no option to use any other version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <gtk/gtk.h>

#include <libxfce4mcs/mcs-manager.h>

#include <libxfce4util/libxfce4util.h>
#include <libxfcegui4/libxfcegui4.h>

#include <xfce-mcs-manager/manager-plugin.h>

#define GFPM_ICON      "gfpm-mcs"
#define GFPM_BIN       "/usr/bin/gfpm"

static gchar *xterm_argv[] = {
    "xterm",
    "-T", "GFpm starter -- DONT'T CLOSE THAT WINDOW",
    "-geometry", "50x1",
    "-e", "su -c " GFPM_BIN,
    NULL 
};

static gchar *gnomesu_argv[] = {
    "gksu",
    GFPM_BIN,
    NULL
};


/* static prototypes */
static void run_dialog (McsPlugin *);

/*
 */
McsPluginInitResult mcs_plugin_init (McsPlugin *plugin)
{
    gchar *where;
    GError *error = NULL;

    xfce_textdomain (GETTEXT_PACKAGE, LOCALEDIR, "UTF-8");

    plugin->plugin_name = g_strdup ("gfpm");
    /* the button label in the xfce-mcs-manager dialog */
    plugin->caption = g_strdup (Q_ ("Package Manager"));
    plugin->run_dialog = run_dialog;
    plugin->icon = xfce_themed_icon_load (GFPM_ICON, 48);
    if (plugin->icon) {
        g_object_set_data_full (G_OBJECT (plugin->icon), "mcs-plugin-icon-name",
                g_strdup (GFPM_ICON), g_free);
    }

    return MCS_PLUGIN_INIT_OK;
}

/*
 */
static void run_dialog(McsPlugin * plugin)
{
    GError *error = NULL;

    /* try gnomesu first */
    if (!g_spawn_async
            (NULL, gnomesu_argv, NULL,
             G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL |
             G_SPAWN_STDERR_TO_DEV_NULL, NULL, NULL, NULL, NULL)) {

        /* and fallback to xterm */
        if (!g_spawn_async
                (NULL, xterm_argv, NULL,
                 G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL |
                 G_SPAWN_STDERR_TO_DEV_NULL, NULL, NULL, NULL, &error)) {
            g_error ("Could not start GFpm: %s", error->message);
            g_error_free (error);
        }
    }
}

/* */
MCS_PLUGIN_CHECK_INIT

/* vim: set sw=4 ts=4 et: */
