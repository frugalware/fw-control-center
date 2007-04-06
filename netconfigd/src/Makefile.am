INCLUDES = @NETCONFIGD_CFLAGS@ \
	-DPREFIX=\"$(prefix)\" \
	-fPIC

bin_PROGRAMS = netconfigd

netconfigd_SOURCES = netconfigd.c

netconfigd_LDADD= @NETCONFIGD_LIBS@ -lfwnetconfig

