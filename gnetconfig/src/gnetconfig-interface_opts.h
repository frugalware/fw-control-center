#ifndef __GNETCONFIG_INTERFACE_OPTS_H__
#define __GNETCONFIG_INTERFACE_OPTS_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>

#define _(string) gettext (string)

void gnetconfig_interface_opts_init (void);

#endif
