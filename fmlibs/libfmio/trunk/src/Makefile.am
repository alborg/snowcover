#
# NAME:
# NA
#
# PURPOSE:
# NA
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
# Thomas Lavergne, METNO/FOU, ??
#
# MODIFIED:
# Øystein Godøy, METNO/FOU, 2011-01-21: Added fm_img2mihead.
# Øystein Godøy, METNO/FOU, 2011-11-11: Added fm_readfmdataset and
# fm_readHLHDFdata
#
# ID:
# $Id$
#

lib_LIBRARIES = libfmio.a

libfmio_a_SOURCES = fmio_config.h \
		    fmio.h \
		    fm_aha_hd.c \
		    fm_byte2float.c \
		    fm_extractstructs.c \
		    fm_init_fmio_img.c \
		    fm_MITIFF_head.c \
		    fm_MITIFF_read.c \
		    fm_MITIFF_write.c \
		    fm_readdata.c \
		    fm_readdataMETSAT.c \
		    fm_readheader.c \
		    fm_img2mihead.c \
		    fm_readfmdataset.c \
		    fm_readHLHDFdata.c


include_HEADERS =   fmio_config.h \
		    fmio.h

libfmio_a_CFLAGS = $(CFLAGS)
if FMUTIL_IS_ENABLED
libfmio_a_CFLAGS  += $(FMUTIL_CFLAGS) 
endif
if PROJ_IS_ENABLED
libfmio_a_CFLAGS  += $(PROJ_CFLAGS) 
endif
if TIFF_IS_ENABLED
libfmio_a_CFLAGS  += $(TIFF_CFLAGS) 
endif
if HDF5_IS_ENABLED
libfmio_a_CFLAGS  += $(HDF5_CFLAGS) 
endif
