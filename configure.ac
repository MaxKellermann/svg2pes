AC_PREREQ(2.60)
AC_INIT(svg2pes, 0.1, max.kellermann@gmail.com)
AC_CONFIG_SRCDIR(src/Main.cxx)
AC_CONFIG_AUX_DIR(build)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign 1.11 subdir-objects dist-xz])
AM_SILENT_RULES

dnl Checks for programs.

AC_PROG_CXX
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

dnl Checks for OS features

AC_SYS_LARGEFILE

dnl
dnl Checks for libraries
dnl

PKG_CHECK_MODULES([EXPAT], [expat],,
	[AC_MSG_ERROR([expat not found])])

dnl
dnl CFLAGS
dnl

AC_SUBST(AM_CXXFLAGS)
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_LDFLAGS)

AM_CXXFLAGS="-std=c++14 -fno-threadsafe-statics -fmerge-all-constants"
AM_CPPFLAGS="-D_REENTRANT -D_GNU_SOURCE"
AM_LDFLAGS="-Wl,--as-needed"

AM_CXXFLAGS="$AM_CXXFLAGS -Wall -Wextra"
AM_CXXFLAGS="$AM_CXXFLAGS -Wwrite-strings -Wcast-qual -Wcast-align -Wfloat-equal"
AM_CXXFLAGS="$AM_CXXFLAGS -Wshadow -Wpointer-arith -Wsign-compare"
AM_CXXFLAGS="$AM_CXXFLAGS -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute"
AM_CXXFLAGS="$AM_CXXFLAGS -Wredundant-decls -Wno-long-long -Wundef"
AM_CXXFLAGS="$AM_CXXFLAGS -Wno-variadic-macros"

dnl
dnl DEBUG
dnl

AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--disable-debug],
		[Disable debugging (default: enabled)]),,
	enable_debug=yes)

if test x$enable_debug != xyes; then
	AM_CPPFLAGS="$AM_CPPFLAGS -DNDEBUG"
	AM_CXXFLAGS="$AM_CXXFLAGS -ffunction-sections -fdata-sections"
	AM_LDFLAGS="$AM_LDFLAGS -Wl,-gc-sections"
fi

AC_ARG_ENABLE(werror,
	AS_HELP_STRING([--enable-werror],
		[treat warnings as errors]),,
	enable_werror=$enable_debug)

if test x$enable_werror = xyes; then
	AM_CXXFLAGS="$AM_CXXFLAGS -Werror"
fi

dnl
dnl Output files
dnl

AC_OUTPUT(Makefile)
