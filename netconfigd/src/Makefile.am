INCLUDES = @NETCONFIGD_CFLAGS@ \
	-DPREFIX=\"$(prefix)\" \
	-fPIC

sbin_PROGRAMS = netconfigd
sbin_SCRIPTS = netconfig-command

netconfigd_SOURCES = netconfigd.c

netconfigd_LDADD= @NETCONFIGD_LIBS@ -lfwnetconfig -lfwutil

BUILT_SOURCES = netconfigd-dbus-glue.h

netconfigd-dbus-glue.h: netconfigd.xml
	@DBUSBINDINGTOOL@ --mode=glib-server --prefix=netconfigd \
		$(srcdir)/netconfigd.xml > netconfigd-dbus-glue.h

EXTRA_DIST = netconfigd.xml
DISTCLEANFILES = \
	netconfigd-dbus-glue.h
