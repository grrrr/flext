#
# automake template
# added by tim blechmann
#

lib_LIBRARIES = libflext-pd_s.a libflext-pd_sd.a libflext-pd_t.a libflext-pd_td.a 

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

pkginclude_HEADERS = \
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


# handling for stk / sndobj
SRCS_STK = flstk.cpp
HDRS_STK = flstk.cpp

SRCS_SNDOBJ = flsndobj.cpp
HDRS_SNDOBJ = flsndobj.cpp

if SNDOBJ
SRCS_FLEXT         += $(SRCS_SNDOBJ)
pkginclude_HEADERS += $(HDRS_SNDOBJ)
LIB_SNDOBJ         = -lsndobj
endif

if STK
SRCS_FLEXT         += $(SRCS_STK)
pkginclude_HEADERS += $(HDRS_STK)
LIB_STK            =  -lstk
endif

FRAMEWORKS = @FRAMEWORKS@

# for static libraries we can use automake
libflext_pd_s_a_SOURCES    = $(SRCS_FLEXT)
libflext_pd_sd_a_SOURCES  = $(SRCS_FLEXT)
libflext_pd_t_a_SOURCES  = $(SRCS_FLEXT)
libflext_pd_td_a_SOURCES = $(SRCS_FLEXT)

libflext_pd_s_a_CXXFLAGS    = @OPT_FLAGS@ @INCLUDEDIR@ 
libflext_pd_sd_a_CXXFLAGS  = @INCLUDEDIR@ -g -DFLEXT_DEBUG
libflext_pd_t_a_CXXFLAGS  = @OPT_FLAGS@ @INCLUDEDIR@ -DFLEXT_THREADS 
libflext_pd_td_a_CXXFLAGS = @INCLUDEDIR@ -g -DFLEXT_DEBUG -DFLEXT_THREADS 

libflext_pd_s_a_LDFLAGS    = @OPT_FLAGS@ @INCLUDEDIR@ \
	$(patsubst %,-framework %,$(FRAMEWORKS))
libflext_pd_sd_a_LDFLAGS  = @INCLUDEDIR@ -g -DFLEXT_DEBUG \
	$(patsubst %,-framework %,$(FRAMEWORKS))
libflext_pd_t_a_LDFLAGS  = @OPT_FLAGS@ @INCLUDEDIR@ -DFLEXT_THREADS \
	$(patsubst %,-framework %,$(FRAMEWORKS))
libflext_pd_td_a_LDFLAGS = @INCLUDEDIR@ -g -DFLEXT_DEBUG -DFLEXT_THREADS \
	$(patsubst %,-framework %,$(FRAMEWORKS))

# for shared libraries, we can't ...
libflext_pd_la_CXXFLAGS   = @OPT_FLAGS@ @INCLUDEDIR@ -DFLEXT_SHARED \
	-DFLEXT_EXPORTS $(DEFS)
libflext_pd_d_la_CXXFLAGS = @INCLUDEDIR@ -g -DFLEXT_DEBUG -DFLEXT_SHARED \
	-DFLEXT_EXPORTS $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES)
libflext_pd_la_LDFLAGS   = @DYNAMIC_LDFLAGS@ $(LIB_STK) $(LIB_SNDOBJ) $(LDFLAGS) \
	$(patsubst %,-framework %,$(FRAMEWORKS))
libflext_pd_d_la_LDFLAGS = @DYNAMIC_LDFLAGS@ $(LIB_STK) $(LIB_SNDOBJ) $(LDFLAGS)
	$(patsubst %,-framework %,$(FRAMEWORKS))

libflext_pd_la_NAME = libflext-pd.@SOEXT@
libflext_pd_d_la_NAME = libflext-pd_d.@SOEXT@

all-local: $(libflext_pd_la_NAME) $(libflext_pd_d_la_NAME)

$(libflext_pd_la_NAME): $(patsubst %.cpp,libflext_pd_la-%.o,$(SRCS_FLEXT))
	$(CXX) $(libflext_pd_la_LDFLAGS) $(LIBS) -o $(libflext_pd_la_NAME) $^

libflext_pd_la-%.o : %.cpp
	$(CXX) -c $(libflext_pd_la_CXXFLAGS) $< -o $@

$(libflext_pd_d_la_NAME): $(patsubst %.cpp,libflext_pd_d_la-%.o,$(SRCS_FLEXT))
	$(CXX) $(libflext_pd_d_la_LDFLAGS) $(LIBS) -o $(libflext_pd_d_la_NAME) $^

libflext_pd_d_la-%.o: %.cpp
	$(CXX) -c $(libflext_pd_d_la_CXXFLAGS) $< -o $@

install-exec-local: 
	$(libLIBRARIES_INSTALL) $(libflext_pd_la_NAME) $(DESTDIR)$(libdir)
	$(libLIBRARIES_INSTALL) $(libflext_pd_d_la_NAME) $(DESTDIR)$(libdir)

