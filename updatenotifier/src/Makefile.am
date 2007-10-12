INCLUDES = @UPDNOTIFIERD_CFLAGS@ \
	-DPREFIX=\"$(prefix)\" \
	-fPIC

sbin_PROGRAMS = updnotifierd

updnotifierd_SOURCES = updnotifierd.c

updnotifierd_LDADD= @UPDNOTIFIERD_LIBS@ -lpacman -lfwutil

BUILT_SOURCES = updnotifierd-dbus-glue.h

updnotifierd-dbus-glue.h: updnotifierd.xml
	@DBUSBINDINGTOOL@ --mode=glib-server --prefix=updnotifierd \
		$(srcdir)/updnotifierd.xml > updnotifierd-dbus-glue.h

EXTRA_DIST = updnotifierd.xml
DISTCLEANFILES = \
	updnotifierd-dbus-glue.h
