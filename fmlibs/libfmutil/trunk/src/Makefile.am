#
# NAME:
# Makefile.am
#
# PURPOSE:
# To be processed by automake to Makefile.
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
# NA
#
# BUGS:
# NA
#
# AUTHOR:
# Thomas Lavergne
#
# MODIFIED:
# Øystein Godøy, METNO/FOU, 06.09.2007: Added header and fmtouch specific
# statements.
# Øystein Godøy, METNO/FOU, 16.10.2007: Added fmfeltfile.
# Øystein Godøy, METNO/FOU, 22.02.2008: Added fmselalg.
#
# CVS_ID:
# $Id: Makefile.am,v 1.10 2010-11-09 14:20:11 thomasl Exp $
#
#

BUILT_SOURCES = fmutil.h


ALLSUBHEADERS = fmutil_config.h fmangleconversion.h fmangles.h fmbyteswap.h \
		fmcolormaps.h fmerrmsg.h fmimage.h fmsolar.h fmstorage.h \
		fmstrings.h fmtime.h fmutil_types.h fm_ch3b_reflectance.h \
		fmcoord.h 

lib_LIBRARIES = libfmutil.a

libfmutil_a_SOURCES = fmutil.h \
		      $(ALLSUBHEADERS) \
		      fmangleconversion.c \
		      fmangles.c \
		      fmbyteswap.c \
		      fmcolormaps.c \
		      fmerrmsg.c \
		      fmstrings.c \
		      fmtime.c \
		      fmimage.c \
		      fmsolar.c \
		      fmstorage.c \
		      fm_ch3b_reflectance.c \
		      fmtouch.c \
		      fmfeltfile.c \
		      fmfilesystem.c 

include_HEADERS =     fmutil.h 

libfmutil_a_CFLAGS = $(CFLAGS) 
libfmutil_a_CFLAGS += $(global_CFLAGS) 
if PROJ_IS_ENABLED
libfmutil_a_CFLAGS  += $(PROJ_CFLAGS) 
libfmutil_a_SOURCES += fmselalg.c fmcoord.c fmcoord.h fmsubtrack.c
endif

fmutil.h : $(ALLSUBHEADERS)
	./prepare_fmutil_h.sh

clean-local:
	rm -f fmutil.h

