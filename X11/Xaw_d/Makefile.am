# Copyright 2021, Jose Da Silva
#
# Permission to use, copy, modify, and distribute this software
# and its documentation for any purpose and without fee is granted
# provided that the above copyright notice appears in all copies.
# It is provided "as is" without express or implied warranty.

EXTRA_DIST = DrawingA.doc DrawingA.h DrawingAP.h Imakefile Makefile.std README

noinst_PROGRAMS =
demo_SOURCES =
demo_LDADD =
if ADD_DEMOXAW
noinst_PROGRAMS += demo
demo_SOURCES += demo.c DrawingA.c
demo_LDADD += -L/usr/X11R6/lib -lXaw -lXm -lXmu -lXt -lX11
else
EXTRA_DIST += demo.c DrawingA.c
endif
