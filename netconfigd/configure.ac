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
AC_CHECK_HEADERS([stdlib.h string.h sys/ioctl.h unistd.h libfwnetconfig.h])

AC_MSG_CHECKING(whether I'm developed by somebody paranoid about their code being warning free)
CFLAGS="$CFLAGS -Werror -Wextra -pedantic -Wall -Wno-unused-parameter"
AC_MSG_RESULT(yes)

PKG_CHECK_MODULES([NETCONFIGD], [dbus-glib-1 >= 0.73])

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
