#!/bin/sh -xu

[ -f Makefile ] && make distclean
rm -rf autom4te.cache
rm -rf Makefile
rm -rf Makefile.in
rm -rf configure
rm -rf config.*
rm -rf stamp*
rm -rf depcomp
rm -rf install-sh
rm -rf missing
rm -rf src/Makefile
rm -rf src/Makefile.in
rm -rf aclocal.m4
rm -rf ltmain.sh
rm -rf compile
rm -rf libtool
rm -rf mkinstalldirs
rm -rf config.rpath
rm -rf intltool-extract
rm -rf intltool-merge
rm -rf intltool-update
rm -rf po/stamp-it
rm -rf intltool*.in
rm -rf po/Makefile.in.in
rm -rf po/Makefile.in
rm -rf po/Makefile
rm -rf po/POTFILES
