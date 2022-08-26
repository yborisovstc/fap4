includedir = @includedir@/fap4

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0 -pthread

lib_LTLIBRARIES = libfap4.la

libfap4_la_SOURCES = \
		       mpdes.h \
		       pconn.h \
		       pdes.h \
		       pdes.cpp

include_HEADERS = \
		  mpdes.h \
		  pconn.h \
		  pdes.h


libfap4_la_LDFLAGS = -rdynamic -no-undefined -lrt -ldl
