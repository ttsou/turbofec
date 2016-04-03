ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = turbofec.pc

AUTOMAKE_OPTIONS = foreign dist-bzip2 1.6

SUBDIRS = include src tests

INCLUDE_FILES=$(wildcard include/turbofec/*.h)
SRC=$(wildcard src/*.c)
TEST_SRC=$(wildcard tests/*.c)
ALL_SRC=$(SRC) $(TEST_SRC) $(INCLUDE_FILES)

tags: $(ALL_SRC)
	@echo "Creating tags for source files.."
	@ctags $(ALL_SRC)
	@echo "Done!"
