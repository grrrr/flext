dnl
dnl autoconf template
dnl added by tim blechmann
dnl

dnl flext API version (current:release:age)
API_VERSION=0:0:0

AC_INIT([flext],[0.4.7],[gr@grrrr.org],[flext])
AM_INIT_AUTOMAKE(1.6)


dnl configure options
AC_ARG_ENABLE(system, [  --enable-system         flext system (default: pd)],
    [
	case $enableval in 
		max) AC_DEFINE(FLEXT_SYS,1);;
		*) AC_DEFINE(FLEXT_SYS,2);;
	esac
	system = $enableval
	],
	AC_DEFINE(FLEXT_SYS,2))

AC_ARG_WITH(sysdir,
	[  --with-sysdir           path to pd or max headers],
    	[
		sysdir=$withval
		INCLUDEDIR="-I$withval $INCLUDEDIR"
	],
	[echo "path to system headers required "$enableval && exit 1])

dnl check for g_canvas.h in pd folder
AC_CHECK_FILE("$sysdir/g_canvas.h",,[echo "$sysdir/g_canvas.h not found" && exit 1])

AC_ARG_WITH(stkdir,
	[  --with-stkdir           path to stk headers],
	[
	    AC_CHECK_FILE("$withval/Stk.h",,[echo "$withval/Stk.h not found" && exit 1])	
		stkdir=$withval
		INCLUDEDIR="-I$withval $INCLUDEDIR"
	])

AM_CONDITIONAL([STK],[test "$stkdir"])

AC_ARG_WITH(sndobjdir,
	[  --with-sndobjdir      path to SndObj headers],
	[
		AC_CHECK_FILE("$withval/SndObj.h",,[echo "$withval/SndObj.h not found" && exit 1])
		sndobjdir=$withval
		INCLUDEDIR="-I$withval $INCLUDEDIR"
	])

AM_CONDITIONAL([SNDOBJ],[test "$sndobjdir"])

AC_ARG_ENABLE(optimize, [  --enable-optimize       enables optimized builds for: pentium4, pentium3, G4, G5],
    [
	case $enableval in
	pentium3 | pentium3m)
		OPT_FLAGS = "-mtune=$enableval -march=$enableval -mmmx -msse -mfpmath=sse";
		AC_DEFINE(FLEXT_USE_SIMD);;
	pentium2 | athlon | pentium-mmx)
		OPT_FLAGS="-mtune=$enableval -march=$enableval -mmmx";;
	pentium)
	 	OPT_FLAGS="-mtune=$enableval -march=$enableval";;
	pentium4 | pentium4m | pentium-m | prescott | nocona | athlon-xp | athlon-mp | athlon64 | opteron) 
		OPT_FLAGS="-mtune=$enableval -march=$enableval -mmmx -msse -msse2 -mfpmath=sse";
		AC_DEFINE(FLEXT_USE_SIMD);;
	G5 | G4)
		OPT_FLAGS="-mtune=$enableval -march=$enableval -maltivec -faltivec -malign-natural";
		AC_DEFINE(FLEXT_USE_SIMD);;
	G3)
		OPT_FLAGS="-mtune=$enableval -march=$enableval -malign-natural";;
	*)
		;;
	esac
	])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl Checks for libraries.

dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

dnl Checks for library functions.

dnl system specific

if test $(uname -s) == Linux ; then
	DYNAMIC_LDFLAGS="-Wl,-Bdynamic"
	PDEXT=pd_linux
	SOEXT=so
fi
if test $(uname -s) == Darwin ; then
	DYNAMIC_LDFLAGS="-version-info $API_VERSION -release $AC_PACKAGE_VERSION -dylib -dynamic -flat_namespace -undefined suppress"

	if test $system == max ; then
		PDEXT=mxo
		SOEXT=dylib
		FRAMEWORKS=vecLib maxlib maxaudio
	else	
		PDEXT=pd_darwin
		SOEXT=dylib
		FRAMEWORKS=vecLib
	fi
fi

dnl else
dnl    echo "Platform NOT supported"
dnl    exit 1
dnl fi

dnl todo: mingw / cygwin

AC_SUBST(INCLUDEDIR)
AC_SUBST(OPT_FLAGS)
AC_SUBST(DYNAMIC_LDFLAGS)
AC_SUBST(stkdir)
AC_SUBST(sndobjdir)
AC_SUBST(PDEXT)
AC_SUBST(SOEXT)
AC_SUBST(FRAMEWORKS) dnl for osx
AC_OUTPUT([
			Makefile
			tutorial/Makefile
			source/Makefile])
