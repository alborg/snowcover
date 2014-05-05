# AX_METNO_ARG_WITH(LIB,[DEFAULT=yes],[REQUIRED=yes])
# ------------------------------------------------------
# Provides the initialisation step when using
# --with-LIB[=yes|no|DIR|INC,LIB] flag to the
# configure script. 
#
# For the sake of easiness, we use an example
# where the library is named 'foo'. 
# The flag is then:
# --with-foo[=yes|no|DIR|INC,LIB]
#
#
# PARAMETERS TO THE MACRO:
# ++++++++++++++++++++++++
# 1) The first parameter is the
# name of the library whose availability we 
# want to check. To (try and) use library foo, 
# use:
#    AX_METNO_ARG_WITH([foo])
# 
# 2) The second parameter, DEFAULT, expresses the 
# default behaviour you want with respect to 
# the library. If the default for your package 
# is *not to use* foo, then write:
#    AX_METNO_ARG_WITH([foo],[no])
# or
#    AX_METNO_ARG_WITH([foo],[yes])
# on the opposite case. The value of DEFAULT is
# the one returned when the user specifies no flag
# for library foo at configure time.
#
# 3) The third parameter, REQUIRED, translates the
# level of dependency of the configured package to
# library foo. If foo is mandatory then use:
#    AX_METNO_ARG_WITH([foo],,[yes])
# This will terminate the configure script with an
# error if a configuration without foo is requested
# by the user. If library foo is optional, use:
#    AX_METNO_ARG_WITH([foo],,[no])
#
# < Notes on the use of DEFAULT and REQUIRED >
# The decision to have foo an *optional* or *required*
# library is solely taken by the developer of 
# configure.ac.
#
# The decision to use (and thus check for) library
# foo is taken by the developer of the package (via the 
# value of DEFAULT) but can be overridden by the user 
# (via the --with-foo flags). 
#
# The DEFAULT and REQUIRED paramaters should only play 
# a role in this macro and nowhere else in the 
# configure.ac 'script'. REQUIRED is only designed to
# prevent a user from trying to configure the 
# package --without-foo. 
# Specifically, you should *not* use the value of 
# REQUIRED to  alter the behaviour of configure in 
# Indeed, once the user asked for the use of foo, 
# whether via a conscious or unconscious use of DEFAULT 
# or via the --with-foo flags, and if foo cannot be used,
# our configure script simply cannot terminate with a 
# success exit code, foo being required or not. An
# optional and non usable library can always be 
# explicitely turned off by the user via the flag
# --without-foo.
# 
# BEHAVIOUR AND OUTPUT:
# +++++++++++++++++++++
# A) On input, the user can choose:
#    <no flag>         : The default behaviour 
#                        is implemented.
#
#    --with-foo
#    or --with-foo=yes : The package is configured 
#                        *with* library foo.
#
#    --without-foo
#    or --with-foo=no  : The package is configured 
#                        *without* library foo.
#
#    --with-foo=DIR    : The pacakge is configured
#                        *with* foo. Header files for foo
#                        should be searched for in 
#                        subdirectories of DIR: the
#                        include file (foo.h?) in DIR/include
#                        and the library file (libfoo.a) in
#                        DIR/lib.
#
#   --with-foo=INC,LIB : Same as above but the two directories
#                        are specified explicitely, separated
#                        by a comma.
#
# B) On (error free) output, several variables are
#    set that can be used in the remaining of the
#    configure script:
#    
#    Variable *use_foo* is set to *yes* if the package
#    should be configured with library foo, and to *no* 
#    otherwise. This variable is always defined.
#
#    Variable *foo_CFLAGS* and *foo_LDFLAGS* are respectively
#    set to '-IheaderDIR' and '-LlibDIR'. Those two directories
#    are found from the user input. These two variables have
#    and empty value if the user did not used one of the
#    two =DIR or =INC,LIB for for --with-foo.
#
#    A message is issued to inform the user if the library
#    will be used or not. It makes use of variable use_foo. 
#
#  C) If an error is detected, the configure script is terminated with
#     a call to AC_MSG_ERROR. 
#     Error conditions:
#        1) One of the directories given by the user does not exist.
#        2) The user tries not to use foo when it is a requirement.
#
# NOTES:
# 1) This macro is not maint at checking if library foo *can be used*. 
#    It merely let the user decide if it *should be tested* and prepares 
#    some variables to ease the needed verifications.
#
# AUTHOR  : Thomas Lavergne, met.no/FOU/FM, 09/07/2007
#
# TODOS   : 
# 1) Have a better 'help message' for the user at configure (2nd parameter
#    to AC_ARG_WITH). The message could change to reflect if the library
#    is REQUIRED and what is the DEFAULT behaviour.
#
# 2) Check the values entered by the developer (DEFAULT and REQUIRED) they
#    should be one of '','yes' and 'no'.
#
# CVS_ID: 
#    $Id: ax_metno_arg_with.m4,v 1.3 2007-07-11 09:35:08 thomasl Exp $
#
AC_DEFUN([AX_METNO_ARG_WITH],[
     dnl Set the default values for the DEFAULT and REQUIRED variables
     default=ifelse(len($2),[0],[yes],[$2])
     required=ifelse(len($3),[0],[yes],[$3])

     AC_ARG_WITH([$1],
     [  --with-$1(=yes|no|DIR|INC,LIB)
          Use/Do not use the $1 library, at optionally specified location. ],
     [checkdirs=false
     case $with_$1 in
         yes) use_$1=yes  
            ;;
         no)  use_$1=no
            ;;
         *,*) use_$1=yes
            checkdirs=true
            addincdir="`echo $with_$1 | cut -f1 -d,`"
	    addlibdir="`echo $with_$1 | cut -f2 -d,`"
	    ;;
         *)   use_$1=yes
            checkdirs=true
            addincdir="$withval/include"
            addlibdir="$withval/lib"
	    ;;
      esac
      if test x$checkdirs = xtrue; then
	 if ! test -x $addincdir; then
	    AC_MSG_ERROR([Cannot find directory $addincdir])
	 else
	    $1_CFLAGS="-I$addincdir"
         fi
         if ! test -x $addlibdir; then
	    AC_MSG_ERROR([Cannot find directory $addlibdir])
	 else
	    $1_LDFLAGS="-L$addlibdir"
	 fi
      fi],[use_$1=$default])
      dnl AC_MSG_NOTICE([use_$1 has a value of $use_$1])
      dnl AC_MSG_NOTICE([required has a value of $required])
      if test $required = yes; then
         if test $use_$1 = no; then
	    AC_MSG_ERROR([Library $1 is required at configure time.])
	 fi
      fi
      AC_MSG_CHECKING([if we should test for $1])
      AS_IF([test x$use_$1 = xyes],[AC_MSG_RESULT([yes])],[AC_MSG_RESULT([no])])

      ])

