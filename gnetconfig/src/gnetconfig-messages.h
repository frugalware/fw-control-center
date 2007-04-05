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

typedef enum _errortype
{
	ERROR_CONSOLE = 1, 	/* Display error on the console */
	ERROR_GUI			/* Display error on gui */
} ErrorType;

void gn_error (const char *, ErrorType);

void gn_message (const char *);

#endif
