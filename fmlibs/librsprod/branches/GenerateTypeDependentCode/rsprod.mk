
include $(top_srcdir)/mk/install.mk

# pkg-config stuff
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = $(pkgname).pc
EXTRA_DIST += $(pkgname).pc.in
