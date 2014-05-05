# AX_METNO_CHECK_LIBHEADER(HEADER,LIB)
# -------------------------------------------------------
# 
# <TODO --- PRELIMINARY NOTE:>
#    This macro does not work. It necessitates some more
#    work because of the lack of actual knowledge about m4
#    programming. Specifically, the macro fails at calling
#    the sub macro AX_METNO_CHECK_HEADER because of 
#    parameter substitution. This macro is currently set
#    as OBSOLETE.
#    The work around is to code in autoconf.ac the actual
#    body of this macro, with parameter substitution.
# </TODO --- PRELIMINARY NOTE:>
# 
# Check the possibility to locate and use header file 
# HEADER when testing if library LIB can be used.
# This macro was primarily designed to work with
# libraries introduced in the configure process by a
# call to AX_METNO_ARG_WITH.
#
# PARAMETERS TO THE MACRO:
# ++++++++++++++++++++++++
# 1) The first parameter is the name of the header file
#    we want to check the availability (and usability). 
#    For example foo.h
# 2) The second parameter is the name of the library
#    HEADER is attached to. For example foo.h might be
#    related to library foo.
#
# BEHAVIOUR AND OUTPUT:
# +++++++++++++++++++++
# A) Considering the following sequence in configure.ac:
#       AX_METNO_ARG_WITH([foo])
#       if test $use_foo = yes; then
#          AX_METNO_CHECK_LIBHEADER([foo.h],[foo]) 
#       fi
#    AX_METNO_CHECK_LIBHEADER makes use of variable
#    foo_CFLAGS to customize a call to macro
#    AX_METNO_CHECK_HEADER.
#
# B) On output, AX_METNO_CHECK_LIBHEADER defines
#    and set variable foook to yes (if the test
#    for the header foo.h was successfull).
#    to no otherwise.
#
# C) If an error is detected, the configure script 
#    is terminated with a call to AC_MSG_ERROR. 
#    Error conditions:
#       1) File foo.h could not be located or used.
#
# NOTES:
# 1) This macro should only be used if the intended
#    behaviour is to check for the possibiliy to use
#    library foo. The present macro implements only
#    one level of test: the one based on the header
#    files.
#
# 2) This macro relies on variables such as foo_CFLAGS.
#    Its call should always be under the condition
#    that those exist (see code snipset in A), above).
#
# AUTHOR  : Thomas Lavergne, met.no/FOU/FM, 10/07/2007
#
# TODOS   : 
#
#
# CVS_ID: 
#    $Id: ax_metno_check_libheader.m4,v 1.2 2009-03-10 21:29:23 steingod Exp $
#
AC_DEFUN([AX_METNO_CHECK_LIBHEADER],[
      dnl call AX_METNO_CHECK_HEADER
      param=${!$2_CFLAGS}
      AC_MSG_NOTICE([param is $param])
      AX_METNO_CHECK_HEADER([$1],[$param],[$param])
      dnl check the result
      if test $ax_metno_check_header_ok = no; then
         AC_MSG_ERROR([Could not find header file $1 for library $2.])
      fi
      ])

