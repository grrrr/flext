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

LDFLAGS += -dynamic -Wl,-x -framework ApplicationServices -framework vecLib

##############################################

ifdef DEBUG
CFLAGS += -g
else
LDFLAGS += -Wl,-S
endif
