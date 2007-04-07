/***************************************************************************
 *  netconfigd.h
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

#ifndef _NETCONFIGD_H
#define _NETCONFIGD_H

typedef struct
{
	GObject parent;
	DBusGConnection *connection;
} NetConfig;

typedef struct
{
	GObjectClass parent_class;
} NetConfigClass;

static void netconfigd_init(NetConfig *server);
static void netconfigd_class_init(NetConfigClass *class);

gboolean netconfig_get_current_profile(NetConfig *obj, gchar **profile, GError **error);
gboolean netconfig_change_profile(NetConfig *obj, gchar *profile, gint32 *ret, GError **error);
gboolean netconfig_stop_networking(NetConfig *obj, gint32 *ret, GError **error);
gboolean netconfig_start_networking(NetConfig *obj, gint32 *ret, GError **error);
gboolean netconfig_get_profiles(NetConfig *obj, gchar **profiles, GError **error);

#endif
