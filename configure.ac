AC_INIT([c110d], [0.1], [bug-report@minecamp])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])

PKG_CHECK_MODULES([DEPS], [
    libusb-1.0 >= 1.0.0
    xcb >= 1.9
    xcb-image >= 0.3.9
    xcb-shm >= 1.9
])

AC_CONFIG_FILES([
    Makefile
])



AC_ARG_WITH([boost-lib-path],
    [AS_HELP_STRING([--with-boost-lib-path], [location of the Boost library])],
    [BOOST_LIBS="-L$withval -lboost_program_options"],
    [BOOST_LIBS='-lboost_program_options'])
AC_SUBST([BOOST_LIBS])


AC_ARG_ENABLE([yuv-encoding-only],
    [AS_HELP_STRING([--enable-yuv-encoding-only], [support only YUV encoding])],
    [
        JPEG_LIBS="-ljpeg"
        JPEG_FLAGS="-DC110_DISABLE_JPEG"
    ], [
        JPEG_LIBS=""
        JPEG_FLAGS=""
    ]
)
AC_SUBST([JPEG_LIBS])
AC_SUBST([JPEG_FLAGS])



AC_OUTPUT
