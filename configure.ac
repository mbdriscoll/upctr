AC_INIT([upctr], [0.9], [mbdriscoll@cs.berkeley.edu])
AC_PREREQ([2.68])

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign tar-pax 1.9.5])

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


BOOST_CXXFLAGS="-I/Users/driscoll6/macports/include"
BOOST_LDFLAGS="-L/Users/driscoll6/macports/lib"

AC_SUBST(ROSE_CXXFLAGS, "-I$ROSE_PATH/include $BOOST_CXXFLAGS")
AC_SUBST(ROSE_LDFLAGS, "-L$ROSE_PATH/lib -lrose $BOOST_LDFLAGS")

#  AC_CHECK_HEADER([rose.h],
#      [],
#      AC_MSG_ERROR([cannot find ROSE headers])
#  )
#
#  AC_CHECK_LIB([rose], [version_number],
#      [],
#      AC_MSG_ERROR([cannot link against ROSE library])
#  )

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
