INCLUDES=@GNETCONFIG_CFLAGS@ \
	-DPREFIX=\"$(prefix)\" \
	-fPIC

bin_PROGRAMS=gnetconfig

gnetconfig_SOURCES=	 gnetconfig.c \
					 gnetconfig-misc.c \
					 gnetconfig-messages.c \
					 gnetconfig-about.c \
					 gnetconfig-new_interface.c \
					 gnetconfig-interface.c

gnetconfig_LDADD= @GNETCONFIG_LIBS@ -lfwnetconfig -lfwutil

