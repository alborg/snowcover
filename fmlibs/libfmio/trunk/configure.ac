#
# Autoconf setup
# Run autoreconf --install --verbose to generate configure
<<<<<<< configure.ac
# $Id: configure.ac,v 1.14 2009-04-01 13:25:24 steingod Exp $
=======
#
# MODIFIED:
# Thomas Lavergne, 02.11.2008  Cope with the dependency of fmutil on libproj's definition.
# Thomas Lavergne, 02.11.2008  BUG: used 'fmtiff' instead of 'tiff', same with 'fmproj'
#
# $Id: configure.ac,v 1.14 2009-04-01 13:25:24 steingod Exp $
>>>>>>> 1.13
#

AC_PREREQ(2.59)
AC_INIT([METNO Remote Sensing File Formats Library], [1.0], [o.godoy@met.no],[libfmio])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/fmio.h])
AM_CONFIG_HEADER([src/config.h])
AX_PREFIX_CONFIG_H([src/fmio_config.h],[fmio],[src/config.h]) # prep mylib/_config.h from it..


# ########################################################### 
# Command line options to configure (--with-package)
#


AX_METNO_ARG_WITH([fmutil],[yes],[yes])
AM_CONDITIONAL(FMUTIL_IS_ENABLED, test x$use_fmutil = xyes)
AX_METNO_ARG_WITH([proj],[no],[no])
AM_CONDITIONAL(PROJ_IS_ENABLED, test x$use_proj = xyes)
AX_METNO_ARG_WITH([tiff],[no],[no])
AM_CONDITIONAL(TIFF_IS_ENABLED, test x$use_tiff = xyes)
AX_METNO_ARG_WITH([hdf5],[no],[no])
AM_CONDITIONAL(HDF5_IS_ENABLED,[test x$use_hdf5 = xyes])

# ########################################################### 
# Checks for programs.
#
AC_PROG_CC
AC_PROG_RANLIB

# ########################################################## 
# Checks for standard header files.
AC_CHECK_HEADERS([stdio.h stdlib.h math.h string.h time.h])

# Check for the header of the library we are depending on
if test x$use_proj = xyes; then
   AX_METNO_CHECK_HEADER([projects.h],[$proj_CFLAGS],[$proj_CFLAGS])
   AS_IF([test $ax_metno_check_header_ok = yes],[projok=yes],[projok=no
	  AC_MSG_WARN([Could not find projects.h needed for library proj.])])
fi
if test x$use_fmutil = xyes; then
   AX_METNO_CHECK_HEADER([fmutil.h],[$proj_CFLAGS $fmutil_CFLAGS],[$proj_CFLAGS $fmutil_CFLAGS])
   AS_IF([test $ax_metno_check_header_ok = yes],[fmutilok=yes],[fmutilok=no
	  AC_MSG_WARN([Could not find fmutil.h needed for library fmutil.])])
fi
if test x$use_tiff = xyes; then
   AX_METNO_CHECK_HEADER([tiffio.h],[$tiff_CFLAGS],[$tiff_CFLAGS])
   AS_IF([test $ax_metno_check_header_ok = yes],[tiffok=yes],
	 [AC_MSG_WARN([Could not find tiffio.h needed for library tiff.])])
fi
if test x$use_hdf5 = xyes; then
   AX_METNO_CHECK_HEADER([hdf5.h],[$hdf5_CFLAGS],[$hdf5_CFLAGS])
   AS_IF([test $ax_metno_check_header_ok = yes],[hdf5ok=yes],
	 [AC_MSG_WARN([Could not find hdf5.h needed for library hdf5.])])
fi
 
