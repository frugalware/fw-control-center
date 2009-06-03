/*
 * Copyright (c) 2007-2008 Priyank Gosalia <priyankmg@gmail.com>
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

#define GFPM_BIN       "/usr/bin/gfpm"

static gchar *xterm_argv[] = {
    "xterm",
    "-T", "GFpm starter -- DONT CLOSE THAT WINDOW",
    "-geometry", "50x1",
    "-e", "su -c " GFPM_BIN,
    NULL 
};

static gchar *gnomesu_argv[] = {
    "gksu",
    GFPM_BIN,
    NULL
};

int
main (int argc, char *argv[])
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

