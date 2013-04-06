AM_CPPFLAGS = $(DEPS_CFLAGS) $(JPEG_FLAGS) -std=c++11 -I../include -g -pg

bin_PROGRAMS = c110d
c110d_SOURCES = daemon.cpp projector.cpp screenshotXShm.cpp
c110d_LDADD = $(DEPS_LIBS) $(BOOST_LIBS) $(JPEG_LIBS)

