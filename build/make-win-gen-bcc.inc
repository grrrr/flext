INCPATH=$(INCPATH) -I$(FLEXTPATH)
LIBPATH=$(LIBPATH) -L$(FLEXTPATH)

!ifdef BCCPATH
INCPATH=$(INCPATH) -I$(BCCPATH)\include 
LIBPATH=$(LIBPATH) -L$(BCCPATH)\lib
!endif

LIBS=$(LIBS) cw32.lib import32.lib C0D32.OBJ 
CFLAGS=$(CFLAGS) -tWD

##############################################

# use multithreaded static libraries
!ifdef DEBUG
CFLAGS=$(CFLAGS) /D_DEBUG
!else
CFLAGS=$(CFLAGS) $(OFLAGS) /DNDEBUG
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

TARGET=$(OUTPATH)\$(NAME).$(EXT)

# default target
all: $(OUTPATH) 

OBJS= $(SRCS:.c=.obj)
OBJS= $(OBJS:.objpp=.obj)

$(OUTPATH):
	-@if not exist $< mkdir $<

$(OUTPATH)\pd.lib: $(PDPATH)\bin\pd.dll 
	implib -a $< $**

SETUPFUNCTION=$(NAME)_setup

$(OUTPATH)\$(NAME).def:
	@echo EXPORTS $(SETUPFUNCTION) = _$(SETUPFUNCTION) > $<
# this next line fixes a strange problem with implib - lacking underscore?!
	@echo IMPORTS _rtext_retext=PD.rtext_retext >> $<

{$(SRCDIR)}.cpp{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(OUTPATH) $<

{$(SRCDIR)}.c{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(OUTPATH) $<

$(TARGET) :: $(OUTPATH) $(OUTPATH)\$(NAME).def $(OUTPATH)\pd.lib 

$(TARGET) :: $(OBJS)
	cd $(OUTPATH)
	ilink32 -C -Tpd $(LIBPATH) $** ,..\$<,,$(LIBS),$(NAME).def
	cd ..

# remove build
clean:
	-del /q $(OUTPATH) > nul
	-rmdir $(OUTPATH) > nul

# install build
install:
	@-if not exist $(INSTPATH) mkdir $(INSTPATH)
	copy $(TARGET) $(INSTPATH) > nul
