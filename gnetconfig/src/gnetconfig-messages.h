#ifndef __GNETCONFIG_MESSAGES_H__
#define __GNETCONFIG_MESSAGES_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>

#define _(string) gettext (string)

void gn_error (const char *);

void gn_message (const char *);

gint gn_question (const char *);

char * gn_input (const char *, const char *, int *);

#endif
