!ifndef FLEXTBUILD
INCPATH=$(INCPATH) /I$(FLEXTPATH)
LIBPATH=$(LIBPATH) /LIBPATH:$(FLEXTPATH)
LIBS=$(LIBS) $(FLEXTNAME).lib
!endif

!ifdef MSVCPATH
INCPATH=$(INCPATH) /I$(MSVCPATH)\include 
LIBPATH=$(LIBPATH) /LIBPATH:$(MSVCPATH)\lib
!endif

LIBS=$(LIBS) user32.lib kernel32.lib

# for VC7 - enable exception handling
CFLAGS=$(CFLAGS) /EHsc 

##############################################

!ifdef DEBUG
CFLAGS=$(CFLAGS) /D_DEBUG /DMTd
LDFLAGS=$(LDFLAGS) /DEBUG
!else
CFLAGS=$(CFLAGS) $(OFLAGS) /DNDEBUG /DMT
LDFLAGS=$(LDFLAGS) /INCREMENTAL:NO 
!endif

##############################################

!ifdef FLEXTBUILD
OUTNAME=$(FLEXTNAME)

!ifdef SHARED
EXT=dll
!else
EXT=lib
BUILDLIB=1
!endif

!else
OUTNAME=$(NAME)
!endif

##############################################

TARGETPATH=$(OUTPATH)\$(OUTSUB)
TARGET=$(TARGETPATH)\$(OUTNAME).$(EXT)

##############################################

# default target
_all_: $(TARGETPATH) $(TARGET)

OBJS= $(SRCS:.c=.obj)
OBJS= $(OBJS:.objpp=.obj)

$(TARGETPATH):
	-mkdir $(OUTPATH) > nul
	-mkdir $@ > nul

{$(SRCDIR)}.cpp{}.obj:
	cl /c /LD $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(TARGETPATH)\$@

{$(SRCDIR)}.c{}.obj:
	cl /c /LD $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(TARGETPATH)\$@

$(TARGET): $(OBJS)
	@cd $(TARGETPATH)
!ifdef BUILDLIB
	lib /OUT:$(@F) $**
!else
	link /DLL $(LDFLAGS) $(LIBPATH) $** $(LIBS) /out:$(@F)
!endif

##############################################

# remove build
_clean_:
	pushd $(TARGETPATH)
	-del /q $(OBJS) > nul
	-popd
	-del /q $(TARGET) > nul

$(INSTPATH):
	-mkdir $(INSTPATH) > nul

$(FLEXTPATH):
	-mkdir $(FLEXTPATH) > nul

# install build
_install_: $(INSTPATH) $(FLEXTPATH)
!ifdef FLEXTBUILD
	-copy $(TARGET) $(FLEXTPATH) > nul
	@cd $(SRCDIR)
	-for %%i in ($(HDRS)) do @copy %%i $(FLEXTPATH) > nul	
!else
	copy $(TARGET) $(INSTPATH) > nul
!endif
