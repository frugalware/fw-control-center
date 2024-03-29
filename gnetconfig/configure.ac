ENV_CFLAGS=$CFLAGS

AC_PREREQ(2.59)
AC_INIT([gnetconfig], 0.1.7, [priyankmg@gmail.com], gnetconfig)
AC_CONFIG_SRCDIR([src/gnetconfig.c])
AC_LANG(C)
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_SYSTEM
AC_PROG_INTLTOOL([0.21])
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([locale.h stdlib.h string.h sys/ioctl.h unistd.h libfwnetconfig.h libfwutil.h])

AC_ARG_ENABLE(debug,
    AC_HELP_STRING([  --enable-debug], [Enable debugging support]), [debug="yes"])

if test "${GCC}" = "yes"
then
	CFLAGS="${CFLAGS} -pipe -Wall"
fi

ALL_LINGUAS=""
GETTEXT_PACKAGE=gnetconfig
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [needed for bindtextdomain() and textdomain()])
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)

dnl Check for debug enabled or not
AC_MSG_CHECKING(for debug request)
if test "x$debug" = "xyes" ; then
    AC_DEFINE(GFPM_DEBUG, 1, [Debug support: enabled])
    CFLAGS="${CFLAGS} -ggdb3 -Wall -DGNETCONFIG_DEBUG"
    debug=yes
    AC_MSG_RESULT(yes)
else
    AC_DEFINE(GFPM_DEBUG, 0, [Debug support: disabled])
    CFLAGS="${CFLAGS} -Wall"
    debug=no
    AC_MSG_RESULT(no)
fi

pkg_modules="gtk+-2.0 >= 2.6.0 \
			 libglade-2.0 >= 2.6.0"
PKG_CHECK_MODULES([GNETCONFIG], [$pkg_modules])

CFLAGS="${CFLAGS}"

dnl ==========================================================================

AC_OUTPUT([
src/Makefile
po/Makefile.in
Makefile
])

echo "
Gnetconfig $VERSION configuration summary:

        prefix                   : ${prefix}
        source code location     : ${srcdir}
        compiler                 : ${CC}
        compiler flags           : ${CFLAGS}
"
