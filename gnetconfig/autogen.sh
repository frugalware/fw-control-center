#!/bin/sh -e

cat /usr/share/aclocal/libtool.m4 >> aclocal.m4
libtoolize -f -c
aclocal --force
autoheader -f
autoconf -f
cp -f $(dirname $(which automake))/../share/automake/mkinstalldirs ./
cp -f $(dirname $(which automake))/../share/gettext/config.rpath ./
automake -a -c --gnu --foreign

if [ "$1" == "--dist" ]; then
        ver=`grep AC_INIT configure.ac|sed 's/.*, \([0-9\.]*\), .*/\1/'`
        darcs changes >../_darcs/pristine/ChangeLog
        darcs dist -d gnetconfig-$ver
        rm ../_darcs/pristine/ChangeLog
        gpg --comment "See http://ftp.frugalware.org/pub/README.GPG for info" \
                -ba -u 20F55619 ../gnetconfig-$ver.tar.gz
        #mv ../gnetconfig-$ver.tar.gz.asc ../..
        exit 0
fi
