## IMPORTANT - If you change this file, run the bootstrap script again

SUBDIRS = . desktop X11/Xaw_d tests

man_MANS = xball.6

fallback.h:
	$(srcdir)/gen_fallback $(srcdir)/XBall.ad > fallback.h

rgbtxt_loc.h:
	echo '/* NOTE: This file is generated! */' > rgbtxt_loc.h
	echo '#define RGB1_TXT "$(LOC_RGB1_TXT)"' >> rgbtxt_loc.h
	echo '#define RGB2_TXT "$(LOC_RGB2_TXT)"' >> rgbtxt_loc.h

bin_PROGRAMS = xball
xball_SOURCES = xball.c color_l.c demo.c file_sel.c sim.c \
	help.c item.c items.c list.c menu.c misc.c miscx.c room.c \
	table.c xball_sys.c scrollbar.c
noinst_HEADERS = act_area.h color_l.h demo.h fallback.h file_sel.h \
	help.h intf.h item.h items.h list.h menu.h misc.h miscx.h \
	names.h patchlevel.h room.h scrollbar.h sim.h table.h \
	xball_sys.h
xball.$(OBJEXT): $(noinst_HEADERS) rgbtxt_loc.h

xball_LDADD = -L/usr/X11R6/lib -lXm -lXmu -lXt -lX11

RUN_DEMOS_HEADER = "d=${datarootdir}/xball"
nodist_bin_SCRIPTS = run_demos

run_demos: run_demos.in
	echo "#!/bin/sh" > run_demos
	echo "d=${datarootdir}/xball" >> run_demos
	cat ${srcdir}/run_demos.in >> run_demos
	chmod +x run_demos

do_subst = sed -e 's,[@]xball[@],$(datarootdir)\/xball,' $< > $@

demo1.xball : demo1.xball.in
	$(do_subst) < $(srcdir)/demo1.xball.in > demo1.xball

demo2.xball : demo2.xball.in
	$(do_subst) < $(srcdir)/demo2.xball.in > demo2.xball

demo3.xball : demo3.xball.in
	$(do_subst) < $(srcdir)/demo3.xball.in > demo3.xball

demo4.xball : demo4.xball.in
	$(do_subst) < $(srcdir)/demo4.xball.in > demo4.xball

demo5.xball : demo5.xball.in
	$(do_subst) < $(srcdir)/demo5.xball.in > demo5.xball

demo6.xball : demo6.xball.in
	$(do_subst) < $(srcdir)/demo6.xball.in > demo6.xball

demo12.xball : demo12.xball.in
	$(do_subst) < $(srcdir)/demo12.xball.in > demo12.xball

demo13.xball : demo13.xball.in
	$(do_subst) < $(srcdir)/demo13.xball.in > demo13.xball

demo14.xball : demo14.xball.in
	$(do_subst) < $(srcdir)/demo14.xball.in > demo14.xball

# example demo programs
dist_DEMOSin = demo1.xball.in demo2.xball.in demo3.xball.in demo4.xball.in \
	demo5.xball.in demo6.xball.in demo12.xball.in demo13.xball.in \
	demo14.xball.in

nodist_DEMOS = demo1.xball demo2.xball demo3.xball demo4.xball demo5.xball \
	demo6.xball demo12.xball demo13.xball demo14.xball

dist_DEMOS = demo7.xball demo8.xball demo9.xball demo10.xball demo11.xball

IMAGES = xball-a.xbm xball-b.xbm xball-l.xbm xball.xbm xball-x.xbm \
	man.xbm sgi.xbm

demosdir = ${datarootdir}/xball
demos_DATA = ${dist_DEMOS} ${nodist_DEMOS} ${IMAGES}

EXTRA_DIST = run_demos.in ${dist_DEMOS} ${dist_DEMOSin} ${IMAGES} \
	act_area.c bootstrap filemenu.c gen_fallback good_bitmaps \
	stipple.xbm XBall.ad xball.6 ${noinst_HEADERS} off.xbm on.xbm

MOSTLYCLEANFILES = run_demos rgbtxt_loc.h
DISTCLEANFILES = run_demos rgbtxt_loc.h

uninstall-hook:
	if test -d ${datarootdir}/xball; then rmdir ${datarootdir}/xball; fi

clean-local:
	-rm -rf run_demos ${nodist_DEMOS} rgbtxt_loc.h

distclean-local:
	-rm *~
	-rm -rf autom4te.cache
	-rm -rf .deps
	-rm -rf ${nodist_DEMOS} rgbtxt_loc.h

check-valgrind:
if HAVEVALGRIND
	$(MAKE);
	$(VALGRIND) --error-exitcode=1 $(builddir)/tests/list-tests
else
	echo "install Valgrind, then run ./configure again"
endif
