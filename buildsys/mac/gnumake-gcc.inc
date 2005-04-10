##############################################

FLEXTINC=$(FLEXTPREFIX)/include/flext
FLEXTLIB=$(FLEXTPREFIX)/lib
FLEXTSYS=$(FLEXTPREFIX)/lib/flext
FLEXTBIN=$(FLEXTPREFIX)/bin

ifneq (,$(findstring Frameworks,$(FLEXTLIB)))
FLEXTFRAMEWORK=$(FLEXTLIB)/flext.framework
endif

##############################################

LDFLAGS += -dynamic -Wl,-x -framework ApplicationServices -framework vecLib

##############################################

ifdef DEBUG
CFLAGS += -g
else
LDFLAGS += -Wl,-S
endif
