!ifdef MSVCPATH
INCPATH=$(INCPATH) /I$(MSVCPATH)\include 
LIBPATH=$(LIBPATH) /LIBPATH:$(MSVCPATH)\lib
!endif

##############################################

TARGETPATH=$(OUTPATH)\$(OUTSUB)
TARGET=$(TARGETPATH)\$(OUTNAME).$(EXT)

##############################################

# for VC7 - enable exception handling
CFLAGS=$(CFLAGS) /EHsc 

# use precompiled headers (automatic mode)
!ifndef PRECOMPILE
CFLAGS=$(CFLAGS) /YXflext.h
!else
CFLAGS=$(CFLAGS) /YX$(PRECOMPILE)
!endif
# location of precompiled file
CFLAGS=$(CFLAGS) /Fp$(TARGETPATH)\precompiled.pch

##############################################

!ifdef DEBUG
CFLAGS=$(CFLAGS) /D_DEBUG /DMTd
LDFLAGS=$(LDFLAGS) /DEBUG
!else
CFLAGS=$(CFLAGS) $(OFLAGS) /DNDEBUG /DMT
LDFLAGS=$(LDFLAGS) /INCREMENTAL:NO 
!endif

##############################################
# convert both *.c and *.cpp

OBJSTMP= $(SRCS:.c=.obj)
OBJS= $(OBJSTMP:.objpp=.obj)
