#
# NAME:
# configure.ac
#
# PURPOSE:
# Template for using GNU Autoconf with libfmio.
#
# REQUIREMENTS:
#
# INPUT:
#
# OUTPUT:
#
# NOTES:
# Process this file with autoconf to produce a configure script.
#
# This template have only been tested on Linux systems.
#
# BUGS:
#
# AUTHOR:
# Øystein Godøy, METNO/FOU, 29.09.2006 
#
# MODIFIED:
# NA
#

AC_PREREQ(2.59)
AC_INIT([METNO Remote Sensing File Formats Library], [1.0], [o.godoy@met.no])
AC_CONFIG_SRCDIR([src/fmio.h])
AC_CONFIG_HEADER([src/config.h])

# Checks for programs.
AC_PROG_CC

AC_DEFUN([FM_REQLIB],[
# Checks for libraries that are required.
saved_CPPFLAGS="$CPPFLAGS"
saved_LDFLAGS="$LDFLAGS"
saved_LIBS="$LIBS"
AC_ARG_WITH([$1],
    AC_HELP_STRING([--with-$1=DIR],
	[the location of mandatory lib$1 files and library either as DIR or INC,LIB]),
	,
	[with_$1=yes])
case $with_$1 in
    yes)
     echo "Using system implementation of lib$1"
     ;;
    no)
    AC_MSG_ERROR([lib$1 is required])
     ;;
    *,*)
      incdir="`echo $with_$1 | cut -f1 -d,`"
      libdir="`echo $with_$1 | cut -f2 -d,`"
      CPPFLAGS="$CPPFLAGS -I$incdir"
      LDFLAGS="$LDFLAGS -L$libdir"
      LIBS="$LIBS"
      ;;
    *)
      incdir="$with_$1/include"
      libdir="$with_$1/lib"
      CPPFLAGS="$CPPFLAGS -I$incdir"
      LDFLAGS="$LDFLAGS -L$libdir"
      LIBS="$LIBS"
      ;;
esac
if test [ $with_$1 != no]; then
    AC_CHECK_LIB([$1],[$2],
    	,[CPPFLAGS="$saved_CPPFLAGS";LDFLAGS="$saved_LDFLAGS";
	LIBS="$saved_LIBS";
	AC_MSG_ERROR([Did not find lib$1])])
fi
])

AC_DEFUN([FM_USELIB],[
# Checks for libraries that could be dropped.
saved_CPPFLAGS="$CPPFLAGS"
saved_LDFLAGS="$LDFLAGS"
saved_LIBS="$LIBS"
AC_ARG_WITH([$1],
    AC_HELP_STRING([--with-$1=DIR],
	[the location of optional lib$1 files and library either as DIR or INC,LIB]),
	,
	[with_$1=yes])
case $with_$1 in
    yes)
     echo "Using system implementation of lib$1"
     ;;
    no)
    AC_MSG_WARN([Building library with lib$1 dependent functions disabled])
     ;;
    *,*)
      incdir="`echo $with_$1 | cut -f1 -d,`"
      libdir="`echo $with_$1 | cut -f2 -d,`"
      CPPFLAGS="$CPPFLAGS -I$incdir"
      LDFLAGS="$LDFLAGS -L$libdir"
      LIBS="$LIBS"
      ;;
    *)
      incdir="$with_$1/include"
      libdir="$with_$1/lib"
      CPPFLAGS="$CPPFLAGS -I$incdir"
      LDFLAGS="$LDFLAGS -L$libdir"
      LIBS="$LIBS"
      ;;
esac
if test [ $with_$1 != no]; then
    AC_CHECK_LIB([$1],[$2],
    	,[CPPFLAGS="$saved_CPPFLAGS";LDFLAGS="$saved_LDFLAGS";
	LIBS="$saved_LIBS";
	AC_MSG_WARN([Did not find lib$1, building library with lib$1 dependent functions disabled])])
fi
])

FM_REQLIB([fmutil],[fmivec])

FM_USELIB([proj],[pj_init])

FM_USELIB([hdf5],[H5Fopen])

FM_USELIB([tiff],[TIFFOpen])

##FM_USELIB([hdf_format],[lib_prod_read_mph])

##FM_USELIB([read_hdf3],[lib_read_group])


# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
