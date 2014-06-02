#
# NAME:
# Makefile.in
#
# PURPOSE:
# Makefile
#
# REQUIREMENTS:
# NA
#
# INPUT:
# NA
#
# OUTPUT:
# NA
#
# NOTES:
# o make - builds library
# o make install - installs libraryin LIBBASE (set below)
# o make clean - removes object and archive files from src directory
# o make distclean - performs make clean and removes installed parts
# o make tarball - creates a tarball of library (does not work yet)
#
# BUGS:
# NA
#
# AUTHOR:
# Øystein Godøy, METNO/FOU, 17.09.2007 
#
# MODIFIED:
# NA
#
# CVS_ID:
# $Id: Makefile.in,v 1.7 2013-02-01 08:49:31 mariak Exp $
#

SHELL = /bin/sh

srcdir = .
prefix = /home/anettelb/cpp
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
incdir = ${prefix}/include
libdir = ${exec_prefix}/lib

CFLAGS = -g -O2 -DPACKAGE_NAME=\"METNO\ FMSNOWCOVER\" -DPACKAGE_TARNAME=\"metno-fmsnowcover\" -DPACKAGE_VERSION=\"0.1\" -DPACKAGE_STRING=\"METNO\ FMSNOWCOVER\ 0.1\" -DPACKAGE_BUGREPORT=\"o.godoy@met.no\" -DPACKAGE_URL=\"\" -DFMSNOWCOVER_HAVE_LIBPROJ=1 -DFMSNOWCOVER_HAVE_LIBHDF5=1 -DFMSNOWCOVER_HAVE_LIBMI=1 -DFMSNOWCOVER_HAVE_LIBUSENWP=1 -DFMSNOWCOVER_HAVE_LIBOSIHDF5=1 -DFMSNOWCOVER_HAVE_LIBTIFF=1 -DFMSNOWCOVER_HAVE_LIBFMUTIL=1 -DFMSNOWCOVER_HAVE_LIBFMIO=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDLIB_H=1 -DHAVE_MALLOC=1 -I./

CPPFLAGS =  -I/home/anettelb/cpp/include -I/home/anettelb/cpp/include -I/home/anettelb/cpp/include -I/usr/include -I/home/anettelb/cpp/include -I/home/anettelb/cpp/include

LDFLAGS =  -L/home/anettelb/cpp/lib -L/home/anettelb/cpp/lib -L/home/anettelb/cpp/lib -L/usr/lib -L/home/anettelb/cpp/lib -L/home/anettelb/cpp/lib

LIBS = -Wl,-rpath=/home/anettelb/cpp/lib -lfmio -Wl,-rpath=/home/anettelb/cpp/lib -lfmutil -Wl,-rpath=/usr/lib -ltiff -Wl,-rpath=/home/anettelb/cpp/lib -losihdf5 -Wl,-rpath=/home/anettelb/cpp/lib -lusenwp -lmi -lhdf5 -Wl,-rpath=/home/anettelb/cpp/lib -lproj  -lgfortran

HEADER_FILES1 = \
  fmsnowcover.h \
  fmaccusnow.h \
  getnwp.h
SRC_FILES1 = \
  fmsnowcover.c \
  fmsnowcover_swath.c \
  pix_proc.c \
  pix_proc_swath.c \
  probest.c \
  normalpdf.c \
  getnwp.c \
  gammapdf.c \
  store_snow.c	

HEADER_FILES2 = \
  fmaccusnow.h 
SRC_FILES2 = \
  fmaccusnow.c \
  store_snow.c \
  fmaccusnowfuncs.c 

AUTOMATED_FILES = \
  Makefile

.SUFFIXES:
.SUFFIXES: .c .o

.PHONY: clean install distclean

BINFILE1 = fmsnowcover

BINFILE2 = fmaccusnow

OBJ_FILES1 := $(SRC_FILES1:.c=.o)

OBJ_FILES2 := $(SRC_FILES2:.c=.o)

all: $(BINFILE1) $(BINFILE2)  

$(BINFILE1): $(OBJ_FILES1) 
	$(CC) $(CFLAGS) -o $(BINFILE1) $^ $(LDFLAGS) $(LIBS)

$(BINFILE2): $(OBJ_FILES2) 
	$(CC) $(CFLAGS) -o $(BINFILE2) $^ $(LDFLAGS) $(LIBS)

$(OBJ_FILES1): $(HEADER_FILES1)

$(OBJ_FILES2): $(HEADER_FILES2)

clean:
	find $(srcdir) -name "*.o" -exec rm -f {} \;
	find $(srcdir) -name "*.a" -exec rm -f {} \;

distclean:
	$(MAKE) clean
	rm -rf $(AUTOMATED_FILES)
	rm -rf $(BINFILE1) $(BINFILE2)

install:
	install -d $(incdir)
	install --mode=644 $(HEADER_FILES1) $(incdir)
	install -d $(bindir)
	install --mode=755 $(BINFILE1) $(BINFILE2) $(bindir)
