includedir = @includedir@/fap4

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0 -pthread

lib_LTLIBRARIES = libfap4.la

libfap4_la_SOURCES = \
		       miface.h \
		       mpdes.h \
		       mnode.h \
		       mowning.h \
		       pconn.h \
		       nconn.h \
		       guri.h \
		       guri.cpp \
		       node.h \
		       node.cpp \
		       pdes.h \
		       pdes.cpp

include_HEADERS = \
		  miface.h \
		  mpdes.h \
		  mnode.h \
		  mowning.h \
		  node.h \
		  pconn.h \
		  nconn.h \
		  pdes.h


libfap4_la_LDFLAGS = -rdynamic -no-undefined -lrt -ldl
