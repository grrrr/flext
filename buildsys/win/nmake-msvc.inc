!ifdef MSVCPATH
INCPATH=$(INCPATH) /I$(MSVCPATH)\include 
LIBPATH=$(LIBPATH) /LIBPATH:$(MSVCPATH)\lib
!endif

##############################################

OBJPATH=$(OUTPATH)\$(OUTSUB)
TARGETPATH=$(OBJPATH)
TARGET=$(TARGETPATH)\$(OUTNAME).$(EXT)

##############################################
# use precompiled headers (automatic mode)

!ifdef PRECOMPILE
CFLAGS=$(CFLAGS) /YX$(PRECOMPILE) /Fp$(OBJPATH)\precompiled.pch
!endif

##############################################

# create code for DLL
CFLAGS=$(CFLAGS) /LD

# enable exception handling
CFLAGS=$(CFLAGS) /GX

# no banner
CFLAGS=$(CFLAGS) /nologo
LDFLAGS=$(LDFLAGS) /nologo

##############################################

CFLAGS=$(CFLAGS) $(UFLAGS)

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
