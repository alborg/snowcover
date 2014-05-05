AC_PREREQ(2.59)
AC_INIT([METNO List Library], [1.0], [thomasl@met.no],[liblist])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign])


# ########################################################### 
# Checks for programs.
#
AC_PROG_CC
AC_PROG_RANLIB

# ########################################################### 
# Checks for standard header files.
AC_CHECK_HEADERS([stdio.h stdlib.h])

#############################################################
# Checks for needed external libraries.

#############################################################
# Check for programs
#
AC_PROG_CC
AC_PROG_RANLIB



# librsprod needs support for C99 and we force the -g flag:
global_CFLAGS="--std=c99 -g"
AC_SUBST([global_CFLAGS],[$global_CFLAGS])

# Final messages and export of necessary variables to Makefile.in
successful_configure=yes
AS_IF([test $successful_configure = yes],
      AC_MSG_NOTICE([Configure was successful.]),
      AC_MSG_ERROR([The configure step is *not* successful. 
		    Check the warning messages above.]))

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT

