#ifndef __GNETCONFIG_MISC_H
#define __GNETCONFIG_MISC_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>
#include <libfwnetconfig.h>

void gnetconfig_read_hostname (char *);

int gnetconfig_set_hostname (const char *);

fwnet_profile_t * gnetconfig_new_profile (const char *);

int gnetconfig_save_profile (fwnet_profile_t *);

#endif

