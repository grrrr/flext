INCPATH=$(INCPATH) /I$(FLEXTPATH)
LIBPATH=$(LIBPATH) /LIBPATH:$(FLEXTPATH)

!ifdef MSVCPATH
INCPATH=$(INCPATH) /I$(MSVCPATH)\include 
LIBPATH=$(LIBPATH) /LIBPATH:$(MSVCPATH)\lib
!endif

# for VC7 - enable exception handling
CFLAGS=$(CFLAGS) /EHsc 

##############################################

# use multithreaded static libraries
!ifdef DEBUG
CFLAGS=$(CFLAGS) /D_DEBUG /DMTd
LDFLAGS=$(LDFLAGS) /DEBUG
!else
CFLAGS=$(CFLAGS) $(OFLAGS) /DNDEBUG /DMT
LDFLAGS=$(LDFLAGS) /INCREMENTAL:NO 
!endif

!ifdef SHARED
# --- shared ---
DEFS=$(DEFS) /DFLEXT_SHARED

!elseifdef THREADED
# --- static multi-threaded ---
DEFS=$(DEFS) /DFLEXT_THREADS

!else 
# --- static single-threaded ---

!endif

##############################################

# default target
all: $(OUTPATH) $(OUTPATH)\$(NAME).$(EXT)

OBJS= $(SRCS:.c=.obj)
OBJS= $(OBJS:.objpp=.obj)

$(OUTPATH):
	-mkdir $(OUTPATH) > nul

{$(SRCDIR)}.cpp{}.obj:
	cl /c /LD $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(OUTPATH)\$@

{$(SRCDIR)}.c{}.obj:
	cl /c /LD $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(OUTPATH)\$@

$(OUTPATH)\$(NAME).$(EXT): $(OBJS)
	cd $(OUTPATH)
	link /DLL $(LDFLAGS) /out:$(NAME).$(EXT) $** $(LIBS) $(LIBPATH)
	@-del *.exp
	@-del *.lib
	cd ..

# remove build
clean:
	-del /q $(OUTPATH) > nul
	-rmdir $(OUTPATH) > nul

# install build
install:
	@-if not exist $(INSTPATH) mkdir $(INSTPATH)
	copy $(OUTPATH)\$(NAME).$(EXT) $(INSTPATH) > nul
