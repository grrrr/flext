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

!ifdef PRECOMPILE
CFLAGS=$(CFLAGS) -Hh=$(PRECOMPILE) -H=$(OBJPATH)\precompiled.pch
!endif

##############################################

LIBS=$(LIBS) cw32.lib import32.lib C0D32.OBJ
CFLAGS=$(CFLAGS) -tWD -tWM -w-8004 -w-8027 -w-8057
LDFLAGS=$(LDFLAGS) /C /Tpd 

##############################################

!ifdef DEBUG
CFLAGS=$(CFLAGS) -v
LDFLAGS=$(LDFLAGS) /v
!endif

##############################################
# convert both *.c and *.cpp

OBJSTMP= $(SRCS:.c=.obj)
OBJS= $(OBJSTMP:.objpp=.obj)
