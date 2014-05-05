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
#    Thomas Lavergne, met.no/FoU, 15.02.2009  :   add testing the hdf4 interface
#

check_PROGRAMS = test10.exe test9.exe test8.exe test7.exe test6.exe \
				 test5.exe test4.exe test2.exe test1.exe testHW.exe

if WITH_HDF4
check_PROGRAMS += test3.exe
endif

test10_exe_SOURCES  = test_ncUnlimited.c
test9_exe_SOURCES   = test_Config.c
test8_exe_SOURCES   = test_ncFile.c
test7_exe_SOURCES   = test_ncFileRead.c
test6_exe_SOURCES   = test_NoType.c
test5_exe_SOURCES   = test_StringData.c
test4_exe_SOURCES   = test_ncInterface.c
test3_exe_SOURCES   = test_hdf4Interface.c
test2_exe_SOURCES   = test_PackAndUnpack.c
test1_exe_SOURCES   = test_createAndWriteStandardField.c
testHW_exe_SOURCES  = test_helloWorld.c

LDADD   = ../src/librsprod.a
testHW_exe_LDADD =

CFLAGS   += $(global_CFLAGS)
LDFLAGS  += $(global_LDFLAGS) 

CPPFLAGS  += $(list_CPPFLAGS)
LDFLAGS += $(list_LDFLAGS)
LIBS    += $(list_LIBS)
if WITH_NETCDF
CPPFLAGS  += $(netcdf_CPPFLAGS)
LDFLAGS += $(netcdf_LDFLAGS)
LIBS    += $(netcdf_LIBS)
endif
if WITH_HDF4
CPPFLAGS  += $(hdf4_CPPFLAGS)
LDFLAGS += $(hdf4_LDFLAGS)
LIBS    += $(hdf4_LIBS)
endif

