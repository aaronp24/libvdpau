AC_PREREQ(2.57)

AC_INIT(libvdpau, 0.1, [xorg@lists.freedesktop.org], libvdpau)
AM_INIT_AUTOMAKE([dist-bzip2 foreign])
AM_MAINTAINER_MODE

AM_CONFIG_HEADER(config.h)

# Disable static libraries by default.  Use --enable-static if you really want
# them.
AC_DISABLE_STATIC

# Check for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for dependencies.
PKG_CHECK_MODULES(X11, x11)
AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)

AC_OUTPUT([Makefile
           src/Makefile
           trace/Makefile
           vdpau.pc])
