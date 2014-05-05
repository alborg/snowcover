#
# NAME:
# configure.ac
#
# PURPOSE:
# Template for using GNU Autoconf with libfmcol.
# Process this file with autoconf to produce a configure script.
#
# REQUIREMENTS:
# NA
#
# INPUT:
# NA
#
# OUTPUT:
# NA
#
# NOTES:
# This template have only been tested on Linux systems.
#
# BUGS:
# NA
#
# AUTHOR:
# Øystein Godøy, METNO/FOU, 29.09.2006 
#
# MODIFIED:
# Øystein Godøy, METNO/FOU, 2012-12-11: Adapted for HLHDF 0.79 and PPS
# 2012.
#
# CVS_ID:
# $Id: configure.ac,v 1.5 2007-09-12 14:36:21 steingod Exp $
#

AC_PREREQ(2.59)
AC_INIT([METNO Remote Sensing Data Collocation Library], [1.1], [o.godoy@met.no])
AC_CONFIG_SRCDIR([src/fmcolaccess.h])
AC_CONFIG_HEADER([src/fmcol_config.h])

# ########################################################### 
# Checks for programs.
# 
AC_PROG_CC

# ########################################################## 
# Checks for libraries, both useful and required ones
#
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
      LIBS="-l$1 $LIBS"
     ;;
    no)
    AC_MSG_ERROR([lib$1 is required])
     ;;
    *,*)
      addincdir="`echo $with_$1 | cut -f1 -d,`"
      addlibdir="`echo $with_$1 | cut -f2 -d,`"
      CPPFLAGS="$CPPFLAGS -I$addincdir"
      LDFLAGS="$LDFLAGS -L$addlibdir"
      LIBS="-l$1 $LIBS"
      ;;
    *)
      addincdir="$with_$1/include"
      addlibdir="$with_$1/lib"
      CPPFLAGS="$CPPFLAGS -I$addincdir"
      LDFLAGS="$LDFLAGS -L$addlibdir"
      LIBS="-l$1 $LIBS"
      ;;
esac
if test [ $with_$1 != no]; then
    fmcol_have_feat=m4_toupper(FMCOL_HAVE_LIB$1)
    AC_CHECK_LIB([$1],[$2],
        AC_DEFINE_UNQUOTED([$fmcol_have_feat],[1],[Define if $1 is available]),
    	[CPPFLAGS="$saved_CPPFLAGS";LDFLAGS="$saved_LDFLAGS";
	LIBS="$saved_LIBS";
	AC_MSG_ERROR([Did not find lib$1, this is required to continue])])
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
      LIBS="$LIBS -l$1"
     ;;
    no)
    AC_MSG_WARN([Building library with lib$1 dependent functions disabled])
     ;;
    *,*)
      addincdir="`echo $with_$1 | cut -f1 -d,`"
      addlibdir="`echo $with_$1 | cut -f2 -d,`"
      CPPFLAGS="$CPPFLAGS -I$addincdir"
      LDFLAGS="$LDFLAGS -L$addlibdir"
      LIBS="-l$1 $LIBS"
      ;;
    *)
      addincdir="$with_$1/include"
      addlibdir="$with_$1/lib"
      CPPFLAGS="$CPPFLAGS -I$addincdir"
      LDFLAGS="$LDFLAGS -L$addlibdir"
      LIBS="-l$1 $LIBS"
      ;;
esac
if test [ $with_$1 != no]; then
    fmcol_have_feat=m4_toupper(FMCOL_HAVE_LIB$1)
    AC_CHECK_LIB([$1],[$2],
        AC_DEFINE_UNQUOTED([$fmcol_have_feat],[1],[Define if $1 is available]),
    	[CPPFLAGS="$saved_CPPFLAGS";LDFLAGS="$saved_LDFLAGS";
	LIBS="$saved_LIBS";
	AC_MSG_WARN([=======================================]);
	AC_MSG_WARN([Did not find lib$1]);
	AC_MSG_WARN([Disabling lib$1 dependent functions]);
	AC_MSG_WARN([=======================================])],[$3])
fi
])

FM_REQLIB([hdf5],[H5Fopen])

FM_USELIB([hlhdf],[HL_init])

FM_REQLIB([proj],[pj_init])

##FM_USELIB([smhi_saf],[readCloudType],[-lpps_config])

