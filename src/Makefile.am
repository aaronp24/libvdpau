AM_CFLAGS = \
    -I$(top_srcdir)/include \
    -DVDPAU_MODULEDIR="\"$(moduledir)\"" \
    $(X11_CFLAGS) \
    $(XEXT_CFLAGS)

lib_LTLIBRARIES = libvdpau.la

libvdpau_la_SOURCES = \
    vdpau_wrapper.c \
    $(DRI2_SOURCES)

if DRI2
    DRI2_SOURCES = \
        mesa_dri2.c \
        mesa_dri2.h
endif

libvdpau_la_LIBADD = \
    $(DLOPEN_LIBS) \
    $(XEXT_LIBS)

libvdpau_la_LDFLAGS = -version-info 1:0:0 -no-undefined

libvdpauincludedir = $(includedir)/vdpau
libvdpauinclude_HEADERS = \
    $(top_srcdir)/include/vdpau/vdpau.h \
    $(top_srcdir)/include/vdpau/vdpau_x11.h
