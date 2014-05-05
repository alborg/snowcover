# 
# TYPE:
# Makefile
#
# PURPOSE:
# To create libfmutil. libfmutil is a library containing utility functions
# useful for processing remote sensing data, but also for other purposes.
# libfmutil is based upon the libsatimg library, but in order to avoid the
# KSPT MEOS libraries which are only available in binary format at the
# Norwegian Meteorological Institute, the information contained within
# libsatimg, libfmcoord and libfmsubtrack is reorganized into two
# libraries - libfmutil and libfmio, the first being a prerequisite of the
# latter. The only accepted prerequisite of libfmutil is PROJ.4 which
# ensures a standard map reference system.
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
# Øystein Godøy, met.no/FOU, 07.01.2005 
#
# MODIFICATION:
# NA
#

LIBBASE = ../

TARBALL = $(LIBBASE)/../libfmutil.tgz

DOCDIR = $(LIBBASE)/doc
INCDIR = $(LIBBASE)/include
LIBDIR = $(LIBBASE)/lib

PROJINC = ../../libproj/include

LDFLAGS = 

OPT = -Wall -g

CFLAGS = \
	-I./ \
	-I$(PROJINC) \
	$(OPT)

HEADER_FILES = \
  fmutil.h \
  fmcoord.h \
  fmtime.h \
  fmstrings.h \
  fmangles.h \
  fmsolar.h \
  fmcolormaps.h \
  fmbyteswap.h \
  fmstorage.h \
  fmimage.h

SOURCE_FILES = \
  fmerrmsg.c \
  fmtime.c \
  fmcoord.c \
  fmangles.c \
  fmsubtrack.c \
  fmstrings.c \
  fmsolar.c \
  fmcolormaps.c \
  fmangleconversion.c \
  fmivec.c \
  fmbyteswap.c \
  fmstorage.c \
  fmimage.c

OBJECT_FILES := $(patsubst %.c,%.o,$(SOURCE_FILES))

LIBRARY = libfmutil.a

.PHONY = distclean clean build tarball

$(LIBRARY): $(OBJECT_FILES) $(HEADER_FILES)
	$(AR) $(ARFLAGS) $(LIBRARY) $(OBJECT_FILES)

distclean:
	$(MAKE) clean
	if [ -d $(LIBBASE)/include ]; then rm -rf $(LIBBASE)/include; fi
	if [ -d $(LIBBASE)/lib ]; then rm -rf $(LIBBASE)/lib; fi

clean:
	find  -name "*.o" -exec rm -f {} \;
	find  -name "*.a" -exec rm -f {} \;

build:
	$(MAKE)
	$(MAKE) clean

# The following does not work yet...
tarball:
	tar cvzf $(TARBALL) ../../libfmutil

install:
	install -d $(LIBBASE)/include
	install $(HEADER_FILES) $(INCDIR)
	install -d $(LIBBASE)/lib
	install $(LIBRARY) $(LIBDIR)
