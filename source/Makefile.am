#
# automake template
# added by tim blechmann
#

lib_LIBRARIES = libflext.a libflext_d.a libflext_t.a libflext_td.a
lib_LTLIBRARIES = libflext.la libflext_d.la

SRCS_FLEXT = \
	flbase.cpp \
	flext.cpp \
	flbuf.cpp \
	fldsp.cpp \
	fllib.cpp \
	flxlet.cpp \
	flattr.cpp \
	flattr_ed.cpp \
	flsupport.cpp \
	flutil.cpp \
	flatom.cpp \
	flatom_pr.cpp \
	flthr.cpp \
	fltimer.cpp \
	flsimd.cpp \
	flout.cpp \
	flatom_app.cpp \
	flatom_part.cpp \
	flitem.cpp \
	flmeth.cpp \
	flmsg.cpp \
	flproxy.cpp \
	flqueue.cpp \
	flbind.cpp

HDRS_FLEXT = \
	flprefix.h \
	flstdc.h \
	flbase.h \
	flclass.h \
	flext.h \
	flsupport.h \
	flmap.h \
	fldsp.h \
	flinternal.h \
	fldefs.h \
	fldefs_hdr.h \
	fldefs_setup.h \
	fldefs_methcb.h \
	fldefs_meththr.h \
	fldefs_methadd.h \
	fldefs_methbind.h \
	fldefs_methcall.h \
	fldefs_attrcb.h \
	fldefs_attrvar.h \
	fldefs_attradd.h


#handling for stk / sndobj
if SNDOBJ
SRCS_SNDOBJ = flsndobj.cpp
HDRS_SNDOBJ = flsndobj.h
LIB_SNDOBJ  = -lsndobj
endif

if STK
SRCS_STK = flstk.cpp
HDRS_STK = flstk.h
LIB_STK  =  -lstk
endif

pkginclude_HEADERS = $(HDRS_FLEXT) $(HDRS_SNDOBJ) $(HDRS_STK)

libflext_a_SOURCES    = $(SRCS_FLEXT) $(SRCS_SNDOBJ) $(SRCS_STK)
libflext_d_a_SOURCES  = $(SRCS_FLEXT) $(SRCS_SNDOBJ) $(SRCS_STK)
libflext_t_a_SOURCES  = $(SRCS_FLEXT) $(SRCS_SNDOBJ) $(SRCS_STK)
libflext_td_a_SOURCES = $(SRCS_FLEXT) $(SRCS_SNDOBJ) $(SRCS_STK)
libflext_la_SOURCES   = $(SRCS_FLEXT) $(SRCS_SNDOBJ) $(SRCS_STK)
libflext_d_la_SOURCES = $(SRCS_FLEXT) $(SRCS_SNDOBJ) $(SRCS_STK)

libflext_a_CXXFLAGS    = @OPT_FLAGS@ @INCLUDEDIR@ 
libflext_d_a_CXXFLAGS  = @INCLUDEDIR@ -g -DFLEXT_DEBUG
libflext_t_a_CXXFLAGS  = @OPT_FLAGS@ @INCLUDEDIR@ -DFLEXT_THREADS 
libflext_td_a_CXXFLAGS = @INCLUDEDIR@ -g -DFLEXT_DEBUG -DFLEXT_THREADS 
libflext_la_CXXFLAGS   = @OPT_FLAGS@ @INCLUDEDIR@ -DFLEXT_SHARED \
	-DFLEXT_EXPORTS 
libflext_d_la_CXXFLAGS = @INCLUDEDIR@ -g -DFLEXT_DEBUG -DFLEXT_SHARED \
	-DFLEXT_EXPORTS 

libflext_la_LDFLAGS   = @DYNAMIC_LDFLAGS@ $(LIB_STK) $(LIB_SNDOBJ)
libflext_d_la_LDFLAGS = @DYNAMIC_LDFLAGS@ $(LIB_STK) $(LIB_SNDOBJ)
