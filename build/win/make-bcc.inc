INCPATH=$(INCPATH) -I$(FLEXTPATH)
LIBPATH=$(LIBPATH) -L$(FLEXTPATH)

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

!ifdef SHARED
# --- shared ---
DEFS=$(DEFS) -DFLEXT_SHARED

!else
!ifdef THREADED
# --- static multi-threaded ---
DEFS=$(DEFS) -DFLEXT_THREADS

!else 
# --- static single-threaded ---

!endif
!endif

##############################################

TARGET=$(OUTPATH)\$(NAME).$(EXT)

# default target
all: $(OUTPATH) print $(TARGET)

# convert both *.c and *.cpp
OBJSTMP= $(SRCS:.c=.obj)
OBJS= $(OBJSTMP:.objpp=.obj)

print:
    echo $(OBJS)

$(OUTPATH):
	@-if not exist $< mkdir $<

SETUPFUNCTION=$(NAME)_setup

$(OUTPATH)\$(NAME).def:
	@echo EXPORTS $(SETUPFUNCTION) = _$(SETUPFUNCTION) > $<
# this next line fixes a strange problem with implib - lacking underscore?!
	@echo IMPORTS _rtext_retext=PD.rtext_retext >> $<

{$(SRCDIR)}.cpp{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(OUTPATH) $<

{$(SRCDIR)}.c{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(OUTPATH) $<

$(TARGET) :: $(OUTPATH) $(OUTPATH)\$(NAME).def

$(TARGET) :: $(OBJS)
	cd $(OUTPATH)
	ilink32 $(LDFLAGS) $(LIBPATH) $** ,..\$<,,$(LIBS),$(NAME).def
	cd ..

$(INSTPATH):
	-mkdir $@

# remove build
clean:
	-del /q $(OUTPATH) > nul
	-rmdir $(OUTPATH) > nul

# install build
install: $(INSTPATH) $(TARGET)
	copy $(TARGET) $(INSTPATH) > nul
