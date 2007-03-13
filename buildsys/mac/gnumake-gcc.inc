##############################################

ifneq (,$(findstring Frameworks,$(FLEXTPREFIX)))
FLEXTFRAMEWORK := $(FLEXTPREFIX)/flext.framework
endif

ifdef FLEXTFRAMEWORK
ifndef FLEXTLIB
FLEXTLIB := $(FLEXTFRAMEWORK)
endif
ifndef FLEXTINC
FLEXTINC := $(FLEXTFRAMEWORK)/Headers
endif
ifndef FLEXTSYS
FLEXTSYS := $(FLEXTFRAMEWORK)/Resources
endif
else
ifndef FLEXTLIB
FLEXTLIB := $(FLEXTPREFIX)/lib
endif
ifndef FLEXTINC
FLEXTINC := $(FLEXTPREFIX)/include/flext
endif
ifndef FLEXTSYS
FLEXTSYS := $(FLEXTPREFIX)/lib/flext
endif
endif

FLEXTBIN := $(FLEXTPREFIX)/bin

##############################################

LDFLAGS += -dynamic -framework ApplicationServices -framework vecLib

##############################################

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
else
ifdef PROFILE
CFLAGS += -g
LDFLAGS += -g
else
LDFLAGS += -Wl,-S
endif
endif

##############################################

ifdef STK_INC
INCPATH += -I$(STK_INC)
endif
ifdef STK_LIB
LIBS += $(STK_LIB)
endif

##############################################

ifdef SNDOBJ_INC
INCPATH += -I$(SNDOBJ_INC)
endif
ifdef SNDOBJ_LIB
LIBS += $(SNDOBJ_LIB)
endif

##############################################

LDFLAGS += $(patsubst %,-arch %,$(ARCH))

ifdef OSXSDK
CFLAGS += -isysroot $(OSXSDK)
LDFLAGS += -isysroot $(OSXSDK)
endif
