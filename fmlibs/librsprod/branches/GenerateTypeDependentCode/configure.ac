AC_PREREQ(2.59)
AC_INIT([METNO Remote Sensing Product Library], [1.0], [t.lavergne@met.no],[librsprod])
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
AC_CHECK_HEADERS([stdio.h stdlib.h math.h string.h time.h])

#############################################################
# Checks for needed external libraries.
METNO_REQUIRE_C_LIBRARY([netcdf], [netcdf], [netcdf.h], [nc_open])
METNO_WITH_CONDITIONAL([NETCDF],[netcdf])
METNO_REQUIRE_C_LIBRARY([hdf4], [mfhdf], [mfhdf.h], [SDstart], [-ldf -ljpeg -lz])
METNO_WITH_CONDITIONAL([HDF4],[mfhdf])

global_CFLAGS=""
AC_SUBST([global_CFLAGS],[$global_CFLAGS])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile rsprod.pc])

# liblist dependency is now handled by 'nested' autotooling and
#   we use the liblist implementation that is contained in the
#   subdirectory liblist.
AC_CONFIG_SUBDIRS([liblist])
AC_SUBST([list_CFLAGS],[])
AC_SUBST([list_CPPFLAGS],[-I`pwd`/liblist/src])
AC_SUBST([list_LDFLAGS],[-L`pwd`/liblist/src])
AC_SUBST([list_LIBS],[-llist])
AC_OUTPUT

