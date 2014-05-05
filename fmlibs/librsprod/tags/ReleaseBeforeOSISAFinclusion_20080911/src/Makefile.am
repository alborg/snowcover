###################################################################
#
# NAME : src/Makefile.am
#
# PURPOSE :
#    Input file for Automake, librsprod project.
#    
#
# AUTHOR:
#    Thomas Lavergne, met.no, 15.04.2008
#
# MODIFIED:
#
#


lib_LIBRARIES = librsprod.a

librsprod_a_SOURCES = \
		      calibration.c \
		      type.c \
		      notype.c \
		      field.c \
		      dimensions.c \
		      listattributes.c \
		      attributes.c \
		      nc_interface.c \
		      nc_interface.h \
		      calibration.h \
		      rsprod.h \
		      rsprod_intern.h \
		      list.c \
		      list.h \
		      rsprod_memory_utils.c \
		      rsprod_memory_utils.h \
		      rsprod_string_utils.c \
		      rsprod_string_utils.h 

include_HEADERS = rsprod.h

librsprod_a_CFLAGS  = $(CFLAGS)
librsprod_a_CFLAGS += $(global_CFLAGS)
if NETCDF_IS_ENABLED
librsprod_a_CFLAGS += $(netcdf_CFLAGS)
endif

rsprod.h : list.h rsprod_types.h rsprod_intern.h nc_interface.h calibration.h
	./prepare_rsprod_h.sh

clean-local:
	rm -f rsprod.h
