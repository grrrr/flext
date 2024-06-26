        #
# autoconf template
# modified by Thomas Grill
#

# flext API version (current:release:age)
# API_VERSION=0:0:0

AC_INIT([flext],[0.6.3],[gr@grrrr.org])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIRS([m4])

AU_DEFUN([AC_LIBTOOL_WIN32_DLL],
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
_LT_SET_OPTION([LT_INIT], [win32-dll])
AC_DIAGNOSE([obsolete],
[$0: Remove this warning and the call to _LT_SET_OPTION when you
put the 'win32-dll' option into LT_INIT's first parameter.])
])

dnl '


FLEXT_EXT_CFLAGS=""

AC_CANONICAL_HOST
AC_LANG(C++)

AS_CASE([$host],
 [*mingw*|*cygwin*], [win=1],
 [win=""])

# configure options
AC_ARG_ENABLE(system,
    AS_HELP_STRING([--enable-system],[realtime system [default=pd]]),
    [SYSTEM=$enableval],
    [SYSTEM=pd]
)

AC_ARG_WITH(sdkdir,
  AS_HELP_STRING([--with-sdkdir],[path to pd or max headers]),
  [sdkdir=$withval],
  AC_MSG_ERROR(path to system SDK headers required $withval)
)

AC_ARG_WITH(externalsdir,
  AS_HELP_STRING([--with-externalsdir],[path to install externals to]),
  [EXTDIR="${withval}"])

AC_ARG_WITH(pdbindir,
  AS_HELP_STRING([--with-pdbindir],[path to pd bin dir]), [
  pdbindir="${withval}"
  LIBDIRS="$LIBDIRS $withval"
  AS_IF([test -z "${EXTDIR}"], EXTDIR="${withval}/../extra")
],[
  AS_IF([test -n "${win}"], AC_MSG_ERROR(path to pd bin dir required))
])

# RTE
CPPFLAGS_tmp="${CPPFLAGS}"
CPPFLAGS="${CPPFLAGS} -I${sdkdir}"
AS_CASE([$SYSTEM],
 [max], [
    AC_DEFINE(FLEXT_SYS,1)
# check for MaxAPI.h in pd folder
    AC_CHECK_HEADER([max-includes/MaxAPI.h],,AC_MSG_ERROR([Cannot find $sdkdir/max-includes/MaxAPI.h]))
    AC_CHECK_HEADER([max-includes/MaxAudioAPI.h],,AC_MSG_ERROR([Cannot find $sdkdir/max-includes/MaxAudioAPI.h]))
    INCLUDEDIRS="$INCLUDEDIRS $sdkdir/max-includes $sdkdir/msp-includes"
    pddll=""
    ],
 [pd], [
    AC_DEFINE(FLEXT_SYS,2)
# check for g_canvas.h in pd folder
    AC_CHECK_HEADER([g_canvas.h],,AC_MSG_ERROR([Cannot find $sdkdir/g_canvas.h]),[
      #include "m_pd.h"
    ])
    INCLUDEDIRS="$INCLUDEDIRS $sdkdir"
    pddll=pd
    pdfun=class_new
    ],
 [pd64], [
    AC_DEFINE(FLEXT_SYS,2)
# check for g_canvas.h in pd folder
    AC_CHECK_HEADER([g_canvas.h],,AC_MSG_ERROR([Cannot find $sdkdir/g_canvas.h]),[
      #include "m_pd.h"
    ])
    INCLUDEDIRS="$INCLUDEDIRS $sdkdir"
    pddll=pd64
    pdfun=class_new64

    AC_DEFINE([PD_FLOATSIZE], 64, [size of Pd floats (in bits)])
    FLEXT_EXT_CFLAGS="$FLEXT_EXT_CFLAGS -DPD_FLOATSIZE=64"],
 [
    AC_MSG_ERROR([system must be pd, pd64 or max])
])

LIBS_tmp="${LIBS}"
AS_IF([test -n "${pddll}" && test -n "${win}"],
  LIBS="${LIBS} -L${pdbindir}"
  AC_CHECK_LIB([:${pddll}.dll], [$pdfun], [], [AC_MSG_ERROR([Cannot find/use $pdbindir/$pddll.dll])])
  libs="$libs pddll"
  AC_DEFINE([NT])
)
CPPFLAGS="${CPPFLAGS_tmp}"
LIBS="${LIBS_tmp}"


## atomic_ops
CPPFLAGS_tmp="${CPPFLAGS}"
AC_ARG_WITH(atomic_ops,
  AS_HELP_STRING([--with-atomic_ops],[path to atomic_ops library (needed for gcc version < 4.1 on non-i386 cpus)]),
  [
    CPPFLAGS="${CPPFLAGS} -I${withval}"
    AC_CHECK_HEADER([atomic_ops.h],,AC_MSG_ERROR([Cannot find $withval/atomic_ops.h]))
    INCLUDEDIRS="$INCLUDEDIRS $withval"
    AC_DEFINE(USE_ATOMIC_OPS)
  ]
)
CPPFLAGS="${CPPFLAGS_tmp}"

## STK
CPPFLAGS_tmp="${CPPFLAGS}"
AC_ARG_WITH(stkdir,
  AS_HELP_STRING([--with-stkdir],[path to STK headers]),
  [
    CPPFLAGS="${CPPFLAGS} -I${withval}"
    AC_CHECK_HEADER([Stk.h],,AC_MSG_ERROR([Cannot find $withval/Stk.h]))
    stkdir=$withval
    INCLUDEDIRS="$INCLUDEDIRS $withval"
  ]
)
AM_CONDITIONAL([STK],[test "$stkdir"])
CPPFLAGS="${CPPFLAGS_tmp}"

## SndObj
CPPFLAGS_tmp="${CPPFLAGS}"
AC_ARG_WITH(sndobjdir,
  AS_HELP_STRING([--with-sndobjdir],[path to SndObj headers]),
  [
    CPPFLAGS="${CPPFLAGS} -I${withval}"
    AC_CHECK_HEADER([SndObj.h],,AC_MSG_ERROR([Cannot find $withval/SndObj.h]))
    sndobjdir=$withval
    INCLUDEDIRS="$INCLUDEDIRS $withval"
  ]
)
AM_CONDITIONAL([SNDOBJ],[test "$sndobjdir"])
CPPFLAGS="${CPPFLAGS_tmp}"

# if CFLAGS aren't set by the user, set them to an empty string
# otherwise AC_PROG_CC sets them to "-O2 -g"
test ".$CFLAGS" = "." && CFLAGS=" "
test ".$CXXFLAGS" = "." && CXXFLAGS=" "


AC_ENABLE_STATIC([])
AC_ENABLE_SHARED([])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
LT_INIT
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.

EXT_OS=
EXT_CPU=
EXT_EXT=so

# system specific
AS_CASE([$host],
 [*linux*|*kfreebsd*gnu*],[
    C_FLAGS="$C_FLAGS -fPIC"
    LD_FLAGS="$LD_FLAGS -shared"
    EXTENSION=pd_linux
    EXT_OS=linux
 ], [*darwin*],[
    FRAMEWORKS="$FRAMEWORKS ApplicationServices Accelerate"
    LD_FLAGS="$LD_FLAGS -flat_namespace -undefined dynamic_lookup"
    EXTENSION=pd_darwin
    EXT_OS=darwin
 ], [*mingw*|*cygwin*], [
    C_FLAGS="$C_FLAGS -mms-bitfields -mno-cygwin"
    LD_FLAGS="$LD_FLAGS -mno-cygwin"
    EXTENSION=dll
    EXT_OS=windows
    EXT_EXT=dll
 ], [ # default: like linux (but require extension via the --with-extension flag)
    C_FLAGS="$C_FLAGS -fPIC"
    LD_FLAGS="$LD_FLAGS -shared"
    EXTENSION=
])

AS_IF([test "x${EXT_OS}" = "xwindows"],[EXT_EXT=dll])

AS_CASE([$host_cpu],
  [x86_64|amd64],[EXT_CPU=amd64],
  [i386|i486|i586|i686],[EXT_CPU=i386],
  [aarch64],[EXT_CPU=arm64],
  [armv*l],[EXT_CPU=${host_cpu%l}],
  [armv*b],[EXT_CPU=${host_cpu}],
  [powerpc64],[EXT_CPU=ppc64],
  [powerpc],[EXT_CPU=ppc],
  [])

# In case of pd64, forget default extension definition
AS_IF([test "x${SYSTEM}" == "xpd64"], [EXTENSION=])
# A new extension format .${os}_${arch}_${floatsize}.${sysext} is needed,
# which (for now) must be explicitly given through the --with-extension option

AS_IF([test "x${SYSTEM}" = "xpd64" && test "x${EXT_CPU}" != "x" && test "x${EXT_OS}" != "x" && test "x${EXT_EXT}" != "x"],[
  EXTENSION="${EXT_OS}-${EXT_CPU}-64.${EXT_EXT}"
  AC_MSG_NOTICE([default external extension...${EXTENSION}])
])

# Explicit extension definition
AC_ARG_WITH([extension],
 AS_HELP_STRING([--with-extension],[use the extension given instead of the platform-specific default]),
 AS_CASE([$withval], [yes|no|""], [], [EXTENSION=$withval]))

AC_MSG_CHECKING([extension])
AS_IF([test -z "${EXTENSION}"], AC_MSG_ERROR([Unable to autodetect extension and none given... (pd64 system currently requires --with-extension)]))
AC_MSG_RESULT([${EXTENSION}])

AS_IF([test -z "${EXTDIR}"], EXTDIR="/usr/local/lib/pd-externals")


# set compilation flags

OPT_FLAGS="$C_FLAGS -O3"
DBG_FLAGS="$C_FLAGS -DFLEXT_DEBUG -g"

AC_ARG_ENABLE([cmem],
  AS_HELP_STRING([--enable-cmem],[enables C-style memory allocation (as opposed to overloading 'new' and 'delete')]),
  AS_CASE([$enableval], [yes], [
    AC_DEFINE(FLEXT_USE_CMEM)
    FLEXT_EXT_CFLAGS="$FLEXT_EXT_CFLAGS -DFLEXT_USE_CMEM"])
)


AC_SUBST(SYSTEM)
AC_SUBST(INCLUDEDIRS)
AC_SUBST(LIBDIRS)
AC_SUBST(OPT_FLAGS)
AC_SUBST(DBG_FLAGS)
AC_SUBST(LD_FLAGS)
# AC_SUBST(API_VERSION)
AC_SUBST(libs)
AC_SUBST(stkdir)
AC_SUBST(sndobjdir)
AC_SUBST(FRAMEWORKS)
AC_SUBST(FLEXT_EXT_CFLAGS)

AC_SUBST(EXTENSION)
AC_SUBST(EXTDIR)

AC_CONFIG_FILES([
  $SYSTEM-flext.pc
  Makefile
  source/Makefile
  tutorial/Makefile
  tutorial/1_simple1/Makefile
  tutorial/1_simple2/Makefile
  tutorial/1_simple3/Makefile
  tutorial/2_adv1/Makefile
  tutorial/2_adv2/Makefile
  tutorial/2_adv3/Makefile
  tutorial/3_attr1/Makefile
  tutorial/3_attr2/Makefile
  tutorial/3_attr3/Makefile
  tutorial/4_bind1/Makefile
  tutorial/4_buffer1/Makefile
  tutorial/4_timer1/Makefile
  tutorial/5_signal1/Makefile
  tutorial/5_signal2/Makefile
  tutorial/6_lib1/Makefile
  tutorial/7_thread1/Makefile
  tutorial/7_thread2/Makefile
  tutorial/8_sndobj1/Makefile
  tutorial/8_stk1/Makefile
  tutorial/8_stk2/Makefile
  tutorial/pd/Makefile
  tutorial/maxmsp/Makefile
])
AC_OUTPUT
