# 
#  NAME:
#  Makefile.in
#
#  PURPOSE:
#  Input file to configure for creation of Makefile.
#
#  REQUIREMENTS:
#  NA
#
#  INPUT:
#  NA
#
#  OUTPUT:
#  NA
#
#  NOTES:
#  NA
#
#  BUGS:
#  NA
#
#  AUTHOR:
#  Øystein Godøy, METNO/FOU, 20.09.2006 
#
#  MODIFIED:
#  Øystein Godøy, METNO/FOU, 06.09.2007: Added tarball feature.
#
#  CVS_ID:
#  $Id: Makefile.in,v 1.1.1.1 2009-02-13 23:23:13 steingod Exp $
#   

SHELL = /bin/sh

srcdir = .
prefix = /home/anettelb/cpp
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
incdir = ${prefix}/include
libdir = ${exec_prefix}/lib

CFLAGS = -g -O2

CPPFLAGS =  -I/home/anettelb/cpp/include -I/home/anettelb/cpp/include -I/home/anettelb/cpp/include -I/usr/include -I/home/anettelb/cpp/include -I/home/anettelb/cpp/include

LDFLAGS =  -L/home/anettelb/cpp/lib -L/home/anettelb/cpp/lib -L/home/anettelb/cpp/lib -L/usr/lib -L/home/anettelb/cpp/lib -L/home/anettelb/cpp/lib

LIBS = -Wl,-rpath=/home/anettelb/cpp/lib -lfmio -Wl,-rpath=/home/anettelb/cpp/lib -lfmutil -Wl,-rpath=/usr/lib -ltiff -Wl,-rpath=/home/anettelb/cpp/lib -losihdf5 -Wl,-rpath=/home/anettelb/cpp/lib -lusenwp -lmi -lhdf5 -Wl,-rpath=/home/anettelb/cpp/lib -lproj  -lgfortran

SUBDIRS = \
   src 

AUTOMATED_FILES = \
   config.status \
   config.log \
   Makefile \
   autom4te.cache

.PHONY = all install clean distclean $(SUBDIRS)
 
all: $(SUBDIRS)
	@for dir in $(SUBDIRS); do \
	  echo ""; \
	  echo ""; \
	  echo "==== Compiling directory $$dir ====="; \
	  $(MAKE) -C $$dir || exit 1; \
	done

install:
	@for dir in $(SUBDIRS); do \
	  echo ""; \
	  echo ""; \
	  echo "==== Installing from directory $$dir ====="; \
	  $(MAKE) -C $$dir install || exit 1; \
	done

clean:
	@for dir in $(SUBDIRS); do \
	  echo ""; \
	  echo ""; \
	  echo "==== Cleaning directory $$dir ====="; \
	  $(MAKE) -C $$dir clean || exit 1; \
	done

distclean:
	rm -rf $(AUTOMATED_FILES)
	@for dir in $(SUBDIRS); do \
	  echo ""; \
	  echo ""; \
	  echo "==== Distcleaning directory $$dir ====="; \
	  $(MAKE) -C $$dir distclean || exit 1; \
	done

tarball:
	$(MAKE) distclean
	@echo "==== Creating tarball =====";
	@echo $(CURDIR) | sed -e 's/\/.*\///' -e 's/.*/&\//'
	tar -cvzf ../metno-fmsnowcover-0.1.tgz `echo $(CURDIR) | s ed -e 's/\/.*\///' -e 's/.*/..\/&\//'`

