INCPATH=$(INCPATH) /I$(FLEXTPATH)
LIBPATH=$(LIBPATH) /LIBPATH:$(FLEXTPATH)

!ifdef MSVCPATH
INCPATH=$(INCPATH) /I$(MSVCPATH)\include 
LIBPATH=$(LIBPATH) /LIBPATH:$(MSVCPATH)\lib
!endif

# add user flags
CFLAGS=$(UFLAGS)

# for VC7 - exception handling
CFLAGS=$(CFLAGS) /EHsc 

##############################################

# use multithreaded static libraries
!ifdef _DEBUG
CFLAGS=$(CFLAGS) /DMTd
!else
CFLAGS=$(CFLAGS) /DMT
!endif

!ifdef FLEXT_SHARED
# --- shared ---
DEFS=$(DEFS) /DFLEXT_SHARED

!elseifdef FLEXT_THREADED
# --- static multi-threaded ---
DEFS=$(DEFS) /DFLEXT_THREADS

!else 
# --- static single-threaded ---

!endif

##############################################

all: $(OUTPATH) $(OUTPATH)\$(NAME).$(EXT)

# remove build
clean:
	-del /q $(OUTPATH) > nul
	-rmdir $(OUTPATH) > nul

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
	link /DLL $(LDFLAGS) /out:$(NAME).$(EXT) /INCREMENTAL:NO $** $(LIBS) $(LIBPATH)
	@-del *.exp
	@-del *.lib
	cd ..
!ifdef INSTPATH
	@-if not exist $(INSTPATH) mkdir $(INSTPATH)
	copy $@ $(INSTPATH) > nul
!endif
