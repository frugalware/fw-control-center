#ifndef __GNETCONFIG_INTERFACE_DETECT_H__
#define __GNETCONFIG_INTERFACE_DETECT_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>

#define _(string) gettext (string)

void gnetconfig_if_detect_dlg_init (void);

int gnetconfig_detect_interfaces (void);

#endif
