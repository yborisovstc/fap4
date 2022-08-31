bin_PROGRAMS = ut-fap4-lib

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0

ut_fap4_lib_SOURCES = \
		       ut_fap4_main.cpp \
		       ut_node.cpp \
		       ut_pdes.cpp

ut_fap4_lib_LDFLAGS=-L../src/.libs/ -lfap4  -lcppunit -lrt
ut_fap4_lib_CXXFLAGS=-I@srcdir@/../src

check: test_fap4_lib

test_fap4_lib:
	 ./ut-fap4-lib