# After implementing support for PPS2012 and HLHDF 0.79 testing for PPS
# libraries required a rewrite as the use extern for MODULE_ID and
# sm_debug. A specific check is thus written for this library family.
#FM_USELIB([smhi_saf],[newCloudType],[-lpps_config -lpps_ioutil -lpps_projects -lpps_filters])
#FM_USELIB([pps_filters],[filterTypeIndex])
saved_CPPFLAGS="$CPPFLAGS"
saved_LDFLAGS="$LDFLAGS"
saved_LIBS="$LIBS"
AC_ARG_WITH([smhi_saf],
        AC_HELP_STRING([--with-smhi_saf=DIR],
            [the location of optional libsmhi_saf files and library either as DIR or INC,LIB]),
        ,
        [with_smhi_saf=yes])
case $with_smhi_saf in
    yes)
     echo "Using system implementation of lib$1"
      LIBS="-lsmhi_saf $LIBS"
     ;;
    no)
    AC_MSG_WARN([Building library with lib$1 dependent functions disabled])
     ;;
    *,*)
      addincdir="`echo $with_smhi_saf | cut -f1 -d,`"
      addlibdir="`echo $with_smhi_saf | cut -f2 -d,`"
      CPPFLAGS="$CPPFLAGS -I$addincdir"
      LDFLAGS="$LDFLAGS -L$addlibdir"
      LIBS="-lsmhi_saf -lpps_config $LIBS"
      ;;
    *)
      addincdir="$with_smhi_saf/include"
      addlibdir="$with_smhi_saf/lib"
      CPPFLAGS="$CPPFLAGS -I$addincdir"
      LDFLAGS="$LDFLAGS -L$addlibdir"
      LIBS="-lsmhi_saf -lpps_config $LIBS"
      ;;
esac
AC_LANG(C)
AC_LINK_IFELSE([AC_LANG_PROGRAM([char *MODULE_ID="mytst";
int sm_debug;],
[readCloudType();])],
AC_DEFINE_UNQUOTED([FMCOL_HAVE_LIBSMHI_SAF],[1],
[Define if libsmhi_saf etc is available]),
[CPPFLAGS="$saved_CPPFLAGS";LDFLAGS="$saved_LDFLAGS";
LIBS="$saved_LIBS";
AC_MSG_WARN([=======================================]);
AC_MSG_WARN([Did not find libsmhi_saf and related libraries]);
AC_MSG_WARN([Disabling libsmhi_saf dependent functions]);
AC_MSG_WARN([=======================================])])

FM_REQLIB([fmutil],[fmivec])

FM_REQLIB([fmio],[fm_readheader])

FM_USELIB([osihdf5],[init_osihdf])

# ########################################################## 
# Defines some useful variables to put in config files
#
AC_DEFINE_UNQUOTED([FMCOL_PACKAGE_NAME],[$PACKAGE_NAME])
AC_DEFINE_UNQUOTED([FMCOL_PACKAGE_VERSION],[$PACKAGE_VERSION])
AC_DEFINE_UNQUOTED([FMCOL_PACKAGE_BUGREPORT],[$PACKAGE_BUGREPORT])
AC_DEFINE_UNQUOTED([FMCOL_PACKAGE_STRING],[$PACKAGE_STRING])
AC_DEFINE_UNQUOTED([FMCOL_PACKAGE_TARNAME],[$PACKAGE_TARNAME])

# ########################################################### 
# Checks for header files.
#
AC_HEADER_STDC

# ########################################################### 
# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

# ########################################################### 
# Print Out Summary of Configuration
#
AC_MSG_RESULT([ ])
AC_MSG_RESULT([$PACKAGE_NAME $PACKAGE_VERSION])
AC_MSG_RESULT([Configuration:])
AC_MSG_RESULT([------------------------------------------------------------])
AC_MSG_RESULT([ ])
AC_MSG_RESULT([Source Code:      $srcdir])
AC_MSG_RESULT([Host System:      $host])
AC_MSG_RESULT([  ])
AC_MSG_RESULT([Prefix:           $prefix])
AC_MSG_RESULT([Binaries:         $exec_prefix])
AC_MSG_RESULT([Manuals:          $mandir])
AC_MSG_RESULT([Data:             $datadir])
AC_MSG_RESULT([System Config:    $sysconfdir])
AC_MSG_RESULT([ ])
AC_MSG_RESULT([CPPFLAGS:         $CPPFLAGS])
AC_MSG_RESULT([LDFLAGS:          $LDFLAGS])
AC_MSG_RESULT([LIBS:             $LIBS])
AC_MSG_RESULT([DEFS:             $DEFS])
AC_MSG_RESULT([  ])
AC_MSG_RESULT([------------------------------------------------------------])
AC_MSG_RESULT([ ])

