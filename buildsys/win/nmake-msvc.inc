!ifdef MSVCPATH
INCPATH=$(INCPATH) /I$(MSVCPATH)\include 
LIBPATH=$(LIBPATH) /LIBPATH:$(MSVCPATH)\lib
!endif

##############################################

TARGETPATH=$(OUTPATH)\$(OUTSUB)
TARGET=$(TARGETPATH)\$(OUTNAME).$(EXT)

##############################################
# use precompiled headers (automatic mode)

!ifndef PRECOMPILE
CFLAGS=$(CFLAGS) /YXflext.h
!else
CFLAGS=$(CFLAGS) /YX$(PRECOMPILE)
!endif
# location of precompiled file
CFLAGS=$(CFLAGS) /Fp$(TARGETPATH)\precompiled.pch

##############################################

# create code for DLL
CFLAGS=$(CFLAGS) /LD /GD

# enable exception handling
CFLAGS=$(CFLAGS) /GX

# no banner
CFLAGS=$(CFLAGS) /nologo
LDFLAGS=$(LDFLAGS) /nologo

##############################################

!ifdef DEBUG

CFLAGS=$(CFLAGS) /D_DEBUG
!ifdef DYNAMIC
CFLAGS=$(CFLAGS) /MDd
!else
CFLAGS=$(CFLAGS) /MTd
!endif

LDFLAGS=$(LDFLAGS) /DEBUG

!else

CFLAGS=$(CFLAGS) $(OFLAGS) /DNDEBUG
!ifdef DYNAMIC
CFLAGS=$(CFLAGS) /MD
!else
CFLAGS=$(CFLAGS) /MT
!endif

LDFLAGS=$(LDFLAGS) /INCREMENTAL:NO 

!endif

##############################################
# convert both *.c and *.cpp

OBJSTMP= $(SRCS:.c=.obj)
OBJS= $(OBJSTMP:.objpp=.obj)
