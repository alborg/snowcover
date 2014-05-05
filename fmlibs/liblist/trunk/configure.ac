AC_PREREQ(2.59)
AC_INIT([METNO List Library], [1.0], [t.lavergne@met.no],[liblist])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([m4])


# ########################################################### 
# Checks for programs.
#
AC_PROG_CC_STDC
AS_IF([test x$ac_cv_prog_cc_stdc = xno],
      AC_MSG_ERROR([CC does not support current STDC standard. Abort]),
      AC_MSG_NOTICE([Can use CC in STDC mode]))
AC_PROG_LIBTOOL
AC_PROG_INSTALL

# ########################################################### 
# Checks for standard header files.
AC_CHECK_HEADERS([stdio.h stdlib.h])

#############################################################
# Checks for needed external libraries.
#   none

global_CFLAGS=""
AC_SUBST([global_CFLAGS],[$global_CFLAGS])

# Final messages and export of necessary variables to Makefile.in
successful_configure=yes
AS_IF([test $successful_configure = yes],
      AC_MSG_NOTICE([Configure was successful.]),
      AC_MSG_ERROR([The configure step is *not* successful. 
		    Check the warning messages above.]))

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT

