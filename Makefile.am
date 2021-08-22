## IMPORTANT - If you change this file, run the bootstrap script again

SUBDIRS = desktop X11/Xaw_d

man_MANS = xball.6

bin_PROGRAMS = xball

xball_SOURCES = xball.c color_l.c demo.c file_sel.c sim.c \
	help.c item.c items.c list.c menu.c misc.c miscx.c room.c \
	table.c xball_sys.c scrollbar.c
noinst_HEADERS = act_area.h color_l.h demo.h fallback.h file_sel.h \
	help.h intf.h item.h items.h list.h menu.h misc.h miscx.h \
	names.h patchlevel.h room.h scrollbar.h sim.h table.h \
	xball_sys.h

xball_LDADD = -L/usr/X11R6/lib -lXm -lXmu -lXt -lX11

RUN_DEMOS_HEADER = "d=${datarootdir}/xball"
nodist_bin_SCRIPTS = run_demos

run_demos: run_demos.in
	echo "#!/bin/sh" > run_demos
	echo "d=${datarootdir}/xball" >> run_demos
	cat ${srcdir}/run_demos.in >> run_demos
	chmod +x run_demos

# example demo programs
DEMOS = demo1.xball demo2.xball demo3.xball demo4.xball demo5.xball \
	demo6.xball demo7.xball demo8.xball demo9.xball demo10.xball \
	demo11.xball demo12.xball demo13.xball demo14.xball

demosdir = ${datarootdir}/xball
demos_DATA = ${DEMOS}

EXTRA_DIST = run_demos.in ${DEMOS} ${noinst_HEADERS} \
	act_area.c bootstrap filemenu.c gen_fallback good_bitmaps \
	man.xbm off.xbm on.xbm sgi.xbm stipple.xbm XBall.ad \
	xball-a.xbm xball-b.xbm xball-l.xbm xball.xbm xball-x.xbm \
	xball.6

MOSTLYCLEANFILES = run_demos
DISTCLEANFILES = run_demos

uninstall-hook:
	if test -d ${datarootdir}/xball; then rmdir ${datarootdir}/xball; fi

clean-local:
	-rm -rf run_demos

distclean-local:
	-rm *~
	-rm -rf autom4te.cache
	-rm -rf .deps
