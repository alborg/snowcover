# 
# TYPE:
# Makefile
#
# PURPOSE:
# To create libcollocate.
#
# NOTES:
# NA
#
# BUGS:
# NA
#
# AUTHOR:
# Øystein Godøy, DNMI/FOU, 02/09/2002

# How to build this library
LIBBASE = ../
SRCDIR = $(LIBBASE)/src
INCDIR = $(LIBBASE)/include
LIBDIR = $(LIBBASE)/lib

# Where to find auxiliaries
COMMONPATH = /home/steingod/software/libs
MEOSINC = $(COMMONPATH)/libmeos/include
SATIMGINC = $(COMMONPATH)/libsatimg/include
FMCOORDINC = $(COMMONPATH)/libfmcoord/include
FMSUBTRACKINC = $(COMMONPATH)/libfmsubtrack/include
USENWPINC = $(COMMONPATH)/libusenwp/include
SAFNWCCMINC = /disk1/acpg-0p56/include
OSIHDFINC = $(HOME)/software/osisaf/v3p1/OSI_HL_AUX/OSI_HL_IO/libosihdf5/include
HLHDFINC = /disk1/hlhdf/include
HDF5INC = /disk1/hdf5lib/include
PROJINC = $(COMMONPATH)/libproj/include

OPT = -g

CFLAGS = \
  -I./ \
  -I$(SATIMGINC) \
  -I$(FMCOORDINC) \
  -I$(FMSUBTRACKINC) \
  -I$(MEOSINC) \
  -I$(USENWPINC) \
  -I$(SAFNWCCMINC) \
  -I$(OSIHDFINC) \
  -I$(HLHDFINC) \
  -I$(HDF5INC) \
  -I$(PROJINC) \
  $(OPT)

OBJS = \
    $(SRCDIR)/decode_cfg.o \
    $(SRCDIR)/error.o \
    $(SRCDIR)/avhrr_stdat.o \
    $(SRCDIR)/check_avhrr.o \
    $(SRCDIR)/check_safct.o \
    $(SRCDIR)/nwp_stdat.o \
    $(SRCDIR)/nwp_read.o \
    $(SRCDIR)/std_stdat.o \
    $(SRCDIR)/safcm_stdat.o \
    $(SRCDIR)/safssi_stdat.o \
    $(SRCDIR)/hdfaccess.o 
  
HEADS = \
    $(SRCDIR)/collocate1.h \
    $(SRCDIR)/avhrr_stdat.h \
    $(SRCDIR)/nwp_stdat.h \
    $(SRCDIR)/nwp_read.h \
    $(SRCDIR)/std_stdat.h \
    $(SRCDIR)/safcm_stdat.h \
    $(SRCDIR)/safssi_stdat.h \
    $(SRCDIR)/hdfaccess.h

LIB = libfmcol.a

$(LIB): $(OBJS) $(HEADS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

rambo:
	find $(SRCDIR) -name "*.o" -exec rm {} \;
	find $(SRCDIR) -name "*.a" -exec rm {} \;

distclean:
	$(MAKE) rambo
	if [ -d $(LIBBASE)/include ]; then rm -rf $(LIBBASE)/include; fi
	if [ -d $(LIBBASE)/lib ]; then rm -rf $(LIBBASE)/lib; fi
	$(MAKE) rambo

clean:
	find $(SRCDIR) -name "*.o" -exec rm {} \;

install:
	install -d $(LIBBASE)/include
	install $(HEADS) $(INCDIR)
	install -d $(LIBBASE)/lib
	install $(LIB) $(LIBDIR)
