# Makefile.am - build XBall freedesktop files
#
# Copyright 2021, Jose Da Silva
#
# Permission to use, copy, modify, and distribute this software
# and its documentation for any purpose and without fee is granted
# provided that the above copyright notice appears in all copies.
# It is provided "as is" without express or implied warranty.

# Where .desktop files go.
desktopdir = ${datarootdir}/applications
desktop_DATA = xball.desktop

# Where appdata and metainfo xml files go
appdatadir = ${datarootdir}/appdata
appdata_DATA = xball.appdata.xml
metainfodir = ${datarootdir}/metainfo
metainfo_DATA = xball.metainfo.xml

# Where MIME data goes (Support for .xball files, for instance).
mimedir = ${datarootdir}/mime
mimepackagesdir = ${datarootdir}/mime/packages
mimepackages_DATA = xball.xml

#update-mime-database ${mimedir}

ICONSDIR = ${datarootdir}/icons
HICOLORDIR = ${datarootdir}/icons/hicolor
hicolor32x32dir = $(HICOLORDIR)/32x32/apps
hicolor64x64dir = $(HICOLORDIR)/64x64/apps
hicolor128x128dir = $(HICOLORDIR)/128x128/apps
hicolor256x256dir = $(HICOLORDIR)/256x256/apps

hicolor32x32_DATA = 32x32/xball.png
hicolor64x64_DATA = 64x64/xball.png
hicolor128x128_DATA = 128x128/xball.png
hicolor256x256_DATA = 256x256/xball.png

deskpixsdir = ${datarootdir}/pixmaps
deskpixs_DATA = 32x32/xball.xpm 32x32/xball.png

EXTRA_DIST = xball.desktop xball.appdata.xml xball.metainfo.xml xball.xml \
	32x32/xball.xpm	32x32/xball.png	64x64/xball.png	128x128/xball.png \
	256x256/xball.png
