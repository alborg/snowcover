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
#    Thomas Lavergne, met.no, 15.04.2008
#
# MODIFIED:
#    Thomas Lavergne, met.no, 23.04.2008      :   add the packAndUnpack test
#    Thomas Lavergne, met.no, 29.04.2008      :   add the ncInterface   test
#    Thomas Lavergne, met.no/FoU, 09.05.2008  :   add the String Dataset test
#

check_PROGRAMS = test4.exe test3.exe test2.exe test1.exe testHW.exe

test4_exe_SOURCES   = test_StringData.c
test3_exe_SOURCES   = test_ncInterface.c
test2_exe_SOURCES   = test_PackAndUnpack.c
test1_exe_SOURCES   = test_createAndWriteStandardField.c
testHW_exe_SOURCES  = test_helloWorld.c

LDADD   = ../src/librsprod.a $(LIBS)
testHW_exe_LDADD =

CFLAGS   += $(global_CFLAGS) -g
LDFLAGS  += $(global_LDFLAGS) -g
if NETCDF_IS_ENABLED
CFLAGS  += $(netcdf_CFLAGS)
LDFLAGS += $(netcdf_LDFLAGS)
LIBS    += $(netcdf_LIBS)
endif
