ENV_CFLAGS=$CFLAGS

AC_PREREQ(2.59)
AC_INIT([netconfigd], 0.1, [frugalware-devel@frugalware.org], netconfigd)
AC_CONFIG_SRCDIR([src/netconfigd.c])
AC_LANG(C)
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/ioctl.h unistd.h libfwnetconfig.h libfwutil.h])

AC_PATH_PROG(DBUSBINDINGTOOL, dbus-binding-tool, no)

if test "x$DBUSBINDINGTOOL" = "xno"; then
	AC_MSG_ERROR([dbus-binding-tool not found in your PATH. Please correct this])
fi

AC_SUBST(DBUS_BINDING_TOOL)

PKG_CHECK_MODULES([NETCONFIGD], [dbus-glib-1 >= 0.73])

AC_ARG_WITH(dbus-sys, [  --with-dbus-sys=<dir>   where D-BUS system.d directory is])

if ! test -z "$with_dbus_sys" ; then
	DBUS_SYS_DIR="$with_dbus_sys"
else
	DBUS_SYS_DIR="$sysconfdir/dbus-1/system.d"
fi

AC_SUBST(DBUS_SYS_DIR)

CFLAGS="${CFLAGS}"

dnl ==========================================================================

AC_OUTPUT([
src/Makefile
Makefile
])

echo "
Netconfigd $VERSION configuration summary:

        prefix                   : ${prefix}
        source code location     : ${srcdir}
        compiler                 : ${CC}
        compiler flags           : ${CFLAGS}
"
