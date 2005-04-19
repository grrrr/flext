##############################################

ifneq (,$(findstring Frameworks,$(FLEXTPREFIX)))
FLEXTFRAMEWORK=$(FLEXTPREFIX)/flext.framework
endif

ifdef FLEXTFRAMEWORK
FLEXTLIB=$(FLEXTFRAMEWORK)
FLEXTINC=$(FLEXTFRAMEWORK)/Headers
FLEXTSYS=$(FLEXTFRAMEWORK)/Resources
else
FLEXTLIB=$(FLEXTPREFIX)/lib
FLEXTINC=$(FLEXTPREFIX)/include/flext
FLEXTSYS=$(FLEXTPREFIX)/lib/flext
endif

FLEXTBIN=$(FLEXTPREFIX)/bin

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
LDFLAGS += -Wl,-x -Wl,-S
endif
endif

