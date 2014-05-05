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
#    TL, met.no, 13.04.2010   :   Separate the core of the library from the APIs for
#                                    each of the supported file formats
#
###################################################################


lib_LIBRARIES   = librsprod.a
include_HEADERS = rsprod.h

BUILT_SOURCES       =  $(include_HEADERS)

# ---------------------------------
# API per supported file formats
# ---------------------------------
# -- netCDF
netcdf_ApiH    = nc_interface.h
netcdf_ApiC    = nc_interface.c
# -- HDF4
hdf4_ApiH      = hdf4_interface.h
hdf4_ApiC      = hdf4_interface.c

# ---------------------------------
# CORE of library
# ---------------------------------
# -- Public headers for the core of the library 
rsprodH = rsprod_typedefs.h rsprod_types.h runtime_config.h rsprod_intern.h calibration.h stringdata.h x_interface.h
# -- Source code and private headers for the core of the library 
rsprodC =     stringdata.c \
		      calibration.c \
		      type.c \
		      notype.c \
		      field.c \
		      file.c \
		      dimensions.c \
		      listattributes.c \
		      attributes.c \
			  query_string.c \
			  query_string.h \
		      rsprod_memory_utils.c \
		      rsprod_memory_utils.h \
		      rsprod_string_utils.c \
			  rsprod_report_utils.c \
			  rsprod_report_utils.h \
		      runtime_config.c 

# ---------------------------------
# Build instruction corresponding to the configuration options
# ---------------------------------
librsprod_a_CFLAGS    = $(CFLAGS)
librsprod_a_CPPFLAGS  = $(CPPFLAGS)
apiH =
apiC =
librsprod_a_CFLAGS += $(global_CFLAGS)
# dependency on liblist
librsprod_a_CPPFLAGS += $(list_CPPFLAGS)
if WITH_NETCDF
librsprod_a_CPPFLAGS += $(netcdf_CPPFLAGS)
apiH               += $(netcdf_ApiH)
apiC               += $(netcdf_ApiC)
endif
if WITH_HDF4
librsprod_a_CPPFLAGS += $(hdf4_CPPFLAGS)
apiH               += $(hdf4_ApiH)
apiC               += $(hdf4_ApiC)
endif

librsprod_a_SOURCES = $(rsprodC) $(rsprodH) $(apiC) $(apiH) $(include_HEADERS)

# ---------------------------------
# Build header file 'rsprod.h' by concatenation
# ---------------------------------
#    note: since $(netcdf_ApiH) is included, the netCDF API will be loaded by 
#       all source code that #include <rsprod.h>. This is temporary solution.
$(include_HEADERS) : $(rsprodH) $(apiH)
	$(abs_srcdir)/prepare_rsprod_h.sh $@ $^

clean-local:
	rm -f $(include_HEADERS)
