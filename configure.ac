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
    src/Makefile
])



USED_LIBS=""
USED_FLAGS=""

AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug],
                   [enable debugging, default: no]),
    [case "${enableval}" in
                 yes) USED_FLAGS="-g3 -pg -O0";;
                 no)  USED_FLAGS="-O2";;
                 *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],
    [debug=no]
)

AC_ARG_WITH([boost-lib-path],
    [AS_HELP_STRING([--with-boost-lib-path], [location of the Boost library])],
    [USED_LIBS+='-L$withval -lboost_program_options '],
    [USED_LIBS+='-lboost_program_options '])


AC_ARG_ENABLE([yuv-encoding-only],
    [AS_HELP_STRING([--enable-yuv-encoding-only], [support only YUV encoding])],
    [
        USED_FLAGS+="-DC110_DISABLE_JPEG "
    ], [
        USED_LIBS+="-ljpeg "
    ]
)

AC_SUBST([USED_LIBS])
AC_SUBST([USED_FLAGS])



AC_OUTPUT
