AUTOMAKE_OPTIONS = serial-tests

AM_CFLAGS = -Wall $(LIBOSMOCORE_CFLAGS) -I$(top_builddir)/include

check_PROGRAMS = conv_test turbo_test
conv_test_SOURCES = conv_test.c noise.c codes.c
conv_test_LDADD = $(top_builddir)/src/libturbofec.la $(LIBOSMOCORE_LIBS) -lpthread -lm

turbo_test_SOURCES = turbo_test.c noise.c
turbo_test_LDADD = $(top_builddir)/src/libturbofec.la $(LIBOSMOCORE_LIBS) -lpthread -lm

noinst_HEADERS = \
	noise.h \
	codes.h

TESTS = $(check_PROGRAMS)
