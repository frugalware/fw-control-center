#ifndef __GNETCONFIG_INTERFACE_H__
#define __GNETCONFIG_INTERFACE_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <libfwnetconfig.h>

#define _(string) gettext (string)

void gnetconfig_interface_init (void);

void gnetconfig_populate_interface_list (fwnet_profile_t *);

int gnetconfig_save_profile (fwnet_profile_t *);

#endif
