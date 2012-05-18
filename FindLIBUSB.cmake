# - Try to find LIBUSB
# Once done this will define
#
#  LIBUSB_FOUND - system has LIBUSB
#  LIBUSB_INCLUDE_DIRS - the LIBUSB include directory
#  LIBUSB_LIBRARIES - Link these to use LIBUSB
#  LIBUSB_DEFINITIONS - Compiler switches required for using LIBUSB
#
#  Copyright (c) 2011 Reto Schneider <reto <`)))>< reto-schneider.ch>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (LIBUSB_LIBRARIES AND LIBUSB_INCLUDE_DIRS)
  # in cache already
  set(LIBUSB_FOUND TRUE)
else (LIBUSB_LIBRARIES AND LIBUSB_INCLUDE_DIRS)
  find_path(LIBUSB_INCLUDE_DIR
    NAMES
      libusb-1.0/libusb.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  find_library(USB-1.0_LIBRARY
    NAMES
      usb-1.0
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  set(LIBUSB_INCLUDE_DIRS
    ${LIBUSB_INCLUDE_DIR}
  )
  set(LIBUSB_LIBRARIES
    ${USB-1.0_LIBRARY}
)

  if (LIBUSB_INCLUDE_DIRS AND LIBUSB_LIBRARIES)
     set(LIBUSB_FOUND TRUE)
  endif (LIBUSB_INCLUDE_DIRS AND LIBUSB_LIBRARIES)

  if (LIBUSB_FOUND)
    if (NOT LIBUSB_FIND_QUIETLY)
      message(STATUS "Found LIBUSB: ${LIBUSB_LIBRARIES}")
    endif (NOT LIBUSB_FIND_QUIETLY)
  else (LIBUSB_FOUND)
    if (LIBUSB_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find LIBUSB")
    endif (LIBUSB_FIND_REQUIRED)
  endif (LIBUSB_FOUND)

  # show the LIBUSB_INCLUDE_DIRS and LIBUSB_LIBRARIES variables only in the advanced view
  mark_as_advanced(LIBUSB_INCLUDE_DIRS LIBUSB_LIBRARIES)

endif (LIBUSB_LIBRARIES AND LIBUSB_INCLUDE_DIRS)

