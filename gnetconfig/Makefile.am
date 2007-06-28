SUBDIRS = src po

sharedir   = $(pkgdatadir)
share_DATA = glade/gnetconfig.glade

EXTRA_DIST = $(share_DATA)

update-po:
	make -C po/ update-po

all: update-po
