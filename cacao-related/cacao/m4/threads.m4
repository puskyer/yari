dnl m4/threads.m4
dnl
dnl Copyright (C) 2007 R. Grafl, A. Krall, C. Kruegel,
dnl C. Oates, R. Obermaisser, M. Platter, M. Probst, S. Ring,
dnl E. Steiner, C. Thalinger, D. Thuernbeck, P. Tomsich, C. Ullrich,
dnl J. Wenninger, Institut f. Computersprachen - TU Wien
dnl 
dnl This file is part of CACAO.
dnl 
dnl This program is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU General Public License as
dnl published by the Free Software Foundation; either version 2, or (at
dnl your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
dnl 02110-1301, USA.
dnl 
dnl $Id: configure.ac 7228 2007-01-19 01:13:48Z edwin $


dnl check which thread implementation should be used

AC_DEFUN([AC_CHECK_ENABLE_THREADS],[
AC_MSG_CHECKING(whether to include threads support)
AC_ARG_ENABLE([threads],
              [AS_HELP_STRING(--enable-threads,enable threads support (none,native) [[default=native]])],
              [case "${enableval}" in
                   no | none | single )
                       ENABLE_THREADS=no
                       ;;

                   posix | pthreads )
                       ENABLE_THREADS=posix
                       ;;

                   *)
                       AC_MSG_ERROR($enableval is an unknown thread package)
                       ;;
               esac],
               [ENABLE_THREADS=posix])

AC_MSG_RESULT(${ENABLE_THREADS})
AM_CONDITIONAL([ENABLE_THREADS], test x"${ENABLE_THREADS}" != "xno")

case "${ENABLE_THREADS}" in
    no )
        dnl no threads for boehm
        ac_configure_args="$ac_configure_args --disable-boehm-threads"
        ;;

    posix )
        AC_DEFINE([ENABLE_THREADS], 1, [enable threads])
        AC_CHECK_LIB(pthread, main)

        ARCH_CFLAGS="$ARCH_CFLAGS -D_REENTRANT"

        dnl we changed ARCH_CFLAGS, set CFLAGS again
        CFLAGS="$ARCH_CFLAGS $OPT_CFLAGS"

        dnl tell boehm to support threads as well
        ac_configure_args="$ac_configure_args --enable-boehm-threads=posix"
        ;;
esac
])


dnl currently NOT USED

dnl AC_ARG_ENABLE([__thread], [AS_HELP_STRING(--enable-__thread,use TLS features)], [use__thread=$enableval], [use__thread=no])
dnl 
dnl dnl Check whether the compiler supports the __thread keyword.
dnl if test "x$use__thread" != xno; then
dnl   AC_CACHE_CHECK([for __thread], ac_cv_gcc___thread,
dnl   [cat > conftest.c <<\EOF
dnl __thread int a = 42;
dnl EOF
dnl   if AC_TRY_COMMAND([${CC-cc} $ARCH_CFLAGS $OPT_CFLAGS -c conftest.c >&AS_MESSAGE_LOG_FD]); then
dnl     ac_cv_gcc___thread=yes 
dnl   else
dnl     ac_cv_gcc___thread=no
dnl   fi
dnl   rm -f conftest*])
dnl   if test "$ac_cv_gcc___thread" = yes; then
dnl     AC_DEFINE([HAVE___THREAD], 1, [have __thread])
dnl   fi
dnl else
dnl   ac_cv_gcc___thread=no
dnl fi
