AC_PREREQ(2.59)
AC_INIT([METNO Remote Sensing Product Library], [1.0], [thomasl@met.no],[librsprod])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/rsprod.h])


# ########################################################### 
# Checks for programs.
#
AC_PROG_CC
AC_PROG_RANLIB

# ########################################################### 
# Checks for standard header files.
AC_CHECK_HEADERS([stdio.h stdlib.h math.h string.h time.h])

#############################################################
# Checks for needed external libraries.
AX_METNO_ARG_WITH([netcdf],[yes],[yes])
AM_CONDITIONAL(NETCDF_IS_ENABLED, test x$use_netcdf = xyes)
if test x$use_netcdf = xyes; then
   AX_METNO_CHECK_HEADER([netcdf.h],[$netcdf_CFLAGS],[$netcdf_CFLAGS])
   AS_IF([test $ax_metno_check_header_ok = yes],[netcdfok=yes],[netcdfok=no
	  AC_MSG_WARN([Could not find netcdf.h needed for library netcdf.])])
fi
if test x$use_netcdf = xyes; then
   oldCFLAGS="$CFLAGS"
   oldCPPFLAGS="$CPPFLAGS"
   CFLAGS="$netcdf_CFLAGS $CFLAGS"
   CPPFLAGS="$netcdf_CFLAGS $CPPFLAGS"
   AC_CHECK_DECL([nc_open],[netcdfok=yes],
		 [netcdfok=no
		  AC_MSG_WARN([Could not find function nc_open() in library netcdf.])],
		 [AC_INCLUDES_DEFAULT()
		  #include <netcdf-3/netcdf.h>])
   CFLAGS="$oldCFLAGS"
   CPPFLAGS="$oldCPPFLAGS"
fi

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

