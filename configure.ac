AC_INIT([turbofec],
	[0.1],
	[tom@tsou.cc])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([dist-bzip2])
LT_INIT

dnl kernel style compile messages
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_PROG_AS
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_INSTALL
AC_HEADER_STDC

dnl checks for processor family 
AX_EXT

AC_OUTPUT(
    turbofec.pc
    include/Makefile
    src/Makefile
    tests/Makefile
    Makefile)
