dnl
dnl autoconf template
dnl added by tim blechmann
dnl

AC_INIT([flext],[0.4.7],[gr@grrrr.org],[flext])
AM_INIT_AUTOMAKE(1.8)


dnl configure options
AC_ARG_ENABLE(system, [  --enable-system         flext system (default: pd)],
    [
	case $enableval in 
		max) AC_DEFINE(FLEXT_SYS,1);;
		*) AC_DEFINE(FLEXT_SYS,2);;
	esac
	],
	AC_DEFINE(FLEXT_SYS,2))

AC_ARG_ENABLE(sysdir,
	[  --enable-sysdir       path to pd or max headers],
    [
		INCLUDEDIR="-I$enableval $INCLUDEDIR"
	]
	)

AC_ARG_ENABLE(stkdir,
	[  --enable-stkdir       path to stk headers],
	[
		stkdir=$enableval
		INCLUDEDIR="-I$enableval $INCLUDEDIR"
	])

AC_ARG_ENABLE(sndobjdir,
	[  --enable-sndobjdir    path to SndObj headers],
	[
		sndobjdir=$enableval
		INCLUDEDIR="-I$enableval $INCLUDEDIR"
	])


AM_CONDITIONAL([STK],[test "$stkdir"])
AM_CONDITIONAL([SNDOBJ],[test "$sndobjdir"])

AC_ARG_ENABLE(optimize, [  --enable-optimize       enables optimized builds for: pentium4, pentium3, G4, G5],
    [
	case $enableval in
	pentium4)
		OPT_FLAGS = "-mtune=pentium4 -march=pentium4 -mmmx -msse -msse2 -mfpmath=sse";;
	pentium3)	
		OPT_FLAGS = "-mtune=pentium3 -march=pentium3 -mmmx -msse -mfpmath=sse";;
	pentium2)
		OPT_FLAGS="-mtune=pentium2 -march=pentium2 -mmmx";;
	pentium)
	 	OPT_FLAGS="-mtune=pentium -march=pentium";;
	G5)
		OPT_FLAGS="-march=G5 -mcpu=G5 -maltivec -faltivec -malign-natural";
		AC_DEFINE(FLEXT_USE_SIMD);;
	G4)
		OPT_FLAGS="-march=G4 -mcpu=G4 -maltivec -faltivec -malign-natural";
		AC_DEFINE(FLEXT_USE_SIMD);;
	G3)
		OPT_FLAGS="-mtune=G3 -mcpu=G3 -malign-natural";;
	*)
		;;
	esac
	])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl Checks for libraries.

dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

dnl Checks for library functions.

dnl system specific

if test 'uname -s' == Linux; then
	DYNAMIC_LDFLAGS = -Wl,-Bdynamic
fi

if test 'uname -s' == Darwin; then
	DYNAMIC_LDFLAGS = "-dylib -dynamic -flat_namespace -undefined suppress"
fi


AC_SUBST(INCLUDEDIR)
AC_SUBST(OPT_FLAGS)
AC_SUBST(DYNAMIC_LDFLAGS)
AC_SUBST(stkdir)
AC_SUBST(sndobjdir)
AC_OUTPUT([Makefile
		   source/Makefile])
