SUBDIRS = src po

sharedir	= $(pkgdatadir)
share_DATA	= glade/gnetconfig.glade

icondir		= $(datadir)/pixmaps
icon_DATA	= data/gnetconfig.png data/network-up.png data/network-down.png \
			  data/gnetconfig-wired.png data/gnetconfig-wireless.png

desktopdir	= $(datadir)/applications
desktop_DATA	= data/gnetconfig.desktop

EXTRA_DIST	= $(share_DATA) \
		  $(icon_DATA)

update-po:
	make -C po/ update-po
