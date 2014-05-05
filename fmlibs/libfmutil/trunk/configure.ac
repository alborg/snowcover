AC_PREREQ(2.59)
AC_INIT([METNO Remote Sensing Applications Utility Library], [1.0], [o.godoy@met.no],[libfmutil])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/fmtime.h])
AM_CONFIG_HEADER([src/config.h])

AX_PREFIX_CONFIG_H([src/fmutil_config.h],[FMUTIL],[src/config.h])

# ########################################################### 
# Command line options to configure (--with-package)
#

# Propose the --with-proj= option. The use of libproj is not
# mandatory and is not the default either.
AX_METNO_ARG_WITH([proj],[no],[no])

# Use the $use_proj macro set in AX_METNO_ARG_WITH
AM_CONDITIONAL(PROJ_IS_ENABLED, test x$use_proj = xyes)

# ########################################################### 
# Checks for programs.
#
AC_PROG_CC
AC_PROG_RANLIB

# ########################################################## 
# Checks for standard header files.
AC_CHECK_HEADERS([stdio.h stdlib.h math.h string.h time.h])
AC_CHECK_HEADERS([stdarg.h])

# Checks for local header files.
if test x$use_proj = xyes; then

   AX_METNO_CHECK_HEADER([projects.h],[$proj_CFLAGS],[$proj_CFLAGS])
   AS_IF([test $ax_metno_check_header_ok = yes],[projok=yes],[projok=no
	  AC_MSG_WARN([Could not find projects.h needed by library proj.])])

fi

# ###########################################################
# Check for some usefull functions
AC_CHECK_FUNCS(strtof, , [AC_MSG_WARN([Cannot find strtof, use atof instead])])
AC_CHECK_FUNCS(strtol, , [AC_MSG_WARN([Cannot find strtol, use atol instead])])

# ########################################################### 
# Checks for types.
#  
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


# ########################################################### 
# Checks for libraries.
#
#AC_CHECK_LIB([m],[cos],,AC_MSG_ERROR([Could not find the system math library]))
#if test x$proj = xyes; then
#   AC_CHECK_LIB([proj],[pj_init],[projok=yes],[projok=no])
#   if test $projok = no; then
#      AC_MSG_ERROR([Could not find the libproj.a library. Consider using --with-proj=no or --with-proj=path-to-lib.])
#   fi
#fi


#
# Final messages and export of necessary variables
# to files config.h and Makefile.in
successful_configure=yes
if test x$use_proj = xyes; then
   AC_MSG_CHECKING([whether we can compile with support to libproj])
   AC_MSG_RESULT([$projok])
   AS_IF([test $successful_configure = yes],[successful_configure=$projok])
   if test $projok = yes; then
      AC_DEFINE_UNQUOTED([HAVE_LIBPROJ],[1],[Define if proj is available])
      AC_SUBST([PROJ_CFLAGS],[$proj_CFLAGS])
   fi
fi

# libfmutil needs support for C99 and we force the -g flag:
global_CFLAGS="--std=c99 -g"
AC_SUBST([global_CFLAGS],[$global_CFLAGS])

AS_IF([test $successful_configure = yes],
      AC_MSG_NOTICE([Configure was successful.]),
      AC_MSG_ERROR([The configure step is *not* successful. 
		    Check the warning messages above.]))

AC_CONFIG_FILES([Makefile src/Makefile testsuite/Makefile])
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
AC_MSG_RESULT([PROJ support:	 $projok])
AC_MSG_RESULT([ ])
AC_MSG_RESULT([CPPFLAGS:         $CPPFLAGS])
AC_MSG_RESULT([CFLAGS:           $CFLAGS $global_CFLAGS])
AC_MSG_RESULT([LDFLAGS:          $LDFLAGS])
AC_MSG_RESULT([LIBS:             $LIBS])
AC_MSG_RESULT([DEFS:             $DEFS])
AC_MSG_RESULT([  ])
AC_MSG_RESULT([------------------------------------------------------------])
AC_MSG_RESULT([ ])

