ENV_CFLAGS=$CFLAGS

AC_PREREQ(2.59)
AC_INIT([xfce4-gfpm-plugin], 0.2.0, [priyankmg@gmail.com], xfce4-gfpm-plugin)
AC_CONFIG_SRCDIR([src/gfpm-plugin.c])
AC_LANG(C)
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_SYSTEM
AC_PROG_INTLTOOL([0.21])
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE

AC_PROG_CC(GCC)
AC_HEADER_STDC
AC_CHECK_HEADERS([locale.h stdlib.h string.h unistd.h])

ALL_LINGUAS=""
GETTEXT_PACKAGE=xfce4-gfpm-plugin
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [needed for bindtextdomain() and textdomain()])
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)

pkg_modules="gtk+-2.0 >= 2.12.0"
PKG_CHECK_MODULES([XFCE4_GFPM_PLUGIN], [$pkg_modules])

CFLAGS="${CFLAGS}"

dnl ==========================================================================

AC_OUTPUT([
src/Makefile
icons/Makefile
icons/48x48/Makefile
po/Makefile.in
Makefile
])

echo "
xfce4-gfpm-plugin $VERSION configuration summary:

        prefix                   : ${prefix}
        source code location     : ${srcdir}
        compiler                 : ${CC}
        compiler flags           : ${CFLAGS}
"
