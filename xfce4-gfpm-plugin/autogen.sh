#!/bin/sh -e

gen_pot()
{
	cd po/
	mv Makevars Makevars.tmp
	cp /usr/bin/intltool-extract ./
	intltool-update --pot --gettext-package=gfpm-mcs-plugin
	rm intltool-extract
	mv Makevars.tmp Makevars
	cd - >/dev/null
}

import_pootle()
{
	po_dir=~/git/translations/po
	if [ -d $po_dir ]; then
		: > po/LINGUAS
		for i in $(/bin/ls $po_dir/fwcontrolcenter)
		do
			[ -e $po_dir/fwcontrolcenter/$i/gfpm-mcs-plugin.po ] || continue
			cp $po_dir/fwcontrolcenter/$i/gfpm-mcs-plugin.po po/$i.po
			if msgfmt -c --statistics -o po/$i.gmo po/$i.po; then
				echo $i >> po/LINGUAS
			else
				echo "WARNING: po/$i.po will break your build!"
			fi
		done
	else
		echo "WARNING: no po files will be used"
	fi
}

cd `dirname $0`

if [ "$1" == "--pot-only" ]; then
	gen_pot
	exit 0
fi

if [ "$1" == "--dist" ]; then
	ver=`grep version_major -m1 configure.in | sed 's/.*, \[\(.*\)].*/\1/'`
	ver=$ver.`grep version_minor -m1 configure.in | sed 's/.*, \[\(.*\)].*/\1/'`
	ver=$ver.`grep version_micro -m1 configure.in | sed 's/.*, \[\(.*\)].*/\1/'`
	git archive --format=tar --prefix=gfpm-mcs-plugin-$ver/ HEAD | tar xf -
	git log --no-merges |git name-rev --tags --stdin > gfpm-mcs-plugin-$ver/ChangeLog
	cd gfpm-mcs-plugin-$ver
	./autogen.sh --git
	rm -rf autom4te.cache
	cd ..
	tar czf gfpm-mcs-plugin-$ver.tar.gz gfpm-mcs-plugin-$ver
	rm -rf gfpm-mcs-plugin-$ver
	gpg --comment "See http://ftp.frugalware.org/pub/README.GPG for info" \
		-ba -u 20F55619 gfpm-mcs-plugin-$ver.tar.gz
	#mv gnetconfig-$ver.tar.gz.asc ../..
	exit 0
fi

cat /usr/share/aclocal/libtool.m4 >> aclocal.m4

intltoolize -c -f --automake
import_pootle
gen_pot
libtoolize -f -c
aclocal --force
autoheader -f
autoconf -f
cp -f $(dirname $(which automake))/../share/automake/mkinstalldirs ./
cp -f $(dirname $(which automake))/../share/gettext/config.rpath ./
NOCONFIGURE=1 xdt-autogen
automake -a -c --gnu --foreign
if [ "$1" == "--git" ]; then
	rm -rf autom4te.cache
fi

