# AX_METNO_CHECK_LIB(FUNC,LIB,[ADDLDFLAGS],[ADDLIBS])
# -------------------------------------------------------
# Provides the AC_CHECK_LIB facility but allowing for
# temporarily adding some -L directives to the LDFLAGS. 
# This addition is made only temporarily and
# LDFLAGS is turned back to its original values at the 
# end of the macro.
#
# PARAMETERS TO THE MACRO:
# ++++++++++++++++++++++++
# 1) The first parameter is the name of the function that
#    we want to check the availability (through linking).
#    For example: newfoo
# 2) The second parameter is the name of the library in
#    which this function is supposedly found.
#    For example: foo   (the macro will use -lfoo)
# 3) The third parameter is the flags (usually -Ldir) that
#    are to be added to the LDFLAGS variable for the test. Its
#    value is prepended to LDFLAGS (LDFLAGS="$3 $LDFLAGS").
# 4) The fourth parameter is a list of additional libraries that
#    are required for the success of the test we are to run.
#
# BEHAVIOUR AND OUTPUT:
# +++++++++++++++++++++
# A) The form:
#       AX_METNO_CHECK_LIB([newfoo][foo]) 
#    is the same as AC_CHECK_LIB([foo][newfoo]) except for the 
#    side effects in defining variables (see below). 
#    The form:
#       AX_METNO_CHECK_LIB([newfoo],[foo],[-Lfoo/lib])
#    Uses temporarily updated LDFLAGS for checking 
#    the linking against library foo. 
#
# B) On output to AX_METNO_CHECK_LIB, variable 
#    ax_metno_check_lib_ok is defined and its 
#    returned value is yes for a positive result 
#    (we can link against libfoo.a) and no otherwise.
#
# C) There is no error case.   
#
# NOTES:
# 1) Although it can be used on its own, this macro
#    was originally written for a combined use with
#    AX_METNO_CHECK_LIBINCLUDE.
#
# AUTHOR  : Thomas Lavergne, met.no/FOU/FM, 15/04/2008
#
#
# CVS_ID: 
#    $Id: ax_metno_check_lib.m4,v 1.2 2010-02-12 14:11:55 thomasl Exp $
#
AC_DEFUN([AX_METNO_CHECK_LIB],[
      dnl AC_MSG_NOTICE([2nd param is $2])
      dnl save the variables
      oldLDFLAGS="$LDFLAGS"
      dnl prepend the value to the variables
      LDFLAGS="$3 $LDFLAGS"
      dnl perform the test
      AC_CHECK_LIB([$2],[$1],[ax_metno_check_lib_ok=yes],[ax_metno_check_lib_ok=no],[$4])
      dnl restore the original values
      LDFLAGS="$oldLDFLAGS"
      ])

