#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

m4_define([xball_version], [3.0.1])
AC_INIT([xball],[xball_version],[packages@qa.debian.org],
	[xball],[https://packages.debian.org/sid/xball])
AM_INIT_AUTOMAKE([foreign -Wall])
AC_DEFINE(MOTIF, HAVE_MOTIF)
AC_SUBST([XBALL_VERSION],[xball_version])
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL

# Required development include files (and associated libraries)
AC_MSG_CHECKING([Check for X11 and Motif required developer header files])
AC_CHECK_HEADER([X11/Intrinsic.h],[],[AC_MSG_FAILURE([ERROR: Missing X11/Intrinsic.h, Please install Developer version of libxt-devel or lib64xt-devel],[1])])
AC_CHECK_HEADER([Xm/Xm.h],[],[AC_MSG_FAILURE([ERROR: Missing Xm/Xmu.h, Please install Developer version of libxmotif-devel or lib64motif-devel],[1])])
AC_CHECK_HEADER([X11/Xmu/Converters.h],[],[AC_MSG_FAILURE([ERROR: Missing X11/Xmu/Converters.h, Please install Developer version of libxmu-devel or lib64xmu-devel],[1])])

# Build optional demo in directory ./X11/Xaw/
AC_ARG_ENABLE([Xaw],
  [AS_HELP_STRING([--enable-Xaw],
    [Build optional Xaw demo program @<:@default=check@:>@])],
  [],[enable_Xaw=check])
makeXaw=no
if test "x$enable_Xaw" != xno ; then
  AC_CHECK_HEADER([X11/Xaw/Form.h],[makeXaw=yes],
    [if test "x$enable_Xaw" != xcheck; then
       AC_MSG_FAILURE(
         [ERROR: Missing X11/Xaw/Form.h, Please install Developer version of libxaw-devel or lib64xaw-devel],[1])
     fi
    ])
fi
AM_CONDITIONAL([ADD_DEMOXAW],[test x${makeXaw} = xyes])

#update-mime-database /usr/local/share/mime/

AC_CONFIG_FILES([Makefile xball.pc desktop/Makefile X11/Xaw_d/Makefile])
AC_OUTPUT
AC_MSG_NOTICE([

Configuration:

  Source code location	${srcdir}
  Build code location	${builddir}
  Destination prefix	${prefix}
  Build extra Xaw demo	${enable_Xaw} ${makeXaw}
  Compiler		${CC}
  Config CFLAGS		"${CFLAGS}"

])
