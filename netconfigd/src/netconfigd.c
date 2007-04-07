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

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <libfwnetconfig.h>
#include <dirent.h>
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

gboolean netconfig_get_current_profile(NetConfig *obj, gchar **profile, GError **error) {
	*profile = fwnet_lastprofile();
	printf("[DEBUG] Handled current profile request.\n");
	return TRUE;
}

gboolean netconfig_change_profile(NetConfig *obj, gchar *profile, gint32 *ret, GError **error) {
	ret = 0;
	printf("Changing profile to %s...\n", profile);
	return TRUE;
}

gboolean netconfig_stop_networking(NetConfig *obj, gint32 *ret, GError **error) {
	ret = 0;
	printf("Stopping networking...\n");
	return TRUE;
}

gboolean netconfig_start_networking(NetConfig *obj, gint32 *ret, GError **error) {
	ret = 0;
	printf("Starting networking...\n");
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
	
	printf("[DEBUG] Handled profile list request.\n");
	
	return TRUE;
}

int main (int argc, char *argv[]) {
	GMainLoop *main_loop;
	NetConfig *server;
	
	g_type_init();
	
	server = g_object_new(netconfigd_get_type(), NULL);
	
	main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(main_loop);
	
	return 0;
}

