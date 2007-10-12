/***************************************************************************
 *  netconfigd.c
 *  Author(s):  Alex Smith <alex@alex-smith.me.uk>
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <libfwutil.h>
#include <dirent.h>
#include <config.h>
#include <syslog.h>
#include <pacman.h>
#include "updnotifierd.h"
#include "updnotifierd-dbus-glue.h"

typedef void* netbuf;
static PM_DB *sync_db = NULL;

G_DEFINE_TYPE(UpdNotifier, updnotifierd, G_TYPE_OBJECT);

void updnotifierd_class_init(UpdNotifierClass *class) {
	// Nothing here
}

void updnotifierd_init(UpdNotifier *server) {
	GError *error = NULL;
	DBusGProxy *driver_proxy;
	int request_ret;
	
	// Init the DBus connection
	server->connection = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	if (server->connection == NULL) {
		g_warning("Unable to connect to dbus: %s", error->message);
		g_error_free(error);
		return;
	}
	
	dbus_g_object_type_install_info(updnotifierd_get_type(), &dbus_glib_updnotifierd_object_info);
	
	// Register DBUS path
	dbus_g_connection_register_g_object(server->connection, "/org/frugalware/UpdNotifier", G_OBJECT(server));

	// Register the service name, the constant here are defined in dbus-glib-bindings.h
	driver_proxy = dbus_g_proxy_new_for_name(server->connection, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy, "org.frugalware.UpdNotifier", 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free(error);
	}
	
	g_object_unref(driver_proxy);
}

// Copied from netconfig.
int handle_network_stop() {
	return(1);
}

int handle_network_start() {
	return 0;
}

static void
_db_callback (char *section, PM_DB *db) {
	g_print ("section: %s\n", section);
	return;
}

static void
_evt_progress (unsigned char event, char *pkgname, int percent, int howmany, int remain) {
	g_print ("im here\n");
}

static void
_evt_evt (unsigned char event, void* data1, void *data2) {
	return;
}

static void
_log_cb (unsigned short level, char *msg) {
	g_print ("%s\n", msg);

	return;
}

static gboolean
_updatenotifierd_init_pacman () {
	/* initialize the pacman-g2 library */
	if (pacman_initialize ("/") == -1)
		return FALSE;

	/* parse the pacman-g2 config */
	pacman_parse_config ("/etc/pacman.conf", _db_callback, "");
	pacman_set_option (PM_OPT_LOGMASK, (long)-1);
	pacman_set_option (PM_OPT_LOGCB, (long)_log_cb);
	
	/* register the main repo */
	/* FIXME: Later add support for custom repos */
	sync_db = pacman_db_register ("frugalware-current");

	if (sync_db == NULL)
		return FALSE;

	return TRUE;
}

static gint
_updnotifierd_update_db () {
	int ret = 0;

	/* update the pacman database */
	ret = pacman_db_update (0, sync_db);
	
	/* something went wrong */
	if (ret == -1) {
		printf ("%s\n", pacman_strerror(pm_errno));
		return ret;
	}
	else if (ret == 0) {
		printf ("Database updated\n");
	}
	else {
		printf ("Database is up to date\n");
	}

	return ret;
}

gboolean updnotifier_update_database (UpdNotifier *obj, gint32 *ret, GError **error) {
	PM_LIST *packages = NULL;

	if (_updnotifierd_update_db() == 0) {
		*ret = 0;
	}

	if (pacman_trans_init(PM_TRANS_TYPE_SYNC, 0, _evt_evt, NULL, _evt_progress) == -1) {
		gchar *errorstr = g_strdup_printf ("Failed to init transaction (%s)\n", pacman_strerror(pm_errno));
		g_print (errorstr);
		g_free (errorstr);
	}
	else {
		if (pacman_trans_sysupgrade() == -1) {
			g_print (pacman_strerror(pm_errno));
		}
		else {
			packages = pacman_trans_getinfo (PM_TRANS_PACKAGES);
			if (packages == NULL) {
				g_print ("No new updates are available\n");
				g_print (pacman_strerror(pm_errno));
			}
			else {
				PM_LIST *i = NULL;
	
				for (i=packages;i!=NULL;i=pacman_list_next(i)) {
					g_print ("%s\n", pacman_list_getdata(i));
				}
			}
			pacman_trans_release ();
		}
	}

	return TRUE;
}

gboolean netconfig_get_current_profile(UpdNotifier *obj, gchar **profile, GError **error) {
	return TRUE;
}

gboolean netconfig_change_profile(UpdNotifier *obj, gchar *profile, gint32 *ret, GError **error) {
	return TRUE;
}

gboolean netconfig_stop_networking(UpdNotifier *obj, gint32 *ret, GError **error) {
	return TRUE;
}

gboolean netconfig_start_networking(UpdNotifier *obj, gint32 *ret, GError **error) {
	return TRUE;
}

gboolean netconfig_get_profiles(UpdNotifier *obj, gchar **profiles, GError **error) {
	return TRUE;
}

void usage() {
	printf("Netconfigd v" VERSION "\n");
	printf(" --help        Display this help text\n");
	printf(" --daemon      Fork into the background\n");
}

int main (int argc, char *argv[]) {
	GMainLoop *main_loop;
	UpdNotifier *server;
	int i = 1;
	int daemonize = 0;
	
	// Parse command line options
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--daemon"))
			daemonize = 1;
		else if (!strcmp(argv[i], "--help")) {
			usage();
			return 0;
		} else {
			usage();
			return 1;
		}
	}
	
	if (getuid() != 0) {
		printf("Must run as root!\n");
		exit(1);
	}
	
	// Daemonize if wanted
	if (daemonize) {
		switch (fork()) {
			case 0:
				break;
			case -1:
				printf("Can't fork: %s\n", strerror(errno));
				exit(1);
				break;
			default:
				exit(0);
		}
		
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);
	}
	
	// Connect to syslog
	openlog("netconfigd", LOG_PID, LOG_DAEMON);
	
	syslog(LOG_INFO, "Netconfigd v" VERSION " started...");
	
	g_type_init();
	
	server = g_object_new(updnotifierd_get_type(), NULL);

	_updatenotifierd_init_pacman ();
	main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(main_loop);
	
	return 0;
}

