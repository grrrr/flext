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

# enable exception handling and RTTI
CFLAGS=$(CFLAGS) /EHsc /GR

# no banner
CFLAGS=$(CFLAGS) /nologo
LDFLAGS=$(LDFLAGS) /nologo

##############################################

!ifdef DEBUG

!ifdef DYNAMIC
CFLAGS=$(CFLAGS) /MDd
!else
CFLAGS=$(CFLAGS) /MTd
!endif

LDFLAGS=$(LDFLAGS) /DEBUG

!else

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
