#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([upctr], [0.9], [mbdriscoll@cs.berkeley.edu])
AC_CONFIG_SRCDIR([upctr.C])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([config])

# Checks for programs.
AC_PROG_CXX

AC_ARG_WITH([rose],
    AS_HELP_STRING([--with-rose=@<:@ARG@:>@],
        [use ROSE library @<:@default=yes@:>@, \
         optionally specify path to ROSE installation directory]
    ),
    [
        if test "$withval" = "no"; then
            AC_MSG_ERROR([Cannot build --without-rose])
        elif test "$withval" = "yes"; then
            ROSE_PATH="/usr/local" # default path
        else
            ROSE_PATH="$withval"
        fi
    ],
    [
        ROSE_PATH="/usr/local" # default path
        AC_MSG_NOTICE([No path to ROSE specified. Using --with-rose=$ROSE_PATH])
    ]
)

CFLAGS="-I$ROSE_PATH/include"
LDFLAGS="-L$ROSE_PATH/lib"
LIBS="-lrose"

AC_CHECK_HEADER([rose.h],
    [],
    AC_MSG_ERROR([cannot find ROSE headers])
)

AC_CHECK_LIB([rose], [version_number],
    [],
    AC_MSG_ERROR([cannot link against ROSE library])
)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
