###################################################################
#
# NAME : src/Makefile.am
#
# PURPOSE :
#    Input file for Automake, liblist project.
#    
#
# AUTHOR:
#    Thomas Lavergne, met.no/FoU, 16.05.2008
#
# MODIFIED:
#    Thomas Lavergne, met.no/FoU, 19.05.2008   :   add the sort functions
#
#


lib_LIBRARIES = liblist.a

liblist_a_SOURCES = \
		    dbl_list.c \
		    dbl_list.h \
		    tool_qsort.c \
		    tool_qsort.h

include_HEADERS = dbl_list.h

liblist_a_CFLAGS  = $(CFLAGS)
liblist_a_CFLAGS += $(global_CFLAGS)

