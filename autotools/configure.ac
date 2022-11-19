#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([lolcat], [1.3], [jaseg <github@jaseg.de>])
AC_CONFIG_SRCDIR([../lolcat.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([autoscripts])
AC_CANONICAL_TARGET
AC_CANONICAL_HOST

# Automake
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB([m], [cos])

# Checks for header files.
AC_CHECK_HEADERS([locale.h stdint.h stdlib.h string.h sys/time.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday setlocale strchr strerror strstr wcwidth])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