## ########################################################### 
## Checks for types or methods to make sure the headers we found
## are related to the libraries we want.
if test x$use_fmutil = xyes; then
   oldCFLAGS="$CFLAGS"
   oldCPPFLAGS="$CPPFLAGS"
   CFLAGS="$proj_CFLAGS $fmutil_CFLAGS $CFLAGS"
   CPPFLAGS="$proj_CFLAGS $fmutil_CFLAGS $CPPFLAGS"
   AC_CHECK_DECL([fmerrmsg],[fmutilok=yes],
		 [fmutilok=no
		  AC_MSG_WARN([Could not find function fmerrmg() in library fmutil.])],
		 [AC_INCLUDES_DEFAULT() 
		  #include <fmutil.h>])
   CFLAGS="$oldCFLAGS"
   CPPFLAGS="$oldCPPFLAGS"
fi
##  
if test x$use_proj = xyes; then
   oldCFLAGS="$CFLAGS"
   oldCPPFLAGS="$CPPFLAGS"
   CFLAGS="$proj_CFLAGS $CFLAGS"
   CPPFLAGS="$proj_CFLAGS $CPPFLAGS"
   AC_CHECK_TYPE([projUV],[projok=yes],[projok=no],[#include <projects.h>])
   if test $projok = no; then
      AC_MSG_WARN([Could not find type projUV. Your version of libproj might be too old])
   fi
   CFLAGS="$oldCFLAGS"
   CPPFLAGS="$oldCPPFLAGS"
fi
if test x$use_tiff = xyes; then
   oldCFLAGS="$CFLAGS"
   oldCPPFLAGS="$CPPFLAGS"
   CFLAGS="$tiff_CFLAGS $CFLAGS"
   CPPFLAGS="$tiff_CFLAGS $CPPFLAGS"
   AC_CHECK_DECL([TIFFOpen],[tiffok=yes],
		 [tiffok=no
		  AC_MSG_WARN([Could not find function TIFFOpen() in library tiff.])],
		 [#include <tiffio.h>])
   CFLAGS="$oldCFLAGS"
   CPPFLAGS="$oldCPPFLAGS"
fi
if test x$use_hdf5 = xyes; then
   oldCFLAGS="$CFLAGS"
   oldCPPFLAGS="$CPPFLAGS"
   CFLAGS="$hdf5_CFLAGS $CFLAGS"
   CPPFLAGS="$hdf5_CFLAGS $CPPFLAGS"
   AC_CHECK_DECL([H5Fis_hdf5],[hdf5ok=yes],
		 [hdf5ok=no
		  AC_MSG_WARN([Could not find function H5Fis_hdf5() in library hdf5.])],
		 [#include <hdf5.h>])
   CFLAGS="$oldCFLAGS"
   CPPFLAGS="$oldCPPFLAGS"
fi

##
# Final messages and export of necessary variables
# to files config.h and Makefile.in
successful_configure=yes
if test x$use_fmutil = xyes; then
   AC_MSG_CHECKING([whether we can compile with support to libfmutil])
   AC_MSG_RESULT([$fmutilok])
   AS_IF([test $successful_configure = yes],[successful_configure=$fmutilok])
   if test $fmutilok = yes; then
      AC_DEFINE_UNQUOTED([HAVE_LIBFMUTIL],[1],[Define if libfmutil is available])
      AC_SUBST([FMUTIL_CFLAGS],[$fmutil_CFLAGS])
   fi
fi
if test x$use_proj = xyes; then
   AC_MSG_CHECKING([whether we can compile with support to libproj])
   AC_MSG_RESULT([$projok])
   AS_IF([test $successful_configure = yes],[successful_configure=$projok])
   if test $projok = yes; then
      AC_DEFINE_UNQUOTED([HAVE_LIBPROJ],[1],[Define if libproj is available])
      AC_SUBST([PROJ_CFLAGS],[$proj_CFLAGS])
   fi
fi
if test x$use_tiff = xyes; then
   AC_MSG_CHECKING([whether we can compile with support to libtiff])
   AC_MSG_RESULT([$tiffok])
   AS_IF([test $successful_configure = yes],[successful_configure=$tiffok])
   if test $tiffok = yes; then
      AC_DEFINE_UNQUOTED([HAVE_LIBTIFF],[1],[Define if libtiff is available])
      AC_SUBST([TIFF_CFLAGS],[$tiff_CFLAGS])
   fi
fi
if test x$use_hdf5 = xyes; then
   AC_MSG_CHECKING([whether we can compile with support to libhdf5])
   AC_MSG_RESULT([$hdf5ok])
   AS_IF([test $successful_configure = yes],[successful_configure=$hdf5ok])
   if test $hdf5ok = yes; then
      AC_DEFINE_UNQUOTED([HAVE_LIBHDF5],[1],[Define if libhdf5 is available])
      AC_SUBST([HDF5_CFLAGS],[$hdf5_CFLAGS])
   fi
fi

AS_IF([test $successful_configure = yes],
      AC_MSG_NOTICE([Configure was successful.]),
      AC_MSG_ERROR([The configure step is *not* successful. 
		    Check the warning messages above.]))

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

# Print Out Summary of Configuration
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
AC_MSG_RESULT([using libfmutil:	 $fmutilok])
AC_MSG_RESULT([using libproj:	 $projok])
AC_MSG_RESULT([using libhdf5:	 $hdf5ok])
AC_MSG_RESULT([using libtiff:	 $tiffok])
AC_MSG_RESULT([ ])
AC_MSG_RESULT([CPPFLAGS:         $CPPFLAGS])
AC_MSG_RESULT([CFLAGS:           $CFLAGS])
AC_MSG_RESULT([LDFLAGS:          $LDFLAGS])
AC_MSG_RESULT([LIBS:             $LIBS])
AC_MSG_RESULT([DEFS:             $DEFS])
AC_MSG_RESULT([  ])
AC_MSG_RESULT([------------------------------------------------------------])
AC_MSG_RESULT([ ])

