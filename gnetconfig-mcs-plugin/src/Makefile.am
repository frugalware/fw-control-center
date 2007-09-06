# $Id: Makefile.am 2284 2006-12-29 14:39:00Z pollux $

INCLUDES =								\
	-I$(top_srcdir)							\
	-I$(top_builddir)						\
	-DLOCALEDIR=\""$(localedir)"\"

plugindir = $(XFCE_MCS_MANAGER_PLUGINSDIR)
plugin_LTLIBRARIES =							\
	gnetconfig_plugin.la

gnetconfig_plugin_la_SOURCES =						\
	gnetconfig-plugin.c 

gnetconfig_plugin_la_CFLAGS =						\
	$(LIBXFCEGUI4_CFLAGS)						\
	$(XFCE_MCS_MANAGER_CFLAGS)

gnetconfig_plugin_la_LDFLAGS =						\
	-avoid-version							\
	-module								\
	-no-undefined

gnetconfig_plugin_la_LIBADD =						\
	$(XFCE_MCS_MANAGER_LIBS)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
