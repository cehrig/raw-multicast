#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Socket-Raw Multicast], [1.0.0], [christian@ehrig.io])
AC_CONFIG_SRCDIR([src/rmc.c])
AC_CONFIG_HEADERS([src/config.h])
AM_INIT_AUTOMAKE

# Custom Config header Definitions
AC_DEFINE([RMC_POLL_TIMEOUT], [2000],
            [Number of Milliseconds that poll should block waiting for a file
            desriptor to become ready...])

AC_DEFINE([RMC_OUT_BUFFER], [1024],
            [Number of bytes to read when new client connects])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_SEARCH_LIBS([pthread_create], [pthread])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT