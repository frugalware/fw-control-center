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
#include <libfwnetconfig.h>
#include <libfwutil.h>
#include <dirent.h>
#include <config.h>
#include <syslog.h>
#include "netconfigd.h"
#include "netconfigd-dbus-glue.h"

G_DEFINE_TYPE(NetConfig, netconfigd, G_TYPE_OBJECT);

void netconfigd_class_init(NetConfigClass *class) {
	// Nothing here
}

void netconfigd_init(NetConfig *server) {
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
	
	dbus_g_object_type_install_info(netconfigd_get_type(), &dbus_glib_netconfigd_object_info);
	
	// Register DBUS path
	dbus_g_connection_register_g_object(server->connection, "/org/frugalware/NetConfig", G_OBJECT(server));

	// Register the service name, the constant here are defined in dbus-glib-bindings.h
	driver_proxy = dbus_g_proxy_new_for_name(server->connection, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy, "org.frugalware.NetConfig", 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free(error);
	}
	
	g_object_unref(driver_proxy);
}

// Copied from netconfig.
int handle_network_stop() {
	char *fn = NULL;
	int i;
	fwnet_profile_t *profile;
	
	if ((fn=fwnet_lastprofile())) {
		if (!fn)
			return(1);
		
		profile = fwnet_parseprofile(fn);
		
		if (profile != NULL) {
			// Unload the old profile
			for (i = 0; i < g_list_length(profile->interfaces); i++)
				fwnet_ifdown((fwnet_interface_t*)g_list_nth_data(profile->interfaces, i), profile);
		}
		
		fwnet_setlastprofile(NULL);
		syslog(LOG_INFO, "Network stopped.");
		return(0);
	}
	
	syslog(LOG_WARNING, "Network stop request failed!");
	return(1);
}

int handle_network_start() {
	char *fn=NULL;
	int ret=0, i;
	fwnet_profile_t *profile;
	
	fn = fwnet_lastprofile();
	if (!fn)
		fn = strdup("default");
	
	// Load the new profile
	profile = fwnet_parseprofile(fn);
	
	if (profile == NULL)
		return(1);
	
	if (!fwnet_lastprofile())
		fwnet_loup();
	
	for (i = 0; i < g_list_length(profile->interfaces); i++)
		ret += fwnet_ifup((fwnet_interface_t*)g_list_nth_data(profile->interfaces, i), profile);
	
	fwnet_setdns(profile);
	fwnet_setlastprofile(fn);
	FWUTIL_FREE(fn);
	
	if (ret != 0)
		syslog(LOG_WARNING, "Network start request failed!");
	else
		syslog(LOG_INFO, "Network started");
	return(ret);
}

gboolean netconfig_get_current_profile(NetConfig *obj, gchar **profile, GError **error) {
	*profile = fwnet_lastprofile();
	return TRUE;
}

gboolean netconfig_change_profile(NetConfig *obj, gchar *profile, gint32 *ret, GError **error) {
	*ret = 0;
	
	*ret += handle_network_stop();
	fwnet_setlastprofile(profile);
	*ret += handle_network_start();
	
	if (!ret) {
		syslog(LOG_INFO, "Changed profile to %s", profile);
	} else {
		syslog(LOG_WARNING, "Changing profile to %s failed!", profile);
	}
	
	return TRUE;
}

gboolean netconfig_stop_networking(NetConfig *obj, gint32 *ret, GError **error) {
	*ret = handle_network_stop();
	return TRUE;
}

gboolean netconfig_start_networking(NetConfig *obj, gint32 *ret, GError **error) {
	*ret = handle_network_start();
	return TRUE;
}

gboolean netconfig_get_profiles(NetConfig *obj, gchar **profiles, GError **error) {
	struct dirent *ent=NULL;
	DIR *dir;
	int i = 0;
	gchar *tmp;
	
	dir = opendir(FWNET_PATH);
	while ((ent = readdir(dir))) {
		if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
			if (i == 0)
				tmp = g_strdup(ent->d_name);
			else
				tmp = g_strdup_printf("%s,%s", tmp, ent->d_name);
			
			i++;
		}
	}
	
	*profiles = g_strdup(tmp);
	tmp = NULL;
	
	return TRUE;
}

void usage() {
	printf("Netconfigd v" VERSION "\n");
	printf(" --help        Display this help text\n");
	printf(" --daemon      Fork into the background\n");
}

int main (int argc, char *argv[]) {
	GMainLoop *main_loop;
	NetConfig *server;
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
	
	server = g_object_new(netconfigd_get_type(), NULL);
	
	main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(main_loop);
	
	return 0;
}

