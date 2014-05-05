# AX_METNO_CHECK_HEADER(HEADER,[ADDCFLAGS],[ADDCPPFLAGS])
# -------------------------------------------------------
# Provides the AC_CHECK_HEADER facility but allowing for
# temporarily adding some -I directives to the CPPFLAGS
# and CFLAGS. These 2 additions are made temporarily and
# both CFLAGS and CPPFLAGS are turned back to their
# original values at the end of the macros.
#
# PARAMETERS TO THE MACRO:
# ++++++++++++++++++++++++
# 1) The first parameter is the name of the header file
#    we want to check the availability (and usability). 
#    For example foo.h
# 2) The second parameter is the flags (usually -Idir) that
#    are to be added to the CFLAGS variable for the test. Its
#    value is prepended to CFLAGS (CFLAGS="$2 $CFLAGS").
# 3) The third parameter is the same as the second but for 
#    variable CPPFLAGS.
#
# BEHAVIOUR AND OUTPUT:
# +++++++++++++++++++++
# A) The form:
#       AX_METNO_CHECK_HEADER([foo.h]) 
#    is the same as AC_CHECK_HEADER([foo.h]) except for the 
#    side effects in defining variables (see below). 
#    The form:
#       AX_METNO_CHECK_HEADER([foo.h],[-Ifoo/include],[-Ifoo/include])
#    Uses temporarily updated CFLAGS and CPPFLAGS for checking 
#    the header. See more details in NOTES below.
#
# B) On output to AX_METNO_CHECK_HEADER, variable 
#    ax_metno_check_header_ok is defined and its 
#    returned value is yes for a positive result 
#    (foo.h can be used) and no otherwise.
#
# C) There is no error case.   
#
# NOTES:
# 1) Both CFLAGS and CPPFLAGS are used because of the
#    (evolving) nature of the AC_CHECK_HEADER test
#    of autoconf. It indeed uses both the compiler
#    and the preprocessor for checking the 
#    'availability' and 'usability' of the header.
#
# 2) Although it can be used on its own, this macro
#    was originally written for a combined use with
#    AX_METNO_CHECK_LIBINCLUDE.
#
# AUTHOR  : Thomas Lavergne, met.no/FOU/FM, 10/07/2007
#
# TODOS   : 
# 1) If needed one day (and if it makes sense): make
#    this test portable for fortran programs (FFLAGS).
#
#
# CVS_ID: 
#    $Id: ax_metno_check_header.m4,v 1.2 2009-03-10 21:29:23 steingod Exp $
#
AC_DEFUN([AX_METNO_CHECK_HEADER],[
      dnl AC_MSG_NOTICE([2nd param is $2])
      dnl save the variables
      oldCFLAGS="$CFLAGS"
      oldCPPFLAGS="$CPPFLAGS"
      dnl prepend the value to the variables
      CFLAGS="$2 $CFLAGS"
      CPPFLAGS="$3 $CPPFLAGS"
      dnl perform the test
      AC_CHECK_HEADER([$1],[ax_metno_check_header_ok=yes],[ax_metno_check_header_ok=no])
      dnl restore the original values
      CFLAGS="$oldCFLAGS"
      CPPFLAGS="$oldCPPFLAGS"
      ])

