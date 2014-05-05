###################################################################
#
# NAME : test/Makefile.am
#
# PURPOSE :
#    Input file for Automake, librsprod project.
#
# NOTE :
#    No 'make all' target is available. Use 'make check'.
#
# AUTHOR:
#    Thomas Lavergne, met.no, 28.11.2012
#
# MODIFIED:
#


check_PROGRAMS = test_fmtime_1 test_fmtime_2 test_fmtime_3
test_fmtime_1_SOURCES = test_fmtime_1.c
test_fmtime_2_SOURCES = test_fmtime_2.c
test_fmtime_3_SOURCES = test_fmtime_3.c

TESTS = $(check_PROGRAMS)

LDADD   = ../src/libfmutil.a -lm

CFLAGS   += $(global_CFLAGS)
LDFLAGS  += $(global_LDFLAGS) 

CPPFLAGS  += $(list_CPPFLAGS)
LDFLAGS += $(list_LDFLAGS)
LIBS    += $(list_LIBS)

#if WITH_NETCDF
#CPPFLAGS  += $(netcdf_CPPFLAGS)
#LDFLAGS += $(netcdf_LDFLAGS)
#LIBS    += $(netcdf_LIBS)
#endif
#if WITH_HDF4
#CPPFLAGS  += $(hdf4_CPPFLAGS)
#LDFLAGS += $(hdf4_LDFLAGS)
#LIBS    += $(hdf4_LIBS)
#endif
