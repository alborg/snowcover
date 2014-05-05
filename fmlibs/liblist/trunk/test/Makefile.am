###################################################################
#
# NAME : test/Makefile.am
#
# PURPOSE :
#    Input file for Automake, liblist project.
#
# NOTE :
#    No 'make all' target is available. Use 'make check'.
#
# AUTHOR:
#    Thomas Lavergne, met.no/FoU, 16.05.2008
#
# MODIFIED:
#    Thomas Lavergne, met.no/FoU, 16.05.2008  : add test_listOfStruct.c
#    Thomas Lavergne, met.no/FoU, 16.05.2008  : add test_Sort.c
#    Thomas Lavergne, met.no/FoU, 28.05.2008  : add test_listOfComplexStructs.c
#    Thomas Lavergne, met.no/FoU, 28.05.2008  : add test_listOfLists.c
#

check_PROGRAMS = test5.exe test4.exe test3.exe test2.exe test1.exe

test5_exe_SOURCES   = test_listOfLists.c
test4_exe_SOURCES   = test_listOfComplexStructs.c
test3_exe_SOURCES   = test_Sort.c
test2_exe_SOURCES   = test_listOfStruct.c
test1_exe_SOURCES   = test_listOfNativeType.c

LDADD       = ../src/liblist.a $(LIBS) 
test1_exe_LDADD = $(LDADD) -lm

CFLAGS   += $(global_CFLAGS)
LDFLAGS  += $(global_LDFLAGS)
