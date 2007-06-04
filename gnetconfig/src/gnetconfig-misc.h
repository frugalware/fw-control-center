#ifndef __GNETCONFIG_MISC_H
#define __GNETCONFIG_MISC_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>
#include <libfwnetconfig.h>

typedef enum _gn_conn_type
{
	GN_DHCP = 0,
	GN_STATIC,
	GN_DSL,
	GN_LO
} GN_CONN_TYPE;

typedef enum _gn_wireless_type
{
	GN_AD_HOC = 0,
	GN_MANAGED,
	GN_MASTER,
	GN_MONITOR,
	GN_REPEATER,
	GN_SECONDARY,
	GN_AUTO
} GN_WIRELESS_TYPE;

void gnetconfig_read_hostname (char *);

int gnetconfig_set_hostname (const char *);

fwnet_profile_t * gnetconfig_new_profile (const char *);

void gnetconfig_profile_free (fwnet_profile_t *profile);

int gnetconfig_get_wireless_mode (const char *);

char * gnetconfig_get_wireless_mode_string (int);

char * gnetconfig_get_ifname (char *name, char *p);

#endif

