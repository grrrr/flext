##############################################

LDFLAGS += -dynamic -Wl,-x -framework ApplicationServices -framework vecLib

##############################################

ifdef DEBUG
CFLAGS += -g -D_DEBUG
else
CFLAGS += $(OFLAGS) -DNDEBUG
LDFLAGS += -Wl,-S
endif
