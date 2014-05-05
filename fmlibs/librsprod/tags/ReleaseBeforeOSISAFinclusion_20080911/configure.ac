AC_PREREQ(2.59)
AC_INIT([METNO Remote Sensing Product Library], [1.0], [thomasl@met.no],[librsprod])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign])



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

#############################################################
# Check for programs
#
AC_PROG_CC
AC_PROG_RANLIB

# ########################################################## 
# Checks for standard header files.
AC_CHECK_HEADERS([stdio.h stdlib.h math.h string.h])

# ##########################################################
# Check for the header of the library we are depending on
if test x$use_netcdf = xyes; then
   AX_METNO_CHECK_HEADER([netcdf.h],[$netcdf_CFLAGS],[$netcdf_CFLAGS])
   AS_IF([test $ax_metno_check_header_ok = yes],[netcdfok=yes],[netcdfok=no
	  AC_MSG_WARN([Could not find netcdf.h needed for library netcdf.])])
fi

# ########################################################### 
# Checks for types or methods to make sure the headers we found
# are related to the libraries we want.
if test x$use_netcdf = xyes; then
   oldCFLAGS="$CFLAGS"
   oldCPPFLAGS="$CPPFLAGS"
   CFLAGS="$netcdf_CFLAGS $CFLAGS"
   CPPFLAGS="$netcdf_CFLAGS $CPPFLAGS"
   AC_CHECK_DECL([nc_open],[netcdfok=yes],
		 [netcdfok=no
		  AC_MSG_WARN([Could not find function nc_open() in library netcdf.])],
		 [AC_INCLUDES_DEFAULT()
		  #include <netcdf.h>])
   CFLAGS="$oldCFLAGS"
   CPPFLAGS="$oldCPPFLAGS"
fi

# ########################################################### 
# Checks for linking against the libraries we depend on
if test x$use_netcdf = xyes; then
   AX_METNO_CHECK_LIB([nc_open],[netcdf],[$netcdf_LDFLAGS])
   AS_IF([test $ax_metno_check_lib_ok = yes],[netcdfok=yes
	  netcdf_LIBS="-lnetcdf"],[netcdfok=no
	  AC_MSG_WARN([Could not use nc_open in library netcdf.])])
fi

# Final messages and export of necessary variables to Makefile.in
successful_configure=yes
if test x$use_netcdf = xyes; then
   AC_MSG_CHECKING([whether we can compile with support to libnetcdf])
   AC_MSG_RESULT([$netcdfok])
   AS_IF([test $successful_configure = yes],[successful_configure=$netcdfok])
   if test $netcdfok = yes; then
      AC_SUBST([netcdf_CFLAGS],[$netcdf_CFLAGS])
      AC_SUBST([netcdf_LIBS],[$netcdf_LIBS])
      AC_SUBST([netcdf_LDFLAGS],[$netcdf_LDFLAGS])
   fi
fi
# librsprod needs support for C99 and we force the -g flag:
global_CFLAGS="--std=c99 -g"
AC_SUBST([global_CFLAGS],[$global_CFLAGS])

AS_IF([test $successful_configure = yes],
      AC_MSG_NOTICE([Configure was successful.]),
      AC_MSG_ERROR([The configure step is *not* successful. 
		    Check the warning messages above.]))

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT

