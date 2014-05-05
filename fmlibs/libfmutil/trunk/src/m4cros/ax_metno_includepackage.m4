# This function is provided to do  two things
# 1) make up the include string eg. all the -I<include path> 
# 2) make shure that all the include path strings exsist on the system.
# 3) finding if this is <packagename>/include/*h or 
#    include/<package name>/*h and getting include correct
# 
# if one of the to fails config will not finish, and one needs to install
# or set the correct path for the include package
#
# Parameter controll:
#	--with-<pakage name>=url : Over rides the spesified url, and uses the new url to search for the include package
#
# @param $1 package name eg puTools
# @param $2 the include path to  search on
#
AC_DEFUN([AX_METNO_INCLUDEPACKAGE],[
	AC_MSG_CHECKING([for $1 ])

	AC_ARG_WITH($1,[  --with-<package name>=url \tOver ride the predefined url for includeheaders],[
		base_path=$withval
	],[
		base_path=$2
	])

	if test -e $base_path/$1
	then
		#find if there is a /include/*h dir or just <package name>/*h
		if test -e $base_path/$1/include
		then
			TMPFOUND=-I[$base_path]/[$1]'/include/'
			FOUND_INCLUDES=$FOUND_INCLUDES' '$TMPFOUND
		else
			if test -e $base_path/$1/ 
			then
				TMPFOUND=-I[$base_path]/$1 
				FOUND_INCLUDES=$FOUND_INCLUDES' '$TMPFOUND
			else			
				AC_MSG_RESULT(no headers found)
				AC_MSG_ERROR([Did not find headers for $1 in ]$base_path[, in either $1/*h of $1/include/*h])
			fi
		fi
		AC_MSG_RESULT(found)
		AC_SUBST([FOUND_INCLUDES])
	else
		AC_MSG_RESULT(not found)
		AC_MSG_ERROR([Did not find headers for $1 in ]$base_path[, which is required])
	fi

])
