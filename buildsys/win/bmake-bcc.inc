!ifdef BCCPATH
INCPATH=$(INCPATH) -I$(BCCPATH)\include 
LIBPATH=$(LIBPATH) -L$(BCCPATH)\lib
!endif

##############################################

OBJPATH=$(OUTPATH)\$(OUTSUB)
TARGETPATH=$(OBJPATH)
TARGET=$(TARGETPATH)\$(OUTNAME).$(EXT)

##############################################
# use precompiled headers

!ifndef PRECOMPILE
CFLAGS=$(CFLAGS) -Hh=flext.h
!else
CFLAGS=$(CFLAGS) -Hh=$(PRECOMPILE)
!endif
# location of precompiled file
CFLAGS=$(CFLAGS) -H=$(OBJPATH)\precompiled.pch

##############################################

LIBS=$(LIBS) cw32.lib import32.lib C0D32.OBJ
CFLAGS=$(CFLAGS) -tWD -tWM -w-8004 -w-8027 -w-8057
LDFLAGS=$(LDFLAGS) /C /Tpd 

##############################################

CFLAGS=$(CFLAGS) $(UFLAGS)

!ifdef DEBUG
CFLAGS=$(CFLAGS) -v -D_DEBUG
LDFLAGS=$(LDFLAGS) /v
!else
CFLAGS=$(CFLAGS) $(OFLAGS) -DNDEBUG
!endif

##############################################
# convert both *.c and *.cpp

OBJSTMP= $(SRCS:.c=.obj)
OBJS= $(OBJSTMP:.objpp=.obj)
