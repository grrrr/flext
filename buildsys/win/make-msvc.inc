!ifndef FLEXTBUILD
INCPATH=$(INCPATH) /I$(FLEXTPATH)
LIBPATH=$(LIBPATH) /LIBPATH:$(FLEXTPATH)
LIBS=$(LIBS) $(FLEXTNAME).lib
!endif

!ifdef MSVCPATH
INCPATH=$(INCPATH) /I$(MSVCPATH)\include 
LIBPATH=$(LIBPATH) /LIBPATH:$(MSVCPATH)\lib
!endif

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
_all_: $(TARGET)

# convert both *.c and *.cpp
OBJSTMP= $(SRCS:.c=.obj)
OBJS= $(OBJSTMP:.objpp=.obj)

$(TARGETPATH):
	if not exist $(OUTPATH) mkdir $(OUTPATH) > nul
	if not exist $@ mkdir $@ > nul

{$(SRCDIR)}.cpp{}.obj:
	cl /c /LD $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(TARGETPATH)\$@

{$(SRCDIR)}.c{}.obj:
	cl /c /LD $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(TARGETPATH)\$@

$(TARGET):: $(TARGETPATH)

$(TARGET):: $(OBJS)
	@cd $(TARGETPATH)
!ifdef BUILDLIB
	lib /OUT:$(@F) $(OBJS)
	cd ..
!else
	echo $**
	link /DLL $(LDFLAGS) $(LIBPATH) $(OBJS) $(LIBS) /out:$(@F)
!endif
	cd ..

##############################################

# remove build
_clean_:
	-del /q $(TARGET) > nul
	-cd $(TARGETPATH)
	-del /q $(OBJS) > nul

##############################################

$(INSTPATH):
	if not exist $@ mkdir $@ > nul

$(FLEXTPATH):
	if not exist $@ mkdir $@ > nul

_install_:: $(INSTPATH) $(FLEXTPATH)

_install_::
!ifdef FLEXTBUILD
	-copy $(TARGET) $(FLEXTPATH) > nul
	@cd $(SRCDIR)
	-for %%i in ($(HDRS)) do @copy %%i $(FLEXTPATH) > nul	
!else
	copy $(TARGET) $(INSTPATH) > nul
!endif
