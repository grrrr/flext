!ifndef FLEXTBUILD
INCPATH=$(INCPATH) -I$(FLEXTPATH)
LIBPATH=$(LIBPATH) -L$(FLEXTPATH)
LIBS=$(LIBS) $(FLEXTNAME).lib
!endif

!ifdef BCCPATH
INCPATH=$(INCPATH) -I$(BCCPATH)\include 
LIBPATH=$(LIBPATH) -L$(BCCPATH)\lib
!endif

LIBS=$(LIBS) cw32.lib import32.lib C0D32.OBJ
CFLAGS=$(CFLAGS) -tWD -tWM -w-8004 -w-8027 -w-8057
LDFLAGS=$(LDFLAGS) /C /Tpd 

##############################################

# use multithreaded static libraries
!ifdef DEBUG
CFLAGS=$(CFLAGS) -v -D_DEBUG
LDFLAGS=$(LDFLAGS) /v
!else
CFLAGS=$(CFLAGS) $(OFLAGS) -DNDEBUG
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

!ifdef PDPATH

$(TARGETPATH)\pd.lib : $(PDPATH)\bin\pd.dll
	implib -a $< $**

$(TARGETPATH)\pthreadVC.lib : $(PDPATH)\bin\pthreadVC.dll
	implib -a $< $**

PDTARGETS=$(TARGETPATH)\pd.lib $(TARGETPATH)\pthreadVC.lib

!endif

##############################################

# default target
_all_: $(TARGET) 

# convert both *.c and *.cpp
OBJSTMP= $(SRCS:.c=.obj)
OBJS= $(OBJSTMP:.objpp=.obj)

$(TARGETPATH):
	if not exist $(OUTPATH) mkdir $(OUTPATH) > nul
	if not exist $@ mkdir $@ > nul

!ifndef FLEXTBUILD
SETUPFUNCTION=$(NAME)_setup

$(TARGETPATH)\$(NAME).def:
	@echo EXPORTS $(SETUPFUNCTION) = _$(SETUPFUNCTION) > $<
# this next line fixes a strange problem with implib - lacking underscore?!
	@echo IMPORTS _rtext_retext=PD.rtext_retext >> $<

$(TARGET) :: $(TARGETPATH)\$(NAME).def
!endif

{$(SRCDIR)}.cpp{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(TARGETPATH) $<

{$(SRCDIR)}.c{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(TARGETPATH) $<

$(TARGET) :: $(TARGETPATH) $(PDTARGETS)

$(TARGET) :: $(OBJS)
	cd $(TARGETPATH)
!ifdef BUILDLIB
	-del $(<F)
	tlib "$(<F)" +$(OBJS: = +)
	cd ..
!else
	ilink32 $(LDFLAGS) $(LIBPATH) -L. $(OBJS) ,$(<F),,$(LIBS),$(NAME).def
!endif
	cd ..

##############################################

_clean_:
	-del /q $(TARGET) > nul
	-cd $(TARGETPATH)
	-del /q $(OBJS) > nul

##############################################

$(INSTPATH):
	if not exist $@ mkdir $@ > nul

$(FLEXTPATH):
	if not exist $@ mkdir $@ > nul

_install_ :: $(INSTPATH) $(FLEXTPATH)

_install_ ::
!ifdef FLEXTBUILD
	-copy $(TARGET) $(FLEXTPATH) > nul
	@cd $(SRCDIR)
	-for %%i in ($(HDRS)) do @copy %%i $(FLEXTPATH) > nul	
!else
	copy $(TARGET) $(INSTPATH) > nul
!endif
