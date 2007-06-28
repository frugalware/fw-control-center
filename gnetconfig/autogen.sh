#!/bin/sh -e

generate_pot() {
	cd po/
	mv Makevars Makevars.tmp
	cp /usr/bin/intltool-extract ./
	intltool-update --pot --gettext-package=gnetconfig
	rm intltool-extract
	mv Makevars.tmp Makevars
	cd - > /dev/null
}

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

cat /usr/share/aclocal/libtool.m4 >> aclocal.m4

intltoolize -c -f --automake
generate_pot
libtoolize -f -c
aclocal --force
autoheader -f
autoconf -f
cp -f $(dirname $(which automake))/../share/automake/mkinstalldirs ./
cp -f $(dirname $(which automake))/../share/gettext/config.rpath ./
automake -a -c --gnu --foreign

if [ "$1" == "--darcs" ]; then
	rm -rf autom4te.cache
fi

